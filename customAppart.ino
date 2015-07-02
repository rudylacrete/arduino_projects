#include <MySensor.h>  
#include <SPI.h>
#include <MFRC522.h>

unsigned long SLEEP_TIME = 200; // Sleep time between reports (in milliseconds)
#define DIGITAL_INPUT_SENSOR 3   // The digital input you attached your motion sensor.  (Only 2 and 3 generates interrupt!)
#define INTERRUPT DIGITAL_INPUT_SENSOR-2 // Usually the interrupt = pin -2 (on uno/nano anyway)
#define PIR_CHILD_ID 6   // Id of the sensor child
#define RFID_CHILD_ID 7

MySensor gw;
// Initialize motion message
MyMessage msg(PIR_CHILD_ID, V_TRIPPED);
MyMessage msgRfid(RFID_CHILD_ID, V_TRIPPED);

#define RST_PIN    8   // RFID RST PIN
#define SS_PIN    7   // RFID SS PIN

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
boolean systemUnlocked = false;
const byte authorizedUid[] = {0xFF, 0xFF, 0xFF, 0xFF};

void setup()  
{
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  gw.begin();

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Motion Sensor", "1.0");

  pinMode(DIGITAL_INPUT_SENSOR, INPUT);      // sets the motion sensor digital pin as input
  // Register all sensors to gw (they will be created as child devices)
  gw.present(PIR_CHILD_ID, S_MOTION);
  gw.present(RFID_CHILD_ID, S_DOOR);
  
}

void loop()     
{     
  // Read digital motion value
  boolean tripped = digitalRead(DIGITAL_INPUT_SENSOR) == HIGH;
  boolean authorizedTagUid = false;

  if( mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // TODO check tag from EEPROM
    printUid(mfrc522.uid.uidByte);
    authorizedTagUid = compareUid((byte*)authorizedUid, mfrc522.uid.uidByte);
  }

  if(authorizedTagUid == true){
    systemUnlocked = !systemUnlocked;
  }
        
  Serial.println(tripped);
  Serial.println(systemUnlocked);
  gw.send(msg.set(tripped?"1":"0"));  // Send tripped value to gw
  gw.send(msgRfid.set(systemUnlocked?"1":"0"));  // Send tripped value to gw 
 
  // Sleep until interrupt comes in on motion sensor. Send update every two minute. 
  gw.sleep(INTERRUPT, CHANGE, SLEEP_TIME);

}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}

void printUid(byte* uid) {
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < 4; i++) {
    Serial.print(uid[i] < 0x10 ? " 0" : " ");
    Serial.print(uid[i], HEX);
  } 
  Serial.println();
}

boolean compareUid(byte* uid1, byte* uid2) {
  boolean equal = true;
  for(byte i = 0; i < 4; i++) {
    if (uid1[i] != uid2[i]) {
      equal = false;
      break;
    }
  }
  return equal;
}

