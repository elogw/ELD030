#include <SPI.h>
#include <avr/pgmspace.h>
#include <SdFat.h>
#include <MemoryFree.h>

SdFat sd;
SdFile myFile;

char POWER_CTRL = 0x2D;  //power control register
char DATA_FORMAT = 0x31;
char BW_RATE = 0x2C;
byte *reply;
byte buffer[6];
//char str[512];
int x[140];
int y[140];
int z[140];
int counter1, counter2;
int initialTime, elapsedTime;
const int ADXLSelect = 10;
const int SDSelect = 9;
const int button = 8;

void setup() {
  Serial.begin(9600);

  //Initialise SD card
  Serial.print("Initialising SD card...");
  if (!sd.begin(SDSelect, SPI_HALF_SPEED)) sd.initErrorHalt();
  else(Serial.println("Success!"));
  
  pinMode(ADXLSelect, OUTPUT);
  digitalWrite(ADXLSelect, HIGH);
  pinMode(SDSelect, OUTPUT);
  digitalWrite(SDSelect, HIGH);

  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  
  //Initialise accelerometer
  //00000101 (5) Selects +/- 4g range, MSB mode.
  SPIWrite(DATA_FORMAT, 5);
  //Set Data rate to 3200HZ (highest rate)
  SPIWrite(BW_RATE, 15);
  //Set ADXL345 to measure mode
  SPIWrite(POWER_CTRL, 8);
  
  //Initialise stop button
  pinMode(button, INPUT);
  
}

void loop()  {
  
  if (!myFile.open("datalog.csv", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening datalog.csv for write failed");  
  }
  //Read 6 bytes starting at 0x32 (the X0 register through to)
  //Z1 register
  initialTime = millis();
  for(counter2=0; counter2 < 140; counter2++)  {
    SPIRead (0x32, 6, buffer);
    x[counter2] = (((int)buffer[1]) << 8) | buffer[0]; 
    y[counter2] = (((int)buffer[3]) << 8) | buffer[2];
    z[counter2] = (((int)buffer[5]) << 8) | buffer[4];
    //sprintf(str+(counter2*21), "%d %d %d %d /n", counter2, x, y, z);
    myFile.print(counter2);
    myFile.print(',');
    myFile.print(x[counter2]);
    myFile.print(',');
    myFile.print(y[counter2]);
    myFile.print(',');
    myFile.println(z[counter2]);
  }
  
  //Print time taken for operation
  elapsedTime = millis() - initialTime;
  Serial.print("After acquisition loop : ");
  Serial.println(elapsedTime);


  // open the file for write at end like the Native SD library

  myFile.close();
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  
  elapsedTime = millis() - elapsedTime;
  Serial.print("After write loop : ");
  Serial.println(elapsedTime);
  
  
  
  //If Stop button is pressed, enter infinite loop
  if(digitalRead(button) == HIGH)  {
    Serial.println("Stopping.");
    while(1)  {}
  }
}


void SPIWrite (byte ADXLRegisterAddress, byte value)  {

  digitalWrite(ADXLSelect, LOW);
  //take Slave Select Pin low to select the chip
  SPI.transfer(ADXLRegisterAddress);
  SPI.transfer(value);
  digitalWrite(ADXLSelect, HIGH);
  //take ADXLSelect high to deselect chip
}

void SPIRead (char ADXLRegisterAddress, int numBytes, byte *values) {
  //bitwise OR to set MSB high & 7th bit high. 
  //This is the read bit and Multiple Byte Bit respectively
  ADXLRegisterAddress = 0xC0 | ADXLRegisterAddress;

  
  digitalWrite(ADXLSelect, LOW);
  SPI.transfer(ADXLRegisterAddress);
  for(counter1=0; counter1<numBytes; counter1++)  {
    values[counter1] = SPI.transfer(0x00);
  }
  digitalWrite(ADXLSelect, HIGH);
}

  
  
  

