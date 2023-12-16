#include <SD.h>
#include <Wire.h>


#define DS3231_I2C_ADDRESS 0x68
File myFile;

byte decToBcd(byte val){
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){
  return( (val/16*10) + (val%16) );
}

void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

String getTime(){

  String output = "";

  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor

  switch (dayOfWeek)
  {
    case 1:
      output+=("SUN");
      break;
    case 2:
      output+=("MON");
      break;
    case 3:
      output+=("TUE");
      break;
    case 4:
      output+=("WED");
      break;
    case 5:
      output+=("THU");
      break;
    case 6:
      output+=("FRI");
      break;
    case 7:
      output+=("SAT");
      break;
  }
  output+=(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  output+=(":");
  if (minute<10){
    output+=("0");
  }
  output+=(minute, DEC);
  output+=(":");
  if (second<10){
    output+=("0");
  }
  output+=(second, DEC);
  output+=(" ");
  output+=(dayOfMonth, DEC);
  output+=("-");
  output+=(month, DEC);
  output+=("-");
  output+=(year, DEC);

  return output;
  
}

bool printTIME(String timeSpam){
  //SD.remove("TIMESPAM.txt");
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    return false;
  }
  myFile = SD.open("TIMESPAM.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(timeSpam);
    myFile.close();
  } else {
    return false;
  }
  return true;
}
 
void setup()
{
  Serial.begin(9600);
  // DS3231 seconds, minutes, hours, day, date, month, year
  s
}

void loop()
{
	String timeStamp = getTime();
  //Serial.println(timeStamp);
  bool stored = printTIME(timeStamp);
  Serial.println(timeStamp);
  if(stored){
    Serial.println("Success");
  }
  else{
    Serial.println("Failure");
  }
  delay(1000);

}