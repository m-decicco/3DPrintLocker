#define totalLockers 16
#define smallLockers 5
#define mediumLockers 9
#define largeLockers 2
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define SERVOOPEN 480 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOCLOSE  420 // This is the 'maximum' pulse length count (out of 4096)
#define Battery A1
#include "Adafruit_Keypad.h"
#include <math.h>
#include <EEPROM.h>
#include <Wire.h>
#include "RTC.h"
#include <Adafruit_PWMServoDriver.h>
#include <LiquidCrystal_I2C.h> // Library for LCD
#include <SD.h>
#include <ArduinoBLE.h>

unsigned long timer = 0;
unsigned long timerBat = 0;
int timeout = 10000;// 10 secons
int timeoutBat = 600000; //600000; //10 min
int instance;

const byte ROWS = 4; 
const byte COLS = 4; 

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

//i2c objects and keypad
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
File myFile;
BLEService lockerService("19B10001-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service
BLEByteCharacteristic LockerOpen("19B10001-E8F2-537E-4F6C-0000000000", BLERead | BLEWrite);
BLEIntCharacteristic LockerDrop("19B10001-E8F2-537E-4F6C-0000000100", BLERead | BLEWrite);
BLEIntCharacteristic Locker1("19B10001-E8F2-537E-4F6C-00000000001", BLERead | BLEWrite);
BLEIntCharacteristic Locker2("19B10001-E8F2-537E-4F6C-00000000002", BLERead | BLEWrite);
BLEIntCharacteristic Locker3("19B10001-E8F2-537E-4F6C-00000000003", BLERead | BLEWrite);
BLEIntCharacteristic Locker4("19B10001-E8F2-537E-4F6C-00000000004", BLERead | BLEWrite);
BLEIntCharacteristic Locker5("19B10001-E8F2-537E-4F6C-00000000005", BLERead | BLEWrite);
BLEIntCharacteristic Locker6("19B10001-E8F2-537E-4F6C-00000000006", BLERead | BLEWrite);
BLEIntCharacteristic Locker7("19B10001-E8F2-537E-4F6C-00000000007", BLERead | BLEWrite);
BLEIntCharacteristic Locker8("19B10001-E8F2-537E-4F6C-00000000008", BLERead | BLEWrite);
BLEIntCharacteristic Locker9("19B10001-E8F2-537E-4F6C-00000000009", BLERead | BLEWrite);
BLEIntCharacteristic Locker10("19B10001-E8F2-537E-4F6C-000000000010", BLERead | BLEWrite);
BLEIntCharacteristic Locker11("19B10001-E8F2-537E-4F6C-000000000011", BLERead | BLEWrite);
BLEIntCharacteristic Locker12("19B10001-E8F2-537E-4F6C-000000000012", BLERead | BLEWrite);
BLEIntCharacteristic Locker13("19B10001-E8F2-537E-4F6C-000000000013", BLERead | BLEWrite);
BLEIntCharacteristic Locker14("19B10001-E8F2-537E-4F6C-000000000014", BLERead | BLEWrite);
BLEIntCharacteristic Locker15("19B10001-E8F2-537E-4F6C-000000000015", BLERead | BLEWrite);
BLEIntCharacteristic Locker16("19B10001-E8F2-537E-4F6C-000000000016", BLERead | BLEWrite);
BLEIntCharacteristic lockersBLUE[totalLockers] = {Locker1,Locker2,Locker3,Locker4,Locker5,Locker6,Locker7,Locker8,Locker9,Locker10,Locker11,Locker12,Locker13,Locker14,Locker15,Locker16};

double mapd(long x, double in_min, double in_max, double out_min, double out_max) {
     double result;
     result = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
     return result;
}

String getTime(){

  String output = "";

  RTCTime currentTime;

  // Get current time from RTC
  RTC.getTime(currentTime);
  
  switch (DayOfWeek2int(currentTime.getDayOfWeek(),true))
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
  output+=" ";
  output+=(currentTime.getHour());
  // convert the byte variable to a decimal number when displayed
  output+=(":");
  if (currentTime.getMinutes()<10){
    output+=("0");
  }
  output+=(currentTime.getMinutes());
  output+=(":");
  if (currentTime.getSeconds()<10){
    output+=("0");
  }
  output+=(currentTime.getSeconds());
  output+=(" ");
  output+=(currentTime.getDayOfMonth());
  output+=("-");
  output+=(Month2int(currentTime.getMonth()));
  output+=("-");
  output+=(currentTime.getYear());

  return output;
  
}

bool storeLOG(String LOG_INPUT){
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    return false;
  }
  myFile = SD.open("LOG.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(LOG_INPUT);
    myFile.close();
  } else {
    return false;
  }
  storePRINTOUT();
  return true;
  
}

