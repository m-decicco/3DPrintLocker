
#include <ArduinoBLE.h>

BLEService ledService("19B10001-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service

// Bluetooth® Low Energy LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEIntCharacteristic switchCharacteristic1("19B10001-E8F2-537E-4F6C-0000000000", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic1("19B10001-E8F2-537E-4F6C-00000000001", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic2("19B10001-E8F2-537E-4F6C-00000000003", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic3("19B10001-E8F2-537E-4F6C-00000000004", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic4("19B10001-E8F2-537E-4F6C-00000000005", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic5("19B10001-E8F2-537E-4F6C-00000000006", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic6("19B10001-E8F2-537E-4F6C-00000000007", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic7("19B10001-E8F2-537E-4F6C-00000000008", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic8("19B10001-E8F2-537E-4F6C-00000000009", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic9("19B10001-E8F2-537E-4F6C-000000000010", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic10("19B10001-E8F2-537E-4F6C-000000000011", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic11("19B10001-E8F2-537E-4F6C-000000000012", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic12("19B10001-E8F2-537E-4F6C-000000000013", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic13("19B10001-E8F2-537E-4F6C-000000000014", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic14("19B10001-E8F2-537E-4F6C-000000000015", BLERead | BLEWrite);
BLEIntCharacteristic switchCharacteristic15("19B10001-E8F2-537E-4F6C-000000000016", BLERead | BLEWrite);

const int ledPin = LED_BUILTIN; // pin to use for the LED

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("MAKERSPACE LOCKER");
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic1);
  ledService.addCharacteristic(switchCharacteristic2);
  ledService.addCharacteristic(switchCharacteristic3);
  ledService.addCharacteristic(switchCharacteristic4);
  ledService.addCharacteristic(switchCharacteristic5);
  ledService.addCharacteristic(switchCharacteristic6);
  ledService.addCharacteristic(switchCharacteristic7);
  ledService.addCharacteristic(switchCharacteristic8);
  ledService.addCharacteristic(switchCharacteristic9);

  // add service
  BLE.addService(ledService);

  // set the initial value for the characeristic:
  switchCharacteristic1.writeValue(1111);
  switchCharacteristic2.writeValue(2222);
  switchCharacteristic3.writeValue(3333);
  switchCharacteristic4.writeValue(4444);
  switchCharacteristic5.writeValue(5555);
  switchCharacteristic6.writeValue(6666);
  switchCharacteristic7.writeValue(7777);
  switchCharacteristic8.writeValue(8888);
  switchCharacteristic9.writeValue(9999);

  // start advertising
  BLE.advertise();

  Serial.println("BLE LED Peripheral");
}

void loop() {
  // listen for Bluetooth® Low Energy peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      /*if (switchCharacteristic.written()) {
        int value = switchCharacteristic.value();
        Serial.println(value);

      if (value == 1) {
        Serial.println("LED on");
        switchCharacteristic.writeValue(1);
        digitalWrite(ledPin, HIGH); // Turn the LED on
      } else if (value == 0) {
        Serial.println("LED off");
        switchCharacteristic.writeValue(0);
        digitalWrite(ledPin, LOW); // Turn the LED off
      }
      
          
      }*/
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  
}
}
