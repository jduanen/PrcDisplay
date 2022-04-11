EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 3
Title "Mark2 EL Display"
Date "2022-04-08"
Rev "0.0.1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 "MIT License"
$EndDescr
$Comp
L dk_Embedded-Microcontrollers:PIC16F628A-I_P U1
U 1 1 6250F688
P 7500 4750
F 0 "U1" H 7150 5750 60  0000 L CNN
F 1 "PIC16F628A-I_P" H 6800 5650 60  0000 L CNN
F 2 "digikey-footprints:DIP-18_W7.62mm" H 7700 4950 60  0001 L CNN
F 3 "http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en011915" H 7700 5050 60  0001 L CNN
F 4 "PIC16F628A-I/P-ND" H 7700 5150 60  0001 L CNN "Digi-Key_PN"
F 5 "PIC16F628A-I/P" H 7700 5250 60  0001 L CNN "MPN"
F 6 "Integrated Circuits (ICs)" H 7700 5350 60  0001 L CNN "Category"
F 7 "Embedded - Microcontrollers" H 7700 5450 60  0001 L CNN "Family"
F 8 "http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en011915" H 7700 5550 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/microchip-technology/PIC16F628A-I-P/PIC16F628A-I-P-ND/529766" H 7700 5650 60  0001 L CNN "DK_Detail_Page"
F 10 "IC MCU 8BIT 3.5KB FLASH 18DIP" H 7700 5750 60  0001 L CNN "Description"
F 11 "Microchip Technology" H 7700 5850 60  0001 L CNN "Manufacturer"
F 12 "Active" H 7700 5950 60  0001 L CNN "Status"
	1    7500 4750
	1    0    0    -1  
$EndComp
$Comp
L Device:Battery BT?
U 1 1 62510575
P 1300 6450
F 0 "BT?" H 1408 6496 50  0000 L CNN
F 1 "Battery" H 1408 6405 50  0000 L CNN
F 2 "" V 1300 6510 50  0001 C CNN
F 3 "~" V 1300 6510 50  0001 C CNN
	1    1300 6450
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x05_Female J2
U 1 1 62512340
P 3500 6700
F 0 "J2" H 3500 7150 50  0000 C CNN
F 1 "PROG_INTF" H 3500 7050 50  0000 C CNN
F 2 "" H 3500 6700 50  0001 C CNN
F 3 "~" H 3500 6700 50  0001 C CNN
	1    3500 6700
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 62518AD2
P 1300 7050
F 0 "#PWR?" H 1300 6800 50  0001 C CNN
F 1 "GND" H 1305 6877 50  0000 C CNN
F 2 "" H 1300 7050 50  0001 C CNN
F 3 "" H 1300 7050 50  0001 C CNN
	1    1300 7050
	1    0    0    -1  
$EndComp
$Comp
L power:+6V #PWR?
U 1 1 625196C5
P 1300 5900
F 0 "#PWR?" H 1300 5750 50  0001 C CNN
F 1 "+6V" H 1315 6073 50  0000 C CNN
F 2 "" H 1300 5900 50  0001 C CNN
F 3 "" H 1300 5900 50  0001 C CNN
	1    1300 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1300 5900 1300 6250
Wire Wire Line
	1300 6650 1300 7050
$Comp
L power:GND #PWR?
U 1 1 6251DB06
P 7700 5950
F 0 "#PWR?" H 7700 5700 50  0001 C CNN
F 1 "GND" H 7705 5777 50  0000 C CNN
F 2 "" H 7700 5950 50  0001 C CNN
F 3 "" H 7700 5950 50  0001 C CNN
	1    7700 5950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 5850 7700 5950
$Comp
L power:GND #PWR?
U 1 1 6251DE42
P 4150 7150
F 0 "#PWR?" H 4150 6900 50  0001 C CNN
F 1 "GND" H 4155 6977 50  0000 C CNN
F 2 "" H 4150 7150 50  0001 C CNN
F 3 "" H 4150 7150 50  0001 C CNN
	1    4150 7150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 6900 4150 7150
Wire Wire Line
	4150 6300 4150 6500
$Comp
L power:+6V #PWR?
U 1 1 62520C9A
P 7700 3550
F 0 "#PWR?" H 7700 3400 50  0001 C CNN
F 1 "+6V" H 7715 3723 50  0000 C CNN
F 2 "" H 7700 3550 50  0001 C CNN
F 3 "" H 7700 3550 50  0001 C CNN
	1    7700 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 3550 7700 3750
Wire Wire Line
	5700 5150 6800 5150
Wire Wire Line
	3700 6900 4150 6900
Text Label 4000 6500 2    50   ~ 0
Vcc
Text Label 4000 6600 2    50   ~ 0
Vpp
Text Label 4000 6700 2    50   ~ 0
PGC
Text Label 4000 6800 2    50   ~ 0
PGD
Text Label 4000 6900 2    50   ~ 0
GND
Wire Wire Line
	3700 6500 4150 6500
Wire Wire Line
	6800 5550 6250 5550
Wire Wire Line
	6800 5250 6550 5250
