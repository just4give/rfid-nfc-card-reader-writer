/* This program writes data to a read/write protected NTag213 
 *     
 */

  #include <SPI.h>
  #include <MFRC522.h>


 #define RST_PIN         9           // Configurable, see typical pin layout above
 #define SS_PIN          10          // Configurable, see typical pin layout above

  MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
  MFRC522::StatusCode status; //variable to get card status

  byte buffer[18];  //data transfer buffer (16+2 bytes data+CRC)
  byte size = sizeof(buffer);

  uint8_t pageAddr = 0x06;  //In this example we will write/read 16 bytes (page 6,7,8 and 9).
                            //Ultraligth mem = 16 pages. 4 bytes per page. 
                            //Pages 0 to 4 are for special functions.           

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card 
  Serial.println(F("Sketch has been started!"));
  memcpy(buffer,"HELLO WORLD!  <3",16);
}

void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;


  byte PSWBuff[] = {0xFF, 0xFF, 0xEE, 0xEE}; //32 bit password. Set using NTag213Protection program
  byte pACK[] = {0xFF, 0xFF}; //16 bit PassWord ACK returned by the NFCtag

  status = mfrc522.PCD_NTAG216_AUTH(&PSWBuff[0], pACK);

  if(MFRC522::STATUS_OK !=status){
     Serial.print(F("Authentication failed: "));
     return;
  }
  Serial.println(F("Authentication OK "));


  

  
  // Write data ***********************************************


  for (int i=0; i < 4; i++) {
    //data is writen in blocks of 4 bytes (4 bytes per page)
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Ultralight_Write(pageAddr+i, &buffer[i*4], 4);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Ultralight_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return;
    }
  }
  Serial.println(F("MIFARE_Ultralight_Write() OK "));
  Serial.println();




  // Read data ***************************************************
  Serial.println(F("Reading data ... "));
  //data in 4 block is readed at once.
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(pageAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.print(F("Readed data: "));
  //Dump a byte array to Serial
  for (byte i = 0; i < 16; i++) {
    Serial.write(buffer[i]);
  }
  Serial.println();

  mfrc522.PICC_HaltA();

}
