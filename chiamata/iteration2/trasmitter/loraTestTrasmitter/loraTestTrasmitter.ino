int buttonPin = 13;

boolean buttonState = 0;
boolean buttonStateOLD = 0;

int state = 0;
int ledPin = 12;

///////////////// ID of transmitting boards, choose one.//////////////
const word ID = 61309;   // ID di questo trasmettitore
const word ID_RX = 61308;   // ID  del modulo ricevente interno al negozio
/////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  initradio();
}

void loop() {
  stateMachine();
}

void stateMachine() {
  switch (state) {
    case 0: //nothing is happening

      if (checkbutton()) {
        state = 1;
        Serial.println("going to state 1");

        //effettuo la chiamata
        sendData();
        //accendo il led
        digitalWrite(ledPin, HIGH);
      }
      break;
    case 1:
      //aspetto conferma ricezione
      if (receiveData()) {
        state = 2;
        Serial.println("going to state2");
      }
      //se rivevo conferma -> state=2
      break;
    case 2:
      //notifico chiamata ricevuta
      //aspetto e torno a stato 0
      //delay(2000);
      digitalWrite(ledPin, LOW);
       Serial.println("resetting to state0");

      state = 0;
      break;
  }
}