$Sheet
S 6950 1000 1150 1300
U 6257DD29
F0 "ShiftRegisters" 50
F1 "ShiftRegisters.sch" 50
F2 "SRCLK" I L 6950 1950 50 
F3 "RCLK" I L 6950 1750 50 
F4 "SER" I L 6950 1350 50 
F5 "OE" I L 6950 1550 50 
$EndSheet
Wire Wire Line
	6550 1950 6950 1950
Wire Wire Line
	6800 5350 6450 5350
Wire Wire Line
	6450 1750 6950 1750
Wire Wire Line
	6550 1950 6550 5250
Wire Wire Line
	6450 1750 6450 5350
Wire Wire Line
	6950 1350 6250 1350
Wire Wire Line
	6250 1350 6250 5550
$Comp
L power:+6V #PWR?
U 1 1 6251ED66
P 4150 6300
F 0 "#PWR?" H 4150 6150 50  0001 C CNN
F 1 "+6V" H 4165 6473 50  0000 C CNN
F 2 "" H 4150 6300 50  0001 C CNN
F 3 "" H 4150 6300 50  0001 C CNN
	1    4150 6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 5050 5600 5050
Wire Wire Line
	6800 4350 5100 4350
NoConn ~ 6800 4050
NoConn ~ 6800 4150
NoConn ~ 6800 4250
Wire Wire Line
	3700 6600 5100 6600
Wire Wire Line
	3700 6700 5600 6700
Wire Wire Line
	3700 6800 5700 6800
Wire Wire Line
	6950 1550 6350 1550
Wire Wire Line
	6350 1550 6350 5450
Wire Wire Line
	6350 5450 6800 5450
Wire Wire Line
	5100 4350 5100 6600
Wire Wire Line
	5700 5150 5700 6800
Wire Wire Line
	5600 5050 5600 6700
$Comp
L dk_Pushbutton-Switches:GPTS203211B S1
U 1 1 62643EEC
P 5600 2350
F 0 "S1" H 5600 2625 50  0000 C CNN
F 1 "ENTER" H 5600 2534 50  0000 C CNN
F 2 "digikey-footprints:PushButton_12x12mm_THT_GPTS203211B" H 5800 2550 50  0001 L CNN
F 3 "http://switches-connectors-custom.cwind.com/Asset/GPTS203211BR2.pdf" H 5800 2650 60  0001 L CNN
F 4 "CW181-ND" H 5800 2750 60  0001 L CNN "Digi-Key_PN"
F 5 "GPTS203211B" H 5800 2850 60  0001 L CNN "MPN"
F 6 "Switches" H 5800 2950 60  0001 L CNN "Category"
F 7 "Pushbutton Switches" H 5800 3050 60  0001 L CNN "Family"
F 8 "http://switches-connectors-custom.cwind.com/Asset/GPTS203211BR2.pdf" H 5800 3150 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/cw-industries/GPTS203211B/CW181-ND/3190590" H 5800 3250 60  0001 L CNN "DK_Detail_Page"
F 10 "SWITCH PUSHBUTTON SPST 1A 30V" H 5800 3350 60  0001 L CNN "Description"
F 11 "CW Industries" H 5800 3450 60  0001 L CNN "Manufacturer"
F 12 "Active" H 5800 3550 60  0001 L CNN "Status"
	1    5600 2350
	-1   0    0    -1  
$EndComp
$Comp
L dk_Pushbutton-Switches:GPTS203211B S2
U 1 1 62644889
P 4650 2350
F 0 "S2" H 4650 2625 50  0000 C CNN
F 1 "UP" H 4650 2534 50  0000 C CNN
F 2 "digikey-footprints:PushButton_12x12mm_THT_GPTS203211B" H 4850 2550 50  0001 L CNN
F 3 "http://switches-connectors-custom.cwind.com/Asset/GPTS203211BR2.pdf" H 4850 2650 60  0001 L CNN
F 4 "CW181-ND" H 4850 2750 60  0001 L CNN "Digi-Key_PN"
F 5 "GPTS203211B" H 4850 2850 60  0001 L CNN "MPN"
F 6 "Switches" H 4850 2950 60  0001 L CNN "Category"
F 7 "Pushbutton Switches" H 4850 3050 60  0001 L CNN "Family"
F 8 "http://switches-connectors-custom.cwind.com/Asset/GPTS203211BR2.pdf" H 4850 3150 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/cw-industries/GPTS203211B/CW181-ND/3190590" H 4850 3250 60  0001 L CNN "DK_Detail_Page"
F 10 "SWITCH PUSHBUTTON SPST 1A 30V" H 4850 3350 60  0001 L CNN "Description"
F 11 "CW Industries" H 4850 3450 60  0001 L CNN "Manufacturer"
F 12 "Active" H 4850 3550 60  0001 L CNN "Status"
	1    4650 2350
	-1   0    0    -1  