bool storeBAT(char input){
  double average = 0;
  for (int i=0; i < 10; i++) {
    average = average + analogRead(A1);
  }
  average = average/10;
  Serial.println(average);
  double batteryV = mapd(average,0,1024,0,5);

  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    return false;
  }
  myFile = SD.open("BATTERY.txt", FILE_WRITE);
  if (myFile) {
    if(input != '\0'){
      myFile.println("----NEW BATTERY----\n"+ String(input) + "Ah\n");
    }
    myFile.println(getTime()+", "+String(batteryV,2)+" Volt");
    myFile.close();
  } else {
    return false;
  }
  return true;
}

bool storePRINTOUT(){
  pinMode(10, OUTPUT);
  SD.remove("PRINTOUT.txt");
  if (!SD.begin(10)) {
    return false;
  }
  myFile = SD.open("PRINTOUT.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(printID());
    myFile.println("Last Use: " + getTime());
    myFile.close();
  } else {
    return false;
  }
  return true;
}



bool checkBatTime(){
  if((millis() - timerBat) > timeoutBat){ //if inactive for 10s, turn it off
    timerBat = millis();
    return true;
  }
  return false;
}


class Locker {
  public:
    byte state = byte(0); //1 for in use, 0 for empty
    byte type;  //1 for small, 2 for medium, 3 for large
    byte id;
    unsigned long purge = 0;
    int address;
    int student_id = 0;
    int offset = 0;


    void open(int type){ //type 1 for take out, type 0 for in
      pwm.setPin(id, SERVOOPEN+offset);//up
      if (state == 1 && type == 1){
        state = 0;
        //student_id = 0;
        purge = millis();
      }
      delay(200);
      if(type != 3){
        instance++;
      }
      EEPROM.update(1000, instance);
      storeData();
      fillService();
      close();
    }
    void close(){
      pwm.setPin(id, SERVOCLOSE+offset);//down
    }

    void storeData(){
      EEPROM.update(address, state);
      state = EEPROM.get(address,state);
      if (state == 255){
        state = 0;
      }
      EEPROM.put(address+1, student_id);
    }

    void refeshData(){
      state = EEPROM.get(address,state);
      student_id = EEPROM.get(address+1,student_id);
    }

    void showE(){
      Serial.println(EEPROM.get(address,state));
    }

    void purgeC(){
      if((millis() - purge > 60000L)&& student_id != 0 && state == 0){
        student_id = 0;
        storeData();
      }
    }

};
//Setting up lockers
Locker Lockers[totalLockers];
void fillService(){

  for(int i = 0; i < totalLockers; i++){
    if(i == 1){
      int average = 0;
      for (int i=0; i < 10; i++) {
        average = average + analogRead(A1);
      }
      average = average/10;
      int batteryPercent = map(average,832,1023,0,100);
      lockersBLUE[i].writeValue(batteryPercent);
    }else{
      lockersBLUE[i].writeValue(Lockers[i].student_id);
    }
  }

}

bool checkTime(){
  BLEDevice central = BLE.central();
  if(central.connected()){
    fillService();
    int toOpen=0;
    int toDrop=0;
    toOpen = LockerOpen.value(); //So it checks constantly
    toDrop = LockerDrop.value();
    Serial.println(toOpen);
    Serial.println(toDrop);
    if(toOpen != 0 && toOpen <= totalLockers){
      digitalWrite(A0, LOW);
      delay(500);
      Lockers[toOpen-1].open(0);
      //findLocker(String(toOpen));
      LockerOpen.writeValue(0);
      delay(500);
      digitalWrite(A0, HIGH);
    }

    if(toDrop != 0 && toDrop > 16){
      digitalWrite(A0, LOW);
      delay(500);
      String DropOffData = String(toDrop);
      int length = DropOffData.length();
      if (length > 0) {
        // Extract the last character as a string
        String lastChar = DropOffData.substring(length - 1);
        DropOffData[length-1] = '\0';
        DropOffData.trim();

        // Convert the last character string to an integer
        int lockerType = lastChar.toInt();
        Serial.println("Remote Drop ID: " + DropOffData +", Type" + String(lockerType));
        LockerDrop.writeValue(fillLocker(DropOffData,lockerType));
        delay(500);
        digitalWrite(A0, HIGH);
    }
  }}
  if((millis() - timer) > timeout){ //if inactive for 10s, turn it off
    timer = millis();
    return true;
  }
  return false;
}

