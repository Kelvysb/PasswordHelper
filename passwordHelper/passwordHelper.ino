#include <SPI.h>
#include <SD.h>
#include <Keyboard.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

const int DISP_OP = A0;
const int DISP_A = 5;
const int DISP_B = 6;
const int DISP_C = A1;
const int DISP_D = A2;
const int DISP_E = A3;
const int DISP_F = 3;
const int DISP_G = 2;
const int DISP_OFF = 10;
const int DISP_LOCK = 11;
const int DISP_TIMEOUT = 120;

const int COM_NONE = 0;
const int COM_CLICK = 1;
const int COM_UP = 2;
const int COM_DOWN = 3;
const int COM_DELAY = 200;

int16_t last, value;
ClickEncoder *encoder;

//Test lock ID replace 
const String LOCK_ID = "5236658DKS";
String lockString = "";
String pin = "";
int pinValue = 0;
bool lock = true;
int current = DISP_LOCK;

int timeout = 0;

void setup() {  
  SD.begin(4);

  setPinMode(DISP_OP, OUTPUT);
  setPinMode(DISP_A, OUTPUT);
  setPinMode(DISP_B, OUTPUT);
  setPinMode(DISP_C, OUTPUT);
  setPinMode(DISP_D, OUTPUT);
  setPinMode(DISP_E, OUTPUT);
  setPinMode(DISP_F, OUTPUT);
  setPinMode(DISP_G, OUTPUT);
  
  encoder = new ClickEncoder(8, 9, 7);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 
  last = -1; 
  value = -1; 
  setDisplay(DISP_LOCK, true);

  timeout = DISP_TIMEOUT;
}

void loop() {
  int input = 0;
  input = checkInput();
  if(input != COM_NONE && timeout <= 0) {
    timeout = DISP_TIMEOUT;
    input = COM_NONE;
    setDisplay(current, false);
  }
  if(input != COM_NONE) {
    timeout = DISP_TIMEOUT;
    if(input == COM_UP){
      current++;
    }
    if(input == COM_DOWN){
      current--;
    }
    if(current > 9){
      current = 0;
    }
    if(current < 0){
      current = 9;
    }    
    if(!lock){
      setDisplay(current, false);
      if(input == COM_CLICK){
        execute(current);
      }
    }else{      
      if(input == COM_CLICK){
        pin += String(current);
        current = 0;          
      }
      if(pin.length() == 4){
        unlock();
      }else{
        setDisplay(current, true); 
      }      
    }
  } else{
    timeout--;
  }
  if (timeout <= 0){
    setDisplay(DISP_OFF, false);
  }
  delay(500);
}

void execute(int index){
  String file = "C";
  file.concat(String(index+1));
  file.concat(".TXT");
  String value = readFile(file);
  value = Decripto(pinValue, value);
  sendKeyboard(value);  
}

void unlock() {
  pinValue = pin.toInt();
  String id = readFile("LOCK.TXT");
  id = Decripto(pinValue, id); 
  if(id == LOCK_ID){
    lock = false;    
    current = 0;
    setDisplay(0, false);    
  }else{
    pin = "";
    setDisplay(DISP_LOCK, true);
  }
}

void sendKeyboard(String value) {
  Keyboard.begin();
  Keyboard.print(value);
  Keyboard.write(KEY_RETURN);
  Keyboard.end();
}

String readFile(String filename) {  
  File file = SD.open(filename);
  String result = "";
  while (file.available()) {
    result.concat((char)file.read());
  }
  file.close();
  return result;
}

int checkInput() {
  int result = COM_NONE;
  ClickEncoder::Button b = encoder->getButton();
  if(b == ClickEncoder::Clicked || b == ClickEncoder::DoubleClicked)
  {
    result = COM_CLICK;    
  }else{
    value += encoder->getValue();
    if (value != last) {
      if(value > last){
        result = COM_UP;
      }else{
        result = COM_DOWN;
      }
      last = value;
    }  
  }  
  return result;
}

