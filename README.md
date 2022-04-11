# PrcDisplay
Mark2 Tin-Foil hat

## Notes

* Use 21x7 LED array from old badge as message board
  - controlled by four 74HC595
* Use Alien Font Display driver board to drive EL wires
  - controlled via I2C
* Use ESP8266 to control LED array and EL wires
  - on power-up/periodically, look for a well-known SSID/passwd (WEP?)
  - connect and serve web page with controls

## TODO

* LED array
  - mounting
  - wiring
* hat
  - buy
* cones
  - wiring jig
  - mounting pattern
* driver
  - get parts
  - build board
  - wiring
* controller
  - LED array code
    * print to LED array
    * scroll/flash
  - cone code
  - web server code
  - wiring
* power supply
  - turn on/off
  - LiPo battery and charger
  - HV PSU (step-up buck converter?)
* web app
  - cone pattern/speed
  - message
  - on/off
  - battery level