//Functions that use Lockers
String printID(){
  String output = "";
  output+=(" Current Users: \n________________\n");
  output+=("|"+toFour(String(Lockers[0].student_id))+"|----|BAT |\n");
  output+=("|"+toFour(String(Lockers[2].student_id))+"|"+toFour(String(Lockers[3].student_id))+"|"+toFour(String(Lockers[4].student_id))+"|\n");
  output+=("|"+toFour(String(Lockers[5].student_id))+"|"+toFour(String(Lockers[6].student_id))+"|"+toFour(String(Lockers[7].student_id))+"|\n");
  output+=("|"+toFour(String(Lockers[8].student_id))+"|"+toFour(String(Lockers[9].student_id))+"|"+toFour(String(Lockers[10].student_id))+"|\n");
  output+=("|"+toFour(String(Lockers[11].student_id))+"|"+toFour(String(Lockers[12].student_id))+"|"+toFour(String(Lockers[13].student_id))+"|\n");
  output+=("| "+toFour(String(Lockers[14].student_id))+" | "+toFour(String(Lockers[15].student_id))+"  |\n");
  output+=("⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻\n");

  return output;
/*nice printout

   Current Users:  
  ________________
  |8888|----|BAT |
  |8888|8888|8888|
  |8888|8888|8888|
  |8888|8888|8888|
  |8888|8888|8888|
  | 8888 | 8888  |
  ⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻
*/
}

String toFour(String input){
  while(input.length() < 4){
    input = "0" + input;
  }
  return input;
}


