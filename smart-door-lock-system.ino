//Pemanggilan Library
const byte sda = 10;
const byte rst = -1;

#include <KRrfid.h>
#include <Keypad.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;
const int pinSensorLDR = A0;          //Pin input untuk sensor LDR
int hasil;                                 //Membuat variable untuk hasil sensor

char password[4];
char initial_password[4],new_password[4];

const int buzzer = 8;

int i=0;
bool door_condition = false; // close = false, true = open
bool lock_condition = true; // true = lock, false = unlock
bool break_status = false;
int cahaya;
const int ldr_thres = 30;
char key_pressed=0;

int idle = 0;

const byte rows = 4; 
const byte columns = 4; 

char hexaKeys[rows][columns] = {
{'D','C','B','A'},
{'#','9','6','3'},
{'0','8','5','2'},
{'*','7','4','1'}
};

byte row_pins[rows] = {7,6,5,4};
byte column_pins[columns] = {3,2,1,0};   

Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

void setup(){
  rfidBegin();
  myservo.attach(9);
  myservo.write(5);
  lcd.init();
  lcd.backlight();
  initialpassword();
}

bool break_condition() {
  break_status = true;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("OMO OMO OMO!");
  lcd.setCursor(0,1);
  lcd.print("ALERT!");
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(100);  
}

void idle_status() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Masukan Input");
  idle = 1;
}

void loop(){

  if(idle == 0) {
    idle_status();
  }

  if(lock_condition == true && door_condition == false) {
    if(cahaya > ldr_thres || break_status == true) {
      break_condition();
    }
  }
  
  cahaya = analogRead(pinSensorLDR);
  cahaya = map(cahaya, 10, 800, 0, 100);

  key_pressed = keypad_key.getKey();
  getTAG();
  
  if(key_pressed=='#') change();

  if(key_pressed =='D') {
    idle = 0;
    lcd.clear();
    break_status = false;
  }
  
  if (key_pressed)
  {
    password[i++]=key_pressed;
    lcd.setCursor(i-1,1);
    lcd.print(key_pressed);
  }
  
  if(i==4 || TAG == "36369824881128")
  {
    delay(100);
    for(int j=0;j<4;j++) initial_password[j]=EEPROM.read(j);
    
    if(!(strncmp(password, initial_password,4)) || TAG == "36369824881128")
    {
      lcd.clear();
      lcd.print("ANNYEONGHASEO!");
      delay(500);
      lcd.setCursor(0,1);
      lcd.print("TWICESUMIDA");
      myservo.write(0);
      lock_condition = false;

      delay(500);

      if(cahaya > ldr_thres) {
        door_condition = true;
      }
      else if (cahaya < ldr_thres && door_condition == true) {
        lock_condition = true;
        door_condition = false;
        myservo.write(5);
        TAG = "";
        i = 0;
        idle = 0;
      }
      
    }
    else
    {
      lcd.clear();
      lcd.print("ONCE");
      lcd.setCursor(0,1);
      lcd.print("Pabo-Ya!!");
      delay(2000);

      myservo.write(5);
      i=0;
      idle = 0;
    }
  }
}

void change(){
  int j=0;
  lcd.clear();
  lcd.print("Current Password");
  lcd.setCursor(0,1);
  while(j<4)
  {
    char key=keypad_key.getKey();
    if(key)
    {
      new_password[j++]=key;
      lcd.print(key);
    }
    key=0;
  }
  delay(500);

  if((strncmp(new_password, initial_password, 4)))
  {
    lcd.clear();
    lcd.print("ONCE");
    lcd.setCursor(0,1);
    lcd.print("Pabo-Ya!");
    delay(2000);
  }
  else
  {
    j=0;
    lcd.clear();
    lcd.print("New Password:");
    lcd.setCursor(0,1);
    while(j<4)
    {
      char key=keypad_key.getKey();
      if(key)
      {
        initial_password[j]=key;
        lcd.print(key);
        EEPROM.write(j,key);
        j++;
      }
    }
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Pass Changed");
    delay(500);
  }
  
  key_pressed="";
  idle = 0;
  return;

}

void initialpassword(){
  for(int j=0;j<4;j++) EEPROM.write(j, j+49);

  for(int j=0;j<4;j++) initial_password[j]=EEPROM.read(j);
} 
