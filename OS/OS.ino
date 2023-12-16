#define totalLockers 16
#define smallLockers 5
#define mediumLockers 9
#define largeLockers 2
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define SERVOOPEN 480 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOCLOSE  420 // This is the 'maximum' pulse length count (out of 4096)
#define Battery A1
#include <Keypad.h>
#include <math.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <LiquidCrystal_I2C.h> // Library for LCD

unsigned long timer = 0;
int timeout = 10000;// 10 secons


const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

//i2c objects and keypad
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();


bool checkTime(){
  if((millis() - timer) > timeout){ //if inactive for 10s, turn it off
    timer = millis();
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
      storeData();
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
      }
    }

};
//Setting up lockers
Locker Lockers[totalLockers];

//Functions that use Lockers
void printID(){
  Serial.println(" Current Users: \n________________");
  Serial.println("|"+toFour(String(Lockers[0].student_id))+"|----|BAT |");
  Serial.println("|"+toFour(String(Lockers[2].student_id))+"|"+toFour(String(Lockers[3].student_id))+"|"+toFour(String(Lockers[4].student_id))+"|");
  Serial.println("|"+toFour(String(Lockers[5].student_id))+"|"+toFour(String(Lockers[6].student_id))+"|"+toFour(String(Lockers[7].student_id))+"|");
  Serial.println("|"+toFour(String(Lockers[8].student_id))+"|"+toFour(String(Lockers[9].student_id))+"|"+toFour(String(Lockers[10].student_id))+"|");
  Serial.println("|"+toFour(String(Lockers[11].student_id))+"|"+toFour(String(Lockers[12].student_id))+"|"+toFour(String(Lockers[13].student_id))+"|");
  Serial.println("| "+toFour(String(Lockers[14].student_id))+" | "+toFour(String(Lockers[15].student_id))+"  |");
  Serial.println("⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻⎻");

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
  digitalWrite(12, HIGH);
  delay(100);
  while(customKeypad.getKey() == '\0'){
    delay(100);
    
    if(checkTime()){
      return;
    }
    for(int i = 0; i<totalLockers;i++){
      Lockers[i].purgeC();
    }
    
  }
  timer = millis();
  digitalWrite(12, LOW);
  lcd.backlight();
  lcd.setCursor(0, 0); //
  lcd.print("Pick up press 1");
  lcd.setCursor(0, 1); //
  lcd.print("Drop off press 2");
  delay(100);
  char customKey = customKeypad.getKey();
  while(customKey == '\0'){
    customKey = customKeypad.getKey();
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
      char customKey = customKeypad.getKey();
      while(customKey == '\0'){
        customKey = customKeypad.getKey();
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
      char customKey = customKeypad.getKey();
      while(customKey == '\0'){
        customKey = customKeypad.getKey();
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
    lcd.clear();
    lcd.setCursor(0, 0); //
    lcd.print("What Size");
    lcd.setCursor(0, 1); //
    lcd.print("S:1 M:2 L:3");
    done = false;
    char customKey1;
    while(!done){
      delay(100);
      customKey1 = customKeypad.getKey();
      while(customKey1 == '\0'){
        customKey1 = customKeypad.getKey();
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
    }
    int type = customKey1 - '0';
    Serial.println("TYPE:"+ String(type));
    byte status2 = fillLocker(ID, type);

    if (status2 != byte(0) && status2 != byte(20)){
      Serial.println("You got locker #"+String(status2)+"\nPlease close the door after inserting item\nHave a great day!");
      lcd.clear();
      lcd.setCursor(0, 0); //
      lcd.print("Use Locker #"+ String(status2));
      lcd.setCursor(0, 1); //
      lcd.print("Then close door");
      delay(5000);
    }
    else if(status2 == byte(0)){
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
      char customKey = customKeypad.getKey();
      while(customKey == '\0'){
        customKey = customKeypad.getKey();
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
        char customKey = customKeypad.getKey();
        while(customKey == '\0'){
          customKey = customKeypad.getKey();
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
          delay(2000);
        }
        else if(customKey == 'A'){
          for(int i = 0; i < totalLockers; i++){
            done = true;
            Lockers[i].open(1);
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
    int batteryPercent = map(average,832,1024,0,100);
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
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
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
  lcd.init();
  delay(500);
  Serial.begin(115200);
  digitalWrite(12, HIGH);
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
  printID();
  

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
    if(Lockers[i].type == typeIn && Lockers[i].student_id == ID.toInt()){
      Serial.println("Reuse? (1 yes 2 no)");
      lcd.clear();
      lcd.setCursor(0, 0); //
      lcd.print("Reuse Locker?");
      lcd.setCursor(0, 1);
      lcd.print("1:YES || 2:NO");
      char customKey = customKeypad.getKey();
      while(customKey == '\0'){
        customKey = customKeypad.getKey();
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
    if(Lockers[i].type == typeIn){
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