void run(){

  bool done = false;
  int i = 1;
  lcd.clear();
  lcd.noBacklight();
  digitalWrite(A0, HIGH);
  delay(500);
  customKeypad.tick();
  char customKey = '\0';
  while(customKey == '\0'){
    customKeypad.tick();
    keypadEvent e = customKeypad.read();
    if(e.bit.EVENT == KEY_JUST_PRESSED) customKey = (char)e.bit.KEY;
    delay(100);
    for(int i = 0; i<totalLockers;i++){
      Lockers[i].purgeC();
    }
    if(checkBatTime()){
      storeBAT('\0');
    }
    if(checkTime()){
      return;
    }
    
  }
  timer = millis();
  digitalWrite(A0, LOW);
  lcd.backlight();
  lcd.setCursor(0, 0); //
  lcd.print("Pick up press 1");
  lcd.setCursor(0, 1); //
  lcd.print("Drop off press 2");
  delay(100);
  customKeypad.tick();
  customKey = '\0';
  while(customKey == '\0'){
    customKeypad.tick();
    keypadEvent e = customKeypad.read();
    if(e.bit.EVENT == KEY_JUST_PRESSED) customKey = (char)e.bit.KEY;
    delay(100);
    if(checkTime()){
      return;
    }
  }
  

  timer = millis();
  if (customKey == '1'){
    Serial.println("Pickup: Enter ID then press #");
    lcd.clear();
    lcd.setCursor(0, 0); //
    lcd.print("Pickup:Enter ID");
    lcd.setCursor(0, 1); //
    lcd.print("then press #");
    done = false;
    i = 1;
    String ID = "";
    while (!done){
      delay(100);
      char customKey = '\0';
      while(customKey == '\0'){
        customKeypad.tick();
        keypadEvent e = customKeypad.read();
        if(e.bit.EVENT == KEY_JUST_PRESSED) customKey = (char)e.bit.KEY;
        delay(100);
        if(checkTime()){
          return;
        }
      }
      timer = millis();
      if (customKey == '#' || customKey == '*'){
        done = true;
      }
      else if(customKey == 'C'){
        ID = "";
        Serial.println("-"+ID+"-");
        lcd.clear();
        lcd.setCursor(0, 0); //
        lcd.print("-"+ID+"-");
      }
      else if(customKey == 'B'){
        return;
      }
      else{
        ID = String(ID+customKey);
        Serial.println("-"+ID+"-");
        lcd.clear();
        lcd.setCursor(0, 0); //
        lcd.print("-"+ID+"-");
      }
    }
    byte status = findLocker(ID);
    if (status == byte(0)){
      Serial.println("Sorry no locker found");
      lcd.clear();
      lcd.setCursor(0, 0); //
      lcd.print("Sorry no locker");
      lcd.setCursor(0, 1); //
      lcd.print("found");
      delay(2000);
    }
    
  }
  else if(customKey == '2'){
    Serial.println("Drop off: Enter ID then press #");
    lcd.clear();
    lcd.setCursor(0, 0); //
    lcd.print("Dropoff:Enter ID");
    lcd.setCursor(0, 1); //
    lcd.print("then press #");
    done = false;
    i = 1;
    String ID = "";
     while (!done){
      delay(100);
      char customKey = '\0';
      while(customKey == '\0'){
        customKeypad.tick();
        keypadEvent e = customKeypad.read();
        if(e.bit.EVENT == KEY_JUST_PRESSED) customKey = (char)e.bit.KEY;
        delay(100);
        if(checkTime()){
          return;
        }
      }
      timer = millis();
      if (customKey == '#' || customKey == '*'){
        done = true;
      }
      else if(customKey == 'C'){
        ID = "";
        Serial.println("-"+ID+"-");
        lcd.clear();
        lcd.setCursor(0, 0); //
        lcd.print("-"+ID+"-");
      }
      else if(customKey == 'B' || customKey == 'A' || customKey == 'D'){ //bug fix
        return;
      }
      else{
        ID = String(ID+customKey);
        Serial.println("-"+ID+"-");
        lcd.clear();
        lcd.setCursor(0, 0); //
        lcd.print("-"+ID+"-");
      }
    }
    lcd.clear();
    lcd.setCursor(0, 0); //
    lcd.print("What Size");
    lcd.setCursor(0, 1); //
    lcd.print("S:1 M:2 L:3");
    done = false;
    char customKey1 = '\0';
    while(!done){
      customKey1 == '\0';
      while(customKey1 == '\0'){
        customKeypad.tick();
        keypadEvent e = customKeypad.read();
        if(e.bit.EVENT == KEY_JUST_PRESSED) customKey1 = (char)e.bit.KEY;
        Serial.println(customKey1);
        delay(100);
        if(checkTime()){
          return;
        }
        
      }
      timer = millis();
      if ( customKey1 == '1' || customKey1 == '2'|| customKey1 == '3'){
        done = true;
      }
      else{ 
        return;
      }
    }
    int type = customKey1 - '0';
    Serial.println("TYPE:"+ String(type));
    done = false;
    while(!done){
      byte status2 = fillLocker(ID, type);
      if (status2 != byte(0) && status2 != byte(20)){
        Serial.println("You got locker #"+String(status2)+"\nPlease close the door after inserting item\nHave a great day!");
        lcd.clear();
        lcd.setCursor(0, 0); //
        lcd.print("Use Locker #"+ String(status2));
        lcd.setCursor(0, 1); //
        lcd.print("Then close door");
        storeLOG(String(instance)+","+String(status2)+","+ID+","+"D"+","+getTime());
        done = true;
        delay(5000);
      }
      else if(status2 == byte(0) && type !=3){
        type++;
      }
      else if(status2 == byte(0) && type ==3){
        done = true;
        Serial.println("Sorry no locker available");
        lcd.clear();
        lcd.setCursor(0, 0); //
        lcd.print("Sorry no locker");
        lcd.setCursor(0, 1); //
        lcd.print("available");
        delay(2000);
      }
      else if(status2 == byte(20)){
        return;
      }
    }
    

  }
  else if (customKey == '3'){ //hidden menu
    Serial.println("Enter code");
    lcd.clear();
    lcd.setCursor(0, 0); //
    lcd.print("Enter code");
    done = false;
    i = 1;
    String ID = "";
    while (!done){
      delay(100);
      char customKey = '\0';
      while(customKey == '\0'){
        customKeypad.tick();
        keypadEvent e = customKeypad.read();
        if(e.bit.EVENT == KEY_JUST_PRESSED) customKey = (char)e.bit.KEY;
        delay(100);
        if(checkTime()){
          return;
        }
      }
      timer = millis();
      if (customKey == '#' || customKey == '*'){
        done = true;
      }
      else{
        ID = String(ID+customKey);
        Serial.println("-"+ID+"-");
        lcd.clear();
        lcd.setCursor(0, 0); //
        lcd.print("-"+ID+"-");
      }
    }
    if (ID != "1143"){
      Serial.println("Incorrect");
      lcd.clear();
      lcd.setCursor(0, 0); //
      lcd.print("Incorrect");
      done = true;
      delay(3000);
    }
    else{
      Serial.println("What locker");
      lcd.clear();
      lcd.setCursor(0, 0); //
      lcd.print("What locker");
      String ForceOpen = "";
      done = false;
      while(!done){
        delay(100);
        char customKey = '\0';
        while(customKey == '\0'){
          customKeypad.tick();
          keypadEvent e = customKeypad.read();
          if(e.bit.EVENT == KEY_JUST_PRESSED) customKey = (char)e.bit.KEY;
          delay(100);
          if(checkTime()){
            return;
          }
        }
        timer = millis();
        if (customKey == '#' || customKey == '*'){
          done = true;
          int a = ForceOpen.toInt()-1;
          Serial.println("Student #: "+ String(Lockers[a].student_id));
          lcd.clear();
          lcd.setCursor(0, 0); //
          lcd.print("Student #: ");
          lcd.setCursor(0, 1); //
          lcd.print(String(Lockers[a].student_id));
          Lockers[a].open(1);
          storeLOG(String(instance)+","+ForceOpen+","+String(Lockers[a].student_id)+","+"F"+","+getTime());
          delay(2000);
        }
        else if(customKey == 'A'){
          for(int i = 0; i < totalLockers; i++){
            done = true;
            Lockers[i].open(3);
            Lockers[i].showE();
          }
        }
        else if(customKey == 'C'){
          done = true;
        }
        else{
          ForceOpen = String(ForceOpen+customKey);
          Serial.println("-"+ForceOpen+"-");
          lcd.clear();
          lcd.setCursor(0, 0); //
          lcd.print("-"+ForceOpen+"-");
        }
      }
           
    }
    
  }else if( customKey == '4'){
    int average = 0;
    for (int i=0; i < 10; i++) {
      average = average + analogRead(A1);
    }
    average = average/10;
    int batteryPercent = map(average,832,1023,0,100); //OOAA
    lcd.clear();
    lcd.setCursor(0, 0); //
    if(batteryPercent > 0){
      lcd.print("Battery %: "+String(batteryPercent));
    }else{
      lcd.print("Battery %: N/A");
    }
    
    delay(3000);
  }else{
    return;
  }

}

void setup() {
  Serial.begin(115200);
  customKeypad.begin();
  RTC.begin();
  RTCTime startTime(2, Month::NOVEMBER, 2023, 17, 00, 00, DayOfWeek::THURSDAY, SaveLight::SAVING_TIME_ACTIVE);
  RTC.setTimeIfNotRunning(startTime);
  instance = EEPROM.get(1000,instance); //1000 should be middle of nowhere
  Serial.println(instance);


  //collecting battery data
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0); //
  lcd.print("What Size Bat");
  customKeypad.tick();
  char customKey = '\0';
  while(customKey == '\0'){
    customKeypad.tick();
    keypadEvent e = customKeypad.read();
    if(e.bit.EVENT == KEY_JUST_PRESSED) customKey = (char)e.bit.KEY;
    delay(10);

  }
  storeBAT(customKey);
  lcd.clear();
  lcd.setCursor(3, 0); //
  lcd.print("BT INIT.");

  //Bluetooth
  
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }
  BLE.setLocalName("MS_LOCKER");
  BLE.setAdvertisedService(lockerService);

  // add the characteristic to the service
  lockerService.addCharacteristic(LockerOpen);
  lockerService.addCharacteristic(LockerDrop);
  lockerService.addCharacteristic(Locker1);
  lockerService.addCharacteristic(Locker2);
  lockerService.addCharacteristic(Locker3);
  lockerService.addCharacteristic(Locker4);
  lockerService.addCharacteristic(Locker5);
  lockerService.addCharacteristic(Locker6);
  lockerService.addCharacteristic(Locker7);
  lockerService.addCharacteristic(Locker8);
  lockerService.addCharacteristic(Locker9);
  lockerService.addCharacteristic(Locker10);
  lockerService.addCharacteristic(Locker11);
  lockerService.addCharacteristic(Locker12);
  lockerService.addCharacteristic(Locker13);
  lockerService.addCharacteristic(Locker14);
  lockerService.addCharacteristic(Locker15);
  lockerService.addCharacteristic(Locker16);
  BLE.addService(lockerService);
  BLE.advertise();
  lcd.clear();
  lcd.setCursor(5, 0); //
  lcd.print("DONE");
  delay(500);

  pinMode(A0, OUTPUT); //For disabling the servos
  digitalWrite(A0, LOW);
    //OFFSETS
  Lockers[0].offset = (-10);
  Lockers[1].offset = (-20);
  Lockers[2].offset = (10);
  Lockers[5].offset = (-10);
  Lockers[11].offset =(10);
  Lockers[13].offset = (20);
  Lockers[14].offset = (-10);

  pwm.begin();
  pwm.setOscillatorFrequency(26522000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  //pwm.sleep();

  for(int i =0; i<totalLockers;i++){
    pwm.setPin(i, SERVOCLOSE+Lockers[i].offset);
  }
  
  delay(500);
  
  digitalWrite(A0, HIGH);
  for (int i = 0; i < totalLockers; i++){
    Serial.println("Locker["+String(i)+"].offset = "+String(Lockers[i].offset));
    Lockers[i].address = (i+1)*10;
    Lockers[i].refeshData();
    Lockers[i].id = i;
    if (i < smallLockers){
      Lockers[i].type = byte(1);
    }
    else if (i >= smallLockers && i < smallLockers+mediumLockers)
    {
      Lockers[i].type = byte(2);    
    }
    else if (i >= mediumLockers && i < smallLockers+mediumLockers+largeLockers)
    {
      Lockers[i].type = byte(3);    
    }
    Lockers[1].type = byte(4);
  }
  
  fillService();
  Serial.println(printID());
  Serial.println("Current Time: "+ getTime());
  
  
}