String Decripto(int key, String input)
{
    String result = "";
    for (int i = 0; i < input.length(); i++)
    {        
      if(input[i] != 10 && input[i] != 13){
        int spin = (key + i) % 95;
        int newValue = (input[i] - 32) - spin;
        newValue = (newValue < 0 ? newValue + 95 : newValue) + 32;        
        result = result + (char)newValue;        
      }              
    }
    return result;
}

void setDisplay(int value, bool op) {
  
  digitalWrite(DISP_OP, LOW);
  digitalWrite(DISP_A, LOW);
  digitalWrite(DISP_B, LOW);
  digitalWrite(DISP_C, LOW);
  digitalWrite(DISP_D, LOW);
  digitalWrite(DISP_E, LOW);
  digitalWrite(DISP_F, LOW);
  digitalWrite(DISP_G, LOW);
  
  if(op){
    digitalWrite(DISP_OP, HIGH);
  }
    
  switch(value){
    case 0 : 
      digitalWrite(DISP_A, HIGH);
      digitalWrite(DISP_B, HIGH);
      digitalWrite(DISP_C, HIGH);
      digitalWrite(DISP_D, HIGH);
      digitalWrite(DISP_E, HIGH);
      digitalWrite(DISP_F, HIGH);    
      break;
    case 1 : 
      digitalWrite(DISP_B, HIGH);
      digitalWrite(DISP_C, HIGH);      
      break;
    case 2 : 
      digitalWrite(DISP_A, HIGH);
      digitalWrite(DISP_B, HIGH);
      digitalWrite(DISP_D, HIGH);
      digitalWrite(DISP_E, HIGH);    
      digitalWrite(DISP_G, HIGH);
      break;
    case 3 : 
      digitalWrite(DISP_A, HIGH);
      digitalWrite(DISP_B, HIGH);
      digitalWrite(DISP_C, HIGH);
      digitalWrite(DISP_D, HIGH);          
      digitalWrite(DISP_G, HIGH);
      break;
    case 4 : 
      digitalWrite(DISP_B, HIGH);
      digitalWrite(DISP_C, HIGH);      
      digitalWrite(DISP_F, HIGH);
      digitalWrite(DISP_G, HIGH);
      break;
    case 5 : 
      digitalWrite(DISP_A, HIGH);      
      digitalWrite(DISP_C, HIGH);
      digitalWrite(DISP_D, HIGH);
      digitalWrite(DISP_F, HIGH);
      digitalWrite(DISP_G, HIGH);
      break;
    case 6 : 
      digitalWrite(DISP_A, HIGH);
      digitalWrite(DISP_C, HIGH);
      digitalWrite(DISP_D, HIGH);
      digitalWrite(DISP_E, HIGH);
      digitalWrite(DISP_F, HIGH);
      digitalWrite(DISP_G, HIGH);
      break;
    case 7 : 
      digitalWrite(DISP_A, HIGH);
      digitalWrite(DISP_B, HIGH);
      digitalWrite(DISP_C, HIGH);      
      break;
    case 8 : 
      digitalWrite(DISP_A, HIGH);
      digitalWrite(DISP_B, HIGH);
      digitalWrite(DISP_C, HIGH);
      digitalWrite(DISP_D, HIGH);
      digitalWrite(DISP_E, HIGH);
      digitalWrite(DISP_F, HIGH);
      digitalWrite(DISP_G, HIGH);
      break;
    case 9 : 
      digitalWrite(DISP_A, HIGH);
      digitalWrite(DISP_B, HIGH);
      digitalWrite(DISP_C, HIGH);
      digitalWrite(DISP_D, HIGH);
      digitalWrite(DISP_F, HIGH);
      digitalWrite(DISP_G, HIGH);
      break;
    case DISP_LOCK : 
      digitalWrite(DISP_D, HIGH);
      digitalWrite(DISP_E, HIGH);
      digitalWrite(DISP_F, HIGH);
      break;
    case DISP_OFF : 
      break;
  }
}

void timerIsr() {
  encoder->service();
}
