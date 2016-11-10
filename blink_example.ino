#include <SoftwareSerial.h>

typedef void (*func) (String cmd);
extern func functions[];

SoftwareSerial bt(9, 10); // (tx,rx)
const int LED1 = 8; // led pin

// key value struct
struct key_func {
  String key;
  func fn;
};

// function declerations
int getIntParam(String, String, int);
boolean isExistParam(String, String);
void blink(String);
void listenBluetooth();
void listenSerial();

// commands array
key_func func_map[1];

void setup() {
  bt.begin(9600);
  Serial.begin(19200);
  pinMode(LED1, OUTPUT);

  // key and function name must be same
  func_map[0].key = "blink"; // command name
  func_map[0].fn = blink;    // command function

}

String bluetoothData = "";
String serialData = "";

void loop() {
  // blink -t 20 -i 100 -a 
  listenBluetooth();
  // blink -t 20 -i 100 -a
  listenSerial();
}


void listenBluetooth(){
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

void listenSerial(){
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


void blink(String cmd) {
  String paramsStr = cmd.substring(sizeof __func__ / sizeof __func__[0]);

  int times = getIntParam("-t", paramsStr, 10);
  int interval = getIntParam("-i", paramsStr, 250);

  for (int i = 0; i < times ; i++) {
    digitalWrite(LED1, HIGH);
    delay(interval);
    digitalWrite(LED1, LOW);
    delay(interval);
  }

  if (isExistParam("-a", paramsStr)) {
    digitalWrite(LED1, HIGH);
  } else {
    digitalWrite(LED1, LOW);
  }

}

int getIntParam(String param_name, String params, int default_value) {
  int position = params.indexOf(param_name);

  if (position > -1) {
    return params.substring(position + param_name.length() + 1, params.indexOf(' ', position + param_name.length() + 1)).toInt();
  }
  return default_value;
}

boolean isExistParam(String param_name, String params) {
  return params.indexOf(param_name) > -1;
}
