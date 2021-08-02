/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <Adafruit_AW9523.h>
#include <bluefruit.h>

// OTA DFU service
BLEDfu bledfu;

//expander code
Adafruit_AW9523 aw;

uint8_t LedPin = 0;  // 0 thru 15
uint8_t sol1 = 1;
uint8_t sol2 = 2;
uint8_t sol3 = 3;
uint8_t sol4 = 4;
uint8_t sol5 = 5;
uint8_t sol6 = 6;
uint8_t sol7 = 7;

// Uart over BLE service
BLEUart bleuart;

// Function prototypes for packetparser.cpp
uint8_t readPacket (BLEUart *ble_uart, uint16_t timeout);
float   parsefloat (uint8_t *buffer);
void    printHex   (const uint8_t * data, const uint32_t numBytes);

// pins for the solenoid control
const int sol_1 = 17; //15 is actually A0
const int sol_2 = 18; //15 is actually A1
int myPins[] = {17, 18};

// Packet buffer
extern uint8_t packetbuffer[];

void setup(void)
{
  //the setup for the solenoids
  //pinMode(7, OUTPUT);
  //pinMode(11, OUTPUT);
  
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println(F("Adafruit Bluefruit52 Controller App Example"));
  Serial.println(F("-------------------------------------------"));

  //Expander initialization
  while (!Serial) delay(1);  // wait for serial port to open
  
  Serial.println("Adafruit AW9523 GPIO Expander test!");

  if (! aw.begin(0x58)) {
    Serial.println("AW9523 not found? Check wiring!");
    while (1) delay(10);  // halt forever
  }

  Serial.println("AW9523 found!");
  aw.pinMode(LedPin, OUTPUT);
  aw.pinMode(sol1, OUTPUT);
  aw.pinMode(sol2, OUTPUT);
  aw.pinMode(sol3, OUTPUT);
  aw.pinMode(sol4, OUTPUT);
  aw.pinMode(sol5, OUTPUT);
  aw.pinMode(sol6, OUTPUT);
  aw.pinMode(sol7, OUTPUT);
  
  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and start the BLE Uart service
  bleuart.begin();

  // Set up and start advertising
  startAdv();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();  
}

void playSong(void)
{
  Serial.println("playSong reached");
  aw.digitalWrite(sol1, HIGH);
  delay(20);
  aw.digitalWrite(sol1, LOW);
  delay(1000);
  aw.digitalWrite(sol2, HIGH);
  delay(20);
  aw.digitalWrite(sol2, LOW);
  delay(1000);
  aw.digitalWrite(sol3, HIGH);
  delay(20);
  aw.digitalWrite(sol3, LOW);
  delay(1000);
  aw.digitalWrite(sol4, HIGH);
  delay(20);
  aw.digitalWrite(sol4, LOW);
  delay(1000);
  aw.digitalWrite(sol5, HIGH);
  delay(20);
  aw.digitalWrite(sol5, LOW);
  delay(1000);
  aw.digitalWrite(sol6, HIGH);
  delay(20);
  aw.digitalWrite(sol6, LOW);
  delay(1000);
  aw.digitalWrite(sol7, HIGH);
  delay(20);
  aw.digitalWrite(sol7, LOW);
  delay(1000);
  
}
void playNote(int note){
  if(note != 8){
    aw.digitalWrite(note, HIGH);
    delay(20);
    aw.digitalWrite(note, LOW);
  }
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.setName("MusicMan");
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  
  // Include the BLE UART (AKA 'NUS') 128-bit UUID
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  // Wait for new data to arrive
  uint8_t len = readPacket(&bleuart, 500);
  if (len == 0) return;

  // Got a packet!
  // printHex(packetbuffer, len);

  // Color
  if (packetbuffer[1] == 'C') {
    uint8_t red = packetbuffer[2];
    uint8_t green = packetbuffer[3];
    uint8_t blue = packetbuffer[4];
    Serial.print ("RGB #");
    if (red < 0x10) Serial.print("0");
    Serial.print(red, HEX);
    if (green < 0x10) Serial.print("0");
    Serial.print(green, HEX);
    if (blue < 0x10) Serial.print("0");
    Serial.println(blue, HEX);
  }

  // Buttons
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    Serial.print ("Button "); Serial.print(buttnum);
    if (pressed) {
      Serial.println(" pressed");
      playNote(buttnum);
      //playSong();
    } else {
      Serial.println(" released");
    }
  }


}
