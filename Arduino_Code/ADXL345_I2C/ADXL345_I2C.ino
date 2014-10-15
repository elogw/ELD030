#include <Wire.h>
#define DEVICE_ADDR (0x53)

byte buff[6];
char str[512];
int startTime = 0;
int elapsedTime = 0;

void setup()  {
  Wire.begin();
  Serial.begin(9600);
  
  writeTo(DEVICE_ADDR, 0x2D, 0);    //Setting up ADXL345
  writeTo(DEVICE_ADDR, 0x2D, 16);
  writeTo(DEVICE_ADDR, 0x2D, 8);
}

void loop()  {
  
  int x, y, z, i;
  
  startTime = millis();
  
  for(i=0; i<1000; i++)  {
  readFrom(DEVICE_ADDR, 0x32, 6, buff); //read the acceleration data from the ADXL345
  
   //each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significat Byte first!!
   //thus we are converting both bytes in to one int
  x = (((int)buff[1]) << 8) | buff[0];   
  y = (((int)buff[3])<< 8) | buff[2];
  z = (((int)buff[5]) << 8) | buff[4];
  }
  elapsedTime = millis() - startTime;
  
  //we send the x y z values as a string to the serial port
  sprintf(str, "%d %d %d", x, y, z);  
  Serial.print(str);
  Serial.write(10);
}

void writeTo  (int device, byte address, byte val)  {
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.write(val);
  Wire.endTransmission();
}

void readFrom(int device, byte address, int num, byte buff[]) {
  Wire.beginTransmission(device); //start transmission to device 
  Wire.write(address);        //sends address to read from
  Wire.endTransmission(); //end transmission
  
  Wire.beginTransmission(device); //start transmission to device (initiate again)
  Wire.requestFrom(device, num);    // request 6 bytes from device
  
  int i = 0;
  while(Wire.available())    //device may send less than requested (abnormal)
  { 
    buff[i] = Wire.read(); // receive a byte
    i++;
  }
  Wire.endTransmission(); //end transmission
}


