/* NTag213 Read/Write protection with new password
 * This program assumes you have virgin NFC NTag213 which is protected with default password.
 * This program will set a new password and protect the tag from unauthorized read and write. 
 * Library - https://github.com/miguelbalboa/rfid 
 *
 * Use above library to understand circuit diagram.
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
  Serial.println(F("Please scan your NFC Tag."));
  
}

void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;


  byte PSWBuff[] = {0xFF, 0xFF, 0xFF, 0xFF}; //32 bit PassWord default FFFFFFFF
  byte pACK[] = {0, 0}; //16 bit PassWord ACK returned by the NFCtag

  status = mfrc522.PCD_NTAG216_AUTH(&PSWBuff[0], pACK);

  if(MFRC522::STATUS_OK !=status){
     Serial.print(F("Authentication failed: "));
     return;
  }
  Serial.println(F("Authentication OK "));


  byte newPWDBuff[] = {0xFF, 0xFF, 0xEE, 0xEE};
  byte newPack[]  = {0xFF, 0xFF, 0, 0};
  Serial.println(mfrc522.MIFARE_Ultralight_Write(0x2B, newPWDBuff, 4)); // 32 bit Password, page 43
  Serial.println(mfrc522.MIFARE_Ultralight_Write(0x2C, newPack, 4));  // 16 bit ACK , page 44

  byte writeProtection[] = {0,0,0,0x06};  // write protect from page 6 
  byte readProtection[] = {0x80,0,0,0};   
  Serial.println(mfrc522.MIFARE_Ultralight_Write(0x29,writeProtection , 4));  // Write protecton, page 41
  Serial.println(mfrc522.MIFARE_Ultralight_Write(0x2A,readProtection, 4));  // Read protection , page 42
  
  
  Serial.println(F("Password modified. Default password won't work anymore."));

  mfrc522.PICC_HaltA();

}
