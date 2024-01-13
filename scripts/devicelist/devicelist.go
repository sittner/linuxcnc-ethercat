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
)

var (
	srcFlag       = flag.String("src", "src/", "Directory that contains LinuxCNC-Ethercat src.")
	esiFlag       = flag.String("esi", "esi.yml", "Path to esi.yml file (should be in scripts/esi.yml)")
	deviceDirFlag = flag.String("device_directory", "", "Path to documentation/devices; skips writing if empty")
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

func (d *DeviceDefinition) GetVendorID() string {
	return d.VendorID
}
func (d *DeviceDefinition) GetPID() string {
	return d.PID
}

type ESIDefinition struct {
	Type       string `yaml:"Type"`
	PID        string `yaml:"ProductCode"`
	RevisionNo string `yaml:"RevisionNo"`
	VendorName string `yaml:"Vendor"`
	VendorID   string `yaml:"VendorID"`
	URL        string `yaml:"URL"`
	Name       string `yaml:"Name"`
	DeviceType string `yaml:"DeviceGroup"`
}

func (e *ESIDefinition) GetVendorID() string {
	return e.VendorID
}
func (e *ESIDefinition) GetPID() string {
	return e.PID
}

func readESIFile(filename string) ([]*ESIDefinition, error) {
	f, err := os.ReadFile(filename)
	if err != nil {
		return nil, err
	}

	esiEntries := []*ESIDefinition{}

	err = yaml.Unmarshal(f, &esiEntries)
	if err != nil {
		return nil, err
	}

	return esiEntries, nil
}

type DeviceInterface interface {
	GetPID() string
	GetVendorID() string
}

func esiKey(entry DeviceInterface) string {
	return fmt.Sprintf("%s:%s", entry.GetVendorID(), entry.GetPID())
}

func buildESIMap(esiEntries []*ESIDefinition) map[string]*ESIDefinition {
	esiMap := make(map[string]*ESIDefinition)

	for _, entry := range esiEntries {
		key := esiKey(entry)
		if esiMap[key] == nil {
			esiMap[key] = entry
		}
	}

	return esiMap
}

func buildVIDMap(src string) (map[string]string, error) {
	vidMap := make(map[string]string)
	vidRE := regexp.MustCompile("^#define (LCEC_[^_ ]+_VID) +([^ ]*)")

	file, err := os.Open(filepath.Join(src, "lcec.h"))
	if err != nil {
		return nil, err
	}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		if match := vidRE.FindStringSubmatch(line); match != nil {
			vidMap[match[1]] = strings.ToLower(match[2])
		}
	}
	return vidMap, nil
}

func main() {
	flag.Parse()

	fmt.Printf("Reading VIDs from lcec.h\n")
	vidMap, err := buildVIDMap(*srcFlag)
	if err != nil {
		log.Fatalf("couldn't build VID map: %v", err)
	}

	fmt.Printf("Reading ESI file from %s\n", *esiFlag)
	esiEntries, err := readESIFile(*esiFlag)
	if err != nil {
		log.Fatalf("couldn't read ESI file (%s): $v", *esiFlag, err)
	}
	fmt.Printf("Found %d entries\n", len(esiEntries))

	esiMap := buildESIMap(esiEntries)

	cFiles := []string{}
	entries, err := os.ReadDir(*srcFlag)
	if err != nil {
		log.Fatalf("couldn't read source directory (%s): %v", *srcFlag, err)
	}

	for _, entry := range entries {
		if strings.HasSuffix(entry.Name(), ".h") && strings.Contains(entry.Name(), "lcec_") {
			cFiles = append(cFiles, filepath.Join(*srcFlag, entry.Name()))
		}
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

	allDefs := []*DeviceDefinition{}

	deviceLines := 0

	for _, filename := range cFiles {
		defs := []*DeviceDefinition{}
		stubs := 0

		fmt.Printf("Looking in %s...", filename)

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

						def := &DeviceDefinition{
							Device:   match[1],
							VendorID: vidMap[match[2]],
							PID:      strings.ToLower(match[3]),
							SrcFile:  srcFile,
						}

						defs = append(defs, def)
					}
				}
			}
		}

		for _, def := range defs {
			esi := esiMap[esiKey(def)]
			if esi == nil {
				//fmt.Printf("(ESI missing for %q, creating stub) ", esiKey(def))
				stubs++
				def.Description = "UNKNOWN"
			} else {
				def.VendorName = esi.VendorName
				vendorSplit := strings.Split(def.VendorName, " ")
				def.Description = fmt.Sprintf("%s %s", vendorSplit[0], esi.Name)
				def.DocumentationURL = esi.URL
				def.DeviceType = esi.DeviceType
			}

			//fmt.Printf("Got def: %#v\n", def)
		}

		fmt.Printf("found %d devices", len(defs))
		if foundTypes == false {
			fmt.Printf(", no types[]");
		}

		if stubs > 0 {
			fmt.Printf(" (%d not found in ESI data)", stubs)
		}
		fmt.Printf("\n")
		allDefs = append(allDefs, defs...)

	}

	for _, def := range allDefs {
		y, err := yaml.Marshal(def)
		if err != nil {
			log.Fatalf("unable to marshal yaml: %v", err)
		}

		if *deviceDirFlag == "" {
			fmt.Println(string(y))
		} else {
			filename := filepath.Join(*deviceDirFlag, fmt.Sprintf("%s.yml", def.Device))

			if _, err := os.Stat(filename); err == nil {
				// file already exists

				// Skip for now.  Maybe merge at some point?
			} else {
				data := []byte("---\n")
				data = append(data, y...)
				err := os.WriteFile(filename, data, 0644)
				if err != nil {
					panic(err)
				}
			}
		}

	}
}