$EndComp
$Comp
L dk_Pushbutton-Switches:GPTS203211B S3
U 1 1 62644E14
P 3750 2350
F 0 "S3" H 3750 2625 50  0000 C CNN
F 1 "DOWN" H 3750 2534 50  0000 C CNN
F 2 "digikey-footprints:PushButton_12x12mm_THT_GPTS203211B" H 3950 2550 50  0001 L CNN
F 3 "http://switches-connectors-custom.cwind.com/Asset/GPTS203211BR2.pdf" H 3950 2650 60  0001 L CNN
F 4 "CW181-ND" H 3950 2750 60  0001 L CNN "Digi-Key_PN"
F 5 "GPTS203211B" H 3950 2850 60  0001 L CNN "MPN"
F 6 "Switches" H 3950 2950 60  0001 L CNN "Category"
F 7 "Pushbutton Switches" H 3950 3050 60  0001 L CNN "Family"
F 8 "http://switches-connectors-custom.cwind.com/Asset/GPTS203211BR2.pdf" H 3950 3150 60  0001 L CNN "DK_Datasheet_Link"
F 9 "/product-detail/en/cw-industries/GPTS203211B/CW181-ND/3190590" H 3950 3250 60  0001 L CNN "DK_Detail_Page"
F 10 "SWITCH PUSHBUTTON SPST 1A 30V" H 3950 3350 60  0001 L CNN "Description"
F 11 "CW Industries" H 3950 3450 60  0001 L CNN "Manufacturer"
F 12 "Active" H 3950 3550 60  0001 L CNN "Status"
	1    3750 2350
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 62647140
P 5400 2500
F 0 "#PWR?" H 5400 2250 50  0001 C CNN
F 1 "GND" H 5405 2327 50  0000 C CNN
F 2 "" H 5400 2500 50  0001 C CNN
F 3 "" H 5400 2500 50  0001 C CNN
	1    5400 2500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5400 2350 5400 2500
$Comp
L power:GND #PWR?
U 1 1 62647BB3
P 4450 2500
F 0 "#PWR?" H 4450 2250 50  0001 C CNN
F 1 "GND" H 4455 2327 50  0000 C CNN
F 2 "" H 4450 2500 50  0001 C CNN
F 3 "" H 4450 2500 50  0001 C CNN
	1    4450 2500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4450 2350 4450 2500
$Comp
L power:GND #PWR?
U 1 1 6264881F
P 3550 2500
F 0 "#PWR?" H 3550 2250 50  0001 C CNN
F 1 "GND" H 3555 2327 50  0000 C CNN
F 2 "" H 3550 2500 50  0001 C CNN
F 3 "" H 3550 2500 50  0001 C CNN
	1    3550 2500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3550 2350 3550 2500
Wire Wire Line
	6800 4950 5800 4950
Wire Wire Line
	4850 2350 4850 2950
Wire Wire Line
	5600 3050 5600 5050
Connection ~ 5600 5050
Connection ~ 5700 5150
Wire Wire Line
	5700 2950 5700 5150
Wire Wire Line
	4250 4650 4250 4450
Wire Wire Line
	4100 4650 4250 4650
Wire Wire Line
	4250 5150 3700 5150
Wire Wire Line
	4250 4850 4250 5150
Wire Wire Line
	4100 4850 4250 4850
Wire Wire Line
	3100 4850 3100 5150
Connection ~ 3100 4850
Wire Wire Line
	3300 4850 3100 4850
Wire Wire Line
	3100 4750 3100 4850
Connection ~ 3100 4750
Wire Wire Line
	3300 4750 3100 4750
Wire Wire Line
	3700 5150 3700 5300
Connection ~ 3700 5150
Wire Wire Line
	3700 5050 3700 5150
Wire Wire Line
	3100 5150 3700 5150
Wire Wire Line
	3100 4650 3100 4750
Wire Wire Line
	3300 4650 3100 4650
$Comp
L Memory_EEPROM:AT24CS32-XHM U2
U 1 1 62543A24
P 3700 4750
F 0 "U2" H 3300 5150 50  0000 C CNN
F 1 "AT24CS32-XHM" H 3300 5050 50  0000 C CNN
F 2 "Package_SO:TSSOP-8_4.4x3mm_P0.65mm" H 3700 4750 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8869-SEEPROM-AT24CS32-Datasheet.pdf" H 3700 4750 50  0001 C CNN
	1    3700 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 4350 3700 4450
$Comp
L power:+6V #PWR?
U 1 1 625215C1
P 3700 4350
F 0 "#PWR?" H 3700 4200 50  0001 C CNN
F 1 "+6V" H 3715 4523 50  0000 C CNN
F 2 "" H 3700 4350 50  0001 C CNN
F 3 "" H 3700 4350 50  0001 C CNN
	1    3700 4350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6251D8A3
P 3700 5300
F 0 "#PWR?" H 3700 5050 50  0001 C CNN
F 1 "GND" H 3705 5127 50  0000 C CNN
F 2 "" H 3700 5300 50  0001 C CNN
F 3 "" H 3700 5300 50  0001 C CNN
	1    3700 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 4450 6800 4450
Wire Wire Line
	4100 4750 6800 4750
Wire Wire Line
	5800 2350 5800 4950
Wire Wire Line
	4850 2950 5700 2950
Wire Wire Line
	3950 2350 3950 3050
Wire Wire Line
	3950 3050 5600 3050
$EndSCHEMATC
