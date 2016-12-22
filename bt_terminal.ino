#include <SoftwareSerial.h>
#include <Servo.h>

String chars[] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
                  "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"
                 };

String alphabet[] = {
  "  .-",
  "-...",
  "-.-.",
  " -..",
  "   .",
  "..-.",
  " --.",
  "....",
  "  ..",
  ".---",
  " -.-",
  ".-..",
  "  --",
  "  -.",
  " ---",
  ".--.",
  "--.-",
  " .-.",
  " ...",
  "   -",
  " ..-",
  "...-",
  " .--",
  "-..-",
  "-.--",
  "--.."
};

typedef void (*func) (String cmd);

SoftwareSerial bt(9, 10); // (tx,rx)

// key value struct
struct key_func {
  String key;
  func fn;
};

// function declerations
int getIntParam(String, String, int);
boolean isExistParam(String, String);
void blink(String);
void servo(String);
void laser(String);
void listenBluetooth();
void listenSerial();

// commands array
key_func func_map[3];

void setup() {
  bt.begin(9600);
  bt.println("AT+NAMEGox");
  Serial.begin(19200);

  // key and function name must be same
  func_map[0].key = "blink"; // command name
  func_map[0].fn = blink;    // command function

  func_map[1].key = "servo";
  func_map[1].fn = servo;

  func_map[2].key = "laser";
  func_map[2].fn = laser;

}

String bluetoothData = "";
String serialData = "";

void loop() {
  // blink -t 20 -i 100 -a
  listenBluetooth();
  // blink -t 20 -i 100 -a
  listenSerial();
}


void listenBluetooth() {
  while (bt.available() > 0) {
    char chr = bt.read();
    if (chr != '\n') bluetoothData.concat(chr);
    if (chr == '\n') {
      for (int i = 0; i < sizeof func_map / sizeof func_map[0]; i++) {
        if (bluetoothData.startsWith(func_map[i].key)) {
          func_map[i].fn(bluetoothData);
        }
      }
      bluetoothData = "";
    }
  }
}

void listenSerial() {
  while (Serial.available() > 0) {
    char chr = Serial.read();
    if (chr != '\n') serialData.concat(chr);
    if (chr == '\n') {
      for (int i = 0; i < sizeof func_map / sizeof func_map[0]; i++) {
        if (serialData.startsWith(func_map[i].key)) {
          func_map[i].fn(serialData);
        }
      }
      serialData = "";
    }
  }
}

void servo(String cmd) {  
  String paramsStr = cmd.substring(sizeof __func__ / sizeof __func__[0]);

  Serial.println("servo");

  int pin = getIntParam("-p", paramsStr, 3);
  int degree = getIntParam("-d", paramsStr, 0);

  Servo srv;
  srv.attach(pin);
  srv.write(map(degree, 0, 1023, 0, 180));  

}

void blink(String cmd) {
  String paramsStr = cmd.substring(sizeof __func__ / sizeof __func__[0]);

  int pin = getIntParam("-p", paramsStr, 10);
  int times = getIntParam("-t", paramsStr, 10);
  int interval = getIntParam("-i", paramsStr, 250);

  pinMode(pin, OUTPUT);

  for (int i = 0; i < times ; i++) {
    digitalWrite(pin, HIGH);
    delay(interval);
    digitalWrite(pin, LOW);
    delay(interval);
  }

  if (isExistParam("-a", paramsStr)) {
    digitalWrite(pin, HIGH);
  } else {
    digitalWrite(pin, LOW);
  }

}



void laser(String cmd) {
  String paramsStr = cmd.substring(sizeof __func__ / sizeof __func__[0]);

  int pin = getIntParam("-p", paramsStr, 12);
  String msg = getStringParam("-m", paramsStr, " ");

  Serial.println(msg);

  pinMode(pin, OUTPUT);
  // delays are optional
  digitalWrite(pin, HIGH);
  delay(1000);
  digitalWrite(pin, LOW);
  delay(50);

  for (int i = 0; i < msg.length(); i++ ) {
    for (int j = 0; j < 4; j++) {
      Serial.println(alphabet[findCharIndex(msg.charAt(i))].charAt(j));
      if (alphabet[findCharIndex(msg.charAt(i))].charAt(j) == ' ') {
        digitalWrite(pin, HIGH);
        delay(100);
        digitalWrite(pin, LOW);
        delay(50);
      } else if (alphabet[findCharIndex(msg.charAt(i))].charAt(j) == '.') {
        digitalWrite(pin, HIGH);
        delay(300);
        digitalWrite(pin, LOW);
        delay(50);
      } else if (alphabet[findCharIndex(msg.charAt(i))].charAt(j) == '-') {
        digitalWrite(pin, HIGH);
        delay(500);
        digitalWrite(pin, LOW);
        delay(50);
      }
    }
  }
}

int findCharIndex(char chr){
  for(int i=0; i<(sizeof(chars) / sizeof(String)); i++ ){
    if(chars[i].startsWith(String(chr))){
      return i;
    }
  }
  return 0;
} 

int getIntParam(String param_name, String params, int default_value) {
  int position = params.indexOf(param_name);

  if (position > -1) {
    return params.substring(position + param_name.length() + 1, params.indexOf(' ', position + param_name.length() + 1)).toInt();
  }
  return default_value;
}

String getStringParam(String param_name, String params, String default_value) {
  int position = params.indexOf(param_name);

  if (position > -1) {
    return params.substring(position + param_name.length() + 1, params.indexOf(' ', position + param_name.length() + 1));
  }
  return default_value;
}

boolean isExistParam(String param_name, String params) {
  return params.indexOf(param_name) > -1;
}
