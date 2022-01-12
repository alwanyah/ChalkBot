#!/bin/sh

set -ex

cd ~

apt-get update
apt-get install wget git -y

wget https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz
tar xf arduino-cli_latest_Linux_64bit.tar.gz
cp arduino-cli /usr/local/bin

arduino-cli version
arduino-cli config init
arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
arduino-cli core install esp32:esp32
arduino-cli lib install "Adafruit BNO055"
arduino-cli lib install "SparkFun u-blox GNSS Arduino Library"

cd Arduino/libraries
git clone https://github.com/me-no-dev/AsyncTCP.git
git clone https://github.com/me-no-dev/ESPAsyncWebServer.git
cd -
