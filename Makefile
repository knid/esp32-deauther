compile:
	arduino-cli compile -b esp32:esp32:esp32 -p /dev/ttyUSB0

upload:
	arduino-cli upload -b esp32:esp32:esp32 -p /dev/ttyUSB0 

monitor:
	arduino-cli monitor --port /dev/ttyUSB0 --config 115200

all: compile upload monitor
