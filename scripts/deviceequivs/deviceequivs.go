package main

import (
	"bufio"
	"flag"
	"fmt"
	"gopkg.in/yaml.v3"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"strings"
	"github.com/linuxcnc-ethercat/esi-data/esi"
	"slices"
)

type DeviceDefinition struct {
	Device           string `yaml:"Device"`
	VendorID         string `yaml:"VendorID"`
	VendorName       string `yaml:"VendorName"`
	PID              string `yaml:"PID"`
	Description      string `yaml:"Description"`
	DocumentationURL string `yaml:"DocumentationURL"`
	DeviceType       string `yaml:"DeviceType"`
	Notes            string `yaml:"Notes"`
	SrcFile          string `yaml:"SrcFile"`
	TestingStatus    string `yaml:"TestingStatus"`
}

var (
	srcFlag       = flag.String("src", "src/", "Directory that contains LinuxCNC-Ethercat src.")
	esiFlag       = flag.String("esi", "../../esi-data/esi.yml", "Path to esi-data esi.yml file *not* linuxcnc-ethercat esi file")
)

func readESIFile(filename string) ([]*esi.ESIDevice, error) {
	devices := []*esi.ESIDevice{}

	file, err := os.Open(*esiFlag)
	if err != nil {
		return nil, err
	}

	fmt.Printf("Reading %s...\n", *esiFlag)

	decoder := yaml.NewDecoder(file)
	err = decoder.Decode(&devices)
	if err != nil {
		return nil, err
	}

	return devices, nil
}


func main() {
	flag.Parse()

	esidata, err := readESIFile(*esiFlag)
	if err!=nil {
		panic(err)
	}

	devs := map[string]map[string]*esi.ESIDevice{}
	for _, d := range esidata {
		for _, id := range d.IDs {
			devtype := id.Type
			devrev := id.RevisionNo
			if devs[devtype] == nil {
				devs[devtype] = make(map[string]*esi.ESIDevice)
			}
			devs[devtype][devrev] = d
		}
	}

	cFiles := []string{}
	entries, err := os.ReadDir(*srcFlag)
	if err != nil {
		log.Fatalf("couldn't read source directory (%s): %v", *srcFlag, err)
	}

	entries, err = os.ReadDir(filepath.Join(*srcFlag, "devices"))
	if err != nil {
		log.Fatalf("couldn't read devices directory: %v", err)
	}

	for _, entry := range entries {
		if strings.HasSuffix(entry.Name(), ".c") && strings.Contains(entry.Name(), "lcec_") {
			cFiles = append(cFiles, filepath.Join(*srcFlag, "devices", entry.Name()))
		}
	}

	typesRE := regexp.MustCompile("lcec_typelist_t +types\\[\\] *= *\\{")
	endTypesRE := regexp.MustCompile("{ *NULL *},")
	typelistEntryRE := regexp.MustCompile("^ *{ *\"([^\"]+)\" *, *([A-Za-z0-9_]+) *, *(0x[a-fA-F0-9]+) *,")

	deviceLines := 0

	for _, filename := range cFiles {
		suppressEquivs := map[string]bool{}
		equivs := map[string]*esi.ESIDeviceID{}

		if strings.HasSuffix(filename , "/lcec_passive.c") {
			continue
		}
		
		fmt.Printf("Looking in %s...\n", filename)

		file, err := os.Open(filename)
		if err != nil {
			log.Fatalf("can't open file %q: %v", filename, err)
		}

		foundTypes := false
		scanner := bufio.NewScanner(file)
		for scanner.Scan() {
			line := scanner.Text()

			if foundTypes == false {
				if typesRE.MatchString(line) {
					// Found types[] start.
					foundTypes = true
				}
			} else {
				// foundTypes == true
				if endTypesRE.MatchString(line) {
					// found the end of types[]
					break
				} else {
					// Inside of the types[] block, parse lines.
					if match := typelistEntryRE.FindStringSubmatch(line); match != nil {
						// Found an entry!
						deviceLines++
						srcFile, _, _ := strings.Cut(filename[len(*srcFlag)+1:], ".h")
						srcFile = fmt.Sprintf("src/%s.c", srcFile)


						if devs[match[1]] != nil {
							suppressEquivs[match[1]] = true
							for _, entry := range devs[match[1]] {
								for _, id := range entry.IDs {
									equivs[id.Type] = id
								}
							}
						}
					}
				}
			}
		}

		newDevs := []string{}
		
		for k, v := range equivs {
			if !suppressEquivs[k] {
				newDevs = append(newDevs, fmt.Sprintf("  { %q, VID, %s, ... }", v.Type, v.ProductCode))
			}
		}

		slices.Sort(newDevs)

		for _, d := range newDevs {
			fmt.Printf("%s\n", d)
		}
	}
}
