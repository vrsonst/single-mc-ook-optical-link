// ____ Pin definitions ____
const int LED = 12;

// ____ Constants ____
const int LCD = 32;
const int MAX_BUFFER = LCD + 3;
const unsigned long BIT_TIME = 500;
const char STX = 0x02;
const char ETX = 0x03;
const int LIGHT_THRESHOLD = 660;

// ____ States ____
const byte IDLE = 0;
const byte START = 1;
const byte DATA = 2;
const byte END = 3;

// ____ Global ____
char TEXT[MAX_BUFFER];
byte t_state = IDLE;
byte r_state = IDLE;
unsigned int textLen = 0;
unsigned int textIndex = 0;
unsigned int textIndexR = 0;
unsigned int bitCounter = 0;
unsigned int bitCounterR = 0;
unsigned int currentReading = 0;
byte receivedBit = 0;
char RECTEXT[MAX_BUFFER];
char rtxt = 0b00000000;
bool STXcheck = 0;
bool ETXcheck = 0;

unsigned long timeNow = 0;
unsigned long timeStart = 0;
unsigned long timeRec = 0;

/* char bits2Byte() {
  if (currentReading < LIGHT_THRESHOLD) {
    receivedBit = 0;
  } else receivedBit = 1;

  receivedBit <<= bitCounter;
  rtxt |= receivedBit;
} */

void transmitter() {
  switch (t_state) {
    case IDLE:
      digitalWrite(LED, LOW);
      if (Serial.available() > 0) {
        // Get input from user
        textLen = Serial.readBytesUntil('\n', TEXT + 1, LCD);
        TEXT[0] = STX;
        TEXT[textLen + 1] = ETX;
        TEXT[textLen + 2] = '\0';  // TEXT = [0x02, H, e, l, l, o,  , W, o, r, l, d, 0x03, \0]
        Serial.println(TEXT);

        timeStart = timeNow;

        textIndex = 0;
        bitCounter = 0;
        t_state = START;
      }
      break;

    case START:
      digitalWrite(LED, HIGH);
      //if (timeNow - timeStart >= BIT_TIME) {
      timeStart = timeNow;
      bitCounter = 0;
      Serial.print(1);
      t_state = DATA;
      //}
      break;

    case DATA:
      if (timeNow - timeStart >= BIT_TIME) {
        timeStart = timeNow;
        if (bitCounter < 8) {
          byte readLSB = (TEXT[textIndex] >> bitCounter) & 1;
          if (readLSB == 1) {
            digitalWrite(LED, HIGH);
            Serial.print(1);
          } else {
            digitalWrite(LED, LOW);
            Serial.print(0);
          }
          bitCounter++;
        } else {
          digitalWrite(LED, LOW);
          //Serial.print(0);
          t_state = END;
        }
      }
      break;

    case END:
      if (timeNow - timeStart >= BIT_TIME) {
        timeStart = timeNow;
        Serial.println();
        textIndex++;
        if (textIndex <= textLen + 1) {
          t_state = START;
        } else {
          t_state = IDLE;
          Serial.println("Enter your message: ");
        }
      }
      break;
  }
}

void receiver() {
  switch (r_state) {
    case IDLE:
      bitCounterR = 0;
      rtxt = 0b00000000;
      currentReading = analogRead(A0);
      if (currentReading > LIGHT_THRESHOLD) {
        timeRec = timeNow;
        r_state = START;
      }
      break;

    case START:
      if (timeNow - timeRec >= BIT_TIME / 2 + BIT_TIME * (bitCounterR + 1)) {
        currentReading = analogRead(A0);

        if (bitCounterR < 8) {
          if (currentReading < LIGHT_THRESHOLD) {
            receivedBit = 0;
          } else receivedBit = 1;

          receivedBit <<= bitCounterR;
          rtxt |= receivedBit;
          bitCounterR++;
        }
        if (bitCounterR == 8) {
          r_state = DATA;
        }
      }
      break;

    case DATA:
      bitCounterR = 0;
      //Serial.println(rtxt);
      if (rtxt == ETX) {
        ETXcheck = 1;
        STXcheck = 0;
        r_state = END;
        rtxt = 0b00000000;
        break;
      }

      if (rtxt == STX) {
        STXcheck = 1;
        ETXcheck = 0;
        rtxt = 0b00000000;
        r_state = IDLE;
        break;
      }
      if (STXcheck == 1 && ETXcheck == 0) {
        RECTEXT[textIndexR] = rtxt;
        textIndexR++;
        rtxt = 0b00000000;
        r_state = IDLE;
      } else r_state = IDLE;
      break;

    case END:
      RECTEXT[textIndexR] = '\0';
      Serial.println();
      Serial.println(RECTEXT);
      textIndexR = 0;
      r_state = IDLE;
      break;
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.println();
  Serial.print("Enter your message: ");
}

void loop() {
  timeNow = millis();
  transmitter();
  receiver();
}