byte findLocker(String ID){ //for pickup
byte found = byte(0);
  for (int i = 0; i < totalLockers; i++){
    if(Lockers[i].student_id == ID.toInt()){
      found = byte(1);
      Lockers[i].open(1);
      Serial.println("Check Locker #"+ String(Lockers[i].id +1)+"\nPlease close the door when you get your print\nHave a great day!");
      lcd.clear();
      lcd.setCursor(0, 0); //
      lcd.print("Check Locker #"+ String(Lockers[i].id +1));
      lcd.setCursor(0, 1); //
      lcd.print("Then close door");
      storeLOG(String(instance)+","+String(Lockers[i].id +1)+","+ID+","+"P"+","+getTime());
      delay(3000);
    }

  }
  return found;
}

byte fillLocker(String ID, int type){ //for drop off
  byte typeIn = byte(0);
  bool reuse = false;
  
  switch(type){
    case 1: typeIn = byte(1); break;
    case 2: typeIn = byte(2); break;
    case 3: typeIn = byte(3); break;
    default: typeIn = byte(2); break;
  }

  for (int i = 0; i < totalLockers; i++){
    if(Lockers[i].type >= typeIn && Lockers[i].student_id == ID.toInt()){
      Serial.println("Reuse? (1 yes 2 no)");
      lcd.clear();
      lcd.setCursor(0, 0); //
      lcd.print("Reuse Locker?");
      lcd.setCursor(0, 1);
      lcd.print("1:YES || 2:NO");
      char customKey = '\0';
      while(customKey == '\0'){
        customKeypad.tick();
        keypadEvent e = customKeypad.read();
        if(e.bit.EVENT == KEY_JUST_PRESSED) customKey = (char)e.bit.KEY;
        delay(100);
        if(checkTime()){
          return byte(20);// noresponse case
        }
      }
      if(customKey == '1'){
        reuse = true;
      }

    }
  }
  for (int i = 0; i < totalLockers; i++){ //1 (1,3,4,5) //2 (6,7,8,9,10,11,12,13,14) //3 (15,16)
    //Lockers[i].refeshData();
    if(Lockers[i].type >= typeIn){
      //reuse
      if(Lockers[i].student_id == ID.toInt() && reuse){
        Lockers[i].student_id = ID.toInt();
        Lockers[i].state = byte(1);
        Lockers[i].open(0);
        return Lockers[i].id +1;
      }
      else if(Lockers[i].state == 0 && !reuse){
        Lockers[i].student_id = ID.toInt();
        Lockers[i].state = byte(1);
        Lockers[i].open(0);
        return Lockers[i].id +1;
      }
    }
    /*
    if(typeIn != byte(3)){ //Might delete, but checks next size up
      int temp = typeIn;
      return fillLocker(ID, temp+1);
    }
    */
    
  }
  return byte(0);
}

void loop() {
  run();
}
