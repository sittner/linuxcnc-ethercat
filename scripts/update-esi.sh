#!/bin/sh

set -e

esiyml=esi.yml
beckhoffurl="https://www.beckhoff.com/en-en/download/128205835"
omronurl1="https://assets.omron.eu/downloads/ddf/en/v1/r88d-knxxx-ect(-l)_ethercat_esi_file_en.zip"
omronurl2="https://assets.omron.eu/downloads/ddf/en/v3/r88d-1snxxx-ect_ethercat_esi_file_en.zip"
useragent="User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:109.0) Gecko/20100101 Firefox/113.0"

curl -H "$useragent" -L "$beckhoffurl" -o beckhoff.zip
curl -L "$omronurl1" -o omron1.zip
curl -L "$omronurl2" -o omron2.zip

unzip -j beckhoff.zip -d tmpesi
unzip -j omron1.zip -d tmpesi
unzip -j omron2.zip -d tmpesi

cd esidecoder
go build esidecoder.go
cd ..

./esidecoder/esidecoder --esi_directory=tmpesi --output=esi.yml
