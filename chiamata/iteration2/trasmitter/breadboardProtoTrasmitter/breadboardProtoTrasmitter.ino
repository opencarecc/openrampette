#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

////////////////////////////////////////////
/////////////////CONNECTIONS////////////////
////////////////////////////////////////////

int buttonPin = 13;
int ledPin = 12;

////////////////////////////////////////////
/////////////////STATE_MACHINE//////////////
////////////////////////////////////////////

int state = 0;

////////////////////////////////////////////
////////////////LORA_DEVICE_ID//////////////
////////////////////////////////////////////

//const word ID = 61309;
//const word ID_RX = 61308;

//const word ID = 61311;
//const word ID_RX = 61310;

const word ID = 61313;
const word ID_RX = 613012;

////////////////////////////////////////////
/////////////////BLE_&_BEACON///////////////
////////////////////////////////////////////
#define BEACON_MAJOR               "0x0000"
#define BEACON_MINOR               "0x0002"
#define LOCAL_NAME                 "rampette"

int32_t charid_string;
int32_t charid_number;

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
boolean BLE_connected = false;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  initradio();

  initBluetooth();
  setupBeacon();
  setupGatt();
  getGATTList();

  ble.reset(); //you ned to reset the ble after the configuration
  Serial.println();
}

void loop() {
  stateMachine();
  ble.update(200);
}



void stateMachine() {
  switch (state) {
    case 0: //nothing is happening
      analogWrite(ledPin, 0);

      if (checkbutton()) {
        state = 1;
        resetTimer();
        Serial.println("Going to state 1");
        //effettuo la chiamata
        sendData();
      }
      break;
    case 1:
      fadeLed();
      //aspetto conferma ricezione

      if (receiveData()) {
        state = 2;
        resetTimer();
        Serial.println("Going to state 2");
        writeCharacteristic(1, 2);
        readCharacteristic(1);
      } if (timeout()) {
        Serial.println("TIMEOUT: Going to state 0");
        writeCharacteristic(2, 0);
        state = 0;
      }

      break;
    case 2:
      digitalWrite(ledPin, HIGH);

 //     this piece was introduced to try to read the value received by the app
//      readCharacteristic(2);
//      String returnMsg=readBleResponse();
//      Serial.println(">>>>>>>>>>");
//      Serial.println(returnMsg);
//      Serial.println("<<<<<<<<<<");
//
//      lookFor4(returnMsg);
//      
      if (timeout()) {
        Serial.println("Going to state 0");
        state = 0;
        ble.reset();
      }
      break;
  }
}

void BLE_connected_callback(void) {
  Serial.println( "Connected to BLE");
  Serial.println( "Going to state 1...");
  //accendo il led
  digitalWrite(ledPin, HIGH);
  resetTimer();
  sendData();
  state = 1;
  BLE_connected = true;
}

void BLE_disconnected_callback(void) {
  Serial.println( F("Disconnected") );
  BLE_connected = false;
}





