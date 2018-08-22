#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#define TRIGGER1 14
#define TRIGGER2 15
#define TRIGGER3 16
#define TRIGGER4 17
#define TRIGGER5 18
#define TRIGGER6 19

#define EXT_CLK 21
#define INT_CLK A13

#define CHANNEL_BUTTON A8
#define ROTATE_BUTTON 23
#define STEP_BUTTON 7

#define encoder0PinA  2
#define encoder0PinB  3
#define debounceTime 500

#define NEOPIN 13

#define PAUSE_BUTT 24
#define PAUSE_LED 25

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(16, NEOPIN); //Initialize pixels
int YELLOW[3] {255, 200, 0};
int GREEN[3] = {10, 255, 10};
int TEAL[3] = {0, 255, 255};
int ORANGE[3] = {255, 64, 0};
int _PINK[3] = {255, 27, 27};
int PURPLE[3] = {255, 27, 255};

#define encoder0PinA  2
#define encoder0PinB  3
#define debounceTime 500

unsigned long int interruptTime = millis();
volatile unsigned int encoder0Pos = 0;

int stepsPulses [6][2] = {
  {16, 0},
  {16, 0},
  {16, 0},
  {16, 0},
  {16, 0},
  {16, 0}
};

int channel = 0;
int remainder[16];
int count[16];
int level;
int state = 0;
int stateArray[6][16];
int beat[6] = {0, 0, 0, 0, 0, 0};

float triggerOnTime;
bool trigger = false;
unsigned long int hit;
bool pause = false;
int clkTime = 500;
unsigned long int clkChange = millis();
unsigned long int pauseBounce = millis();
void setup() {
  Serial.begin(9600);
  Serial.println("Running..");

  pinMode(EXT_CLK, INPUT_PULLUP);

  pinMode(TRIGGER1, OUTPUT);
  pinMode(TRIGGER2, OUTPUT);
  pinMode(TRIGGER3, OUTPUT);
  pinMode(TRIGGER4, OUTPUT);
  pinMode(TRIGGER5, OUTPUT);
  pinMode(TRIGGER6, OUTPUT);
  pixels.begin();
  pixels.setPixelColor(0, 255, 255, 255);
  pixels.show();
  delay(1000);
  pixels.setPixelColor(0, 0, 0, 0);
  pixels.show();
  pinMode(CHANNEL_BUTTON, INPUT_PULLUP);
  pinMode(ROTATE_BUTTON, INPUT_PULLUP);
  pinMode(PAUSE_BUTT, INPUT_PULLUP);
  pinMode(PAUSE_LED, OUTPUT);
  // digitalWrite(PAUSE_LED, HIGH);
  attachInterrupt(digitalPinToInterrupt(EXT_CLK), triggerOn, CHANGE);
  pinMode(encoder0PinA, INPUT_PULLUP);
  pinMode(encoder0PinB, INPUT_PULLUP);
  attachInterrupt(0, doEncoderA, CHANGE);
  attachInterrupt(1, doEncoderB, CHANGE);
  int i = 0;
  for(int k = 0; k < 6; k++){
    for (int n = 0; n < 2; n++){
      stepsPulses[k][n] = EEPROM.read(i);
      i++;
    }
  }
  calculateAll();
  color(255, 150, 0);
}

void loop() {
  Serial.println(digitalRead(ROTATE_BUTTON));
  clkTime = 1024 - analogRead(INT_CLK);
  if (millis() - clkChange > clkTime) {
    triggerOn();
    clkChange = millis();
  }
  if (digitalRead(CHANNEL_BUTTON) == LOW && millis() - hit > 250) {
    Serial.println("Channel Change");
    changeChannel();
  }
  Serial.println(digitalRead(ROTATE_BUTTON));
  displayPixels();

  if ((millis() - triggerOnTime) >= 20 && trigger) {
    triggerOff();
  }
  if (digitalRead(PAUSE_BUTT) == LOW && millis() - pauseBounce > 300) {
    pauseFunc();
    pauseBounce = millis();
  }
  if (digitalRead(CHANNEL_BUTTON) == LOW && digitalRead(STEP_BUTTON) == HIGH) {
    writeArray();
  }
  if(digitalRead(ROTATE_BUTTON) == LOW && digitalRead(STEP_BUTTON) == HIGH){
    stateArray[6][16];
    int tempChannel = channel;
      for (int i = 0; i < 6; i++){
    remainder[16];
  count[16];
  state = 0;
  compute_bitmap(stepsPulses[channel][0], stepsPulses[channel][1], channel);
  channel++;
  }
  channel = tempChannel;
  }

}
void compute_bitmap(int num_slots, int num_pulses, int channel)  {
  int steps, pulses;

  if (num_pulses > num_slots) {
    num_pulses = num_slots;
  }
  int divisor = num_slots - num_pulses;
  steps = num_slots; pulses = num_pulses;
  remainder[0] = num_pulses;
  level = 0;
  do {
    count[level] = divisor / remainder[level];
    remainder[level + 1] = divisor % remainder[level];
    divisor = remainder[level];
    level = level + 1;
  }
  while (remainder[level] > 1);

  count[level] = divisor;
  build_string (level, channel);

}

//Bjorklund accessory function to build the output..
void build_string (int level, int channel)  {
  if (level == -1) {

    //Serial.println('0'); // Debug
    stateArray[channel][state] = 0; //insert 0 into array
    state = state + 1;  //move to next
  }
  else if (level == -2)  {

    //Serial.println('1');  // Debug
    stateArray[channel][state] = 1; //insert 1 into array
    state = state + 1;  //move to next
  }
  else {
    for (int i = 0; i < count[level]; i++)
      build_string(level - 1, channel);
    if (remainder[level] != 0)
      build_string(level - 2, channel);
  }

}

void triggerOn() {
  if (pause) {
    return;
  }
  trigger = true;

  if (stateArray[0][beat[0]] == 1) {
    digitalWrite(TRIGGER1, HIGH);
  }
  beat[0]++;
  if (beat[0] >= stepsPulses[0][0]) {
    beat[0] = 0;
  }

  if (stateArray[1][beat[1]] == 1) {
    digitalWrite(TRIGGER2, HIGH);
  }
  beat[1]++;
  if (beat[1] >= stepsPulses[1][0]) {
    beat[1] = 0;
  }

  if (stateArray[2][beat[2]] == 1) {
    digitalWrite(TRIGGER3, HIGH);
  }
  beat[2]++;
  if (beat[2] >= stepsPulses[2][0]) {
    beat[2] = 0;
  }

  if (stateArray[3][beat[3]] == 1) {
    digitalWrite(TRIGGER4, HIGH);
  }
  beat[3]++;
  if (beat[3] >= stepsPulses[3][0]) {
    beat[3] = 0;
  }

  if (stateArray[4][beat[4]] == 1) {
    digitalWrite(TRIGGER5, HIGH);
  }
  beat[4]++;
  if (beat[4] >= stepsPulses[4][0]) {
    beat[4] = 0;
  }

  if (stateArray[5][beat[5]] == 1) {
    digitalWrite(TRIGGER6, HIGH);
  }
  beat[5]++;
  if (beat[5] >= stepsPulses[5][0]) {
    beat[5] = 0;
  }

  triggerOnTime = millis();

}

void triggerOff() {
  trigger = false;
  for (int i = TRIGGER1; i <= TRIGGER6; i++) {
    digitalWrite(i, LOW);
  }
}

void doEncoderA() {
  if (millis() - interruptTime < debounceTime) {
    return;
  }
  // look for a low-to-high on channel A
  if (digitalRead(STEP_BUTTON) == HIGH) {
    if (digitalRead(encoder0PinA) == HIGH) {

      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinB) == LOW) {
        encoder0Pos = encoder0Pos + 1;         // CW
        stepsPulses[channel][0]--;
      }
      else {
        encoder0Pos = encoder0Pos - 1;         // CCW
        stepsPulses[channel][0]++;
      }
    }

    else   // must be a high-to-low edge on channel A
    {
      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinB) == HIGH) {
        encoder0Pos = encoder0Pos + 1;          // CW
        stepsPulses[channel][0]--;
      }
      else {
        encoder0Pos = encoder0Pos - 1;          // CCW
        stepsPulses[channel][0]++;
      }
    }
    if (stepsPulses[channel][0] > 16) {
      stepsPulses[channel][0] = 16;
    }
    if (stepsPulses[channel][0] < 1) {
      stepsPulses[channel][1] =  1;
    }
  }
  else {
    if (digitalRead(encoder0PinA) == HIGH) {

      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinB) == LOW) {
        encoder0Pos = encoder0Pos + 1;         // CW
        stepsPulses[channel][1]--;
      }
      else {
        encoder0Pos = encoder0Pos - 1;         // CCW
        stepsPulses[channel][1]++;
      }
    }

    else   // must be a high-to-low edge on channel A
    {
      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinB) == HIGH) {
        encoder0Pos = encoder0Pos + 1;          // CW
        stepsPulses[channel][1]--;
      }
      else {
        encoder0Pos = encoder0Pos - 1;          // CCW
        stepsPulses[channel][1]++;
      }
    }
    if (stepsPulses[channel][1] > stepsPulses[channel][0]) {
      stepsPulses[channel][1] = 0;
    }
    if (stepsPulses[channel][1] < 0) {
      stepsPulses[channel][1] =  stepsPulses[channel][0];
    }
  }
  interruptTime = millis();
  remainder[16];
  count[16];
  state = 0;
  compute_bitmap(stepsPulses[channel][0], stepsPulses[channel][1], channel);

  // use for debugging - remember to comment out
}

void doEncoderB() {

  if (millis() - interruptTime < debounceTime) {
    return;
  }
  // look for a low-to-high on channel B
  if (digitalRead(STEP_BUTTON) == HIGH) {
    if (digitalRead(encoder0PinB) == HIGH) {

      // check channel A to see which way encoder is turning
      if (digitalRead(encoder0PinA) == HIGH) {
        encoder0Pos = encoder0Pos + 1;         // CW
        stepsPulses[channel][0]--;
      }
      else {
        encoder0Pos = encoder0Pos - 1;         // CCW
        stepsPulses[channel][0]++;
      }
    }

    // Look for a high-to-low on channel B

    else {
      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinA) == LOW) {
        encoder0Pos = encoder0Pos + 1;          // CW
        stepsPulses[channel][0]--;
      }
      else {
        encoder0Pos = encoder0Pos - 1;          // CCW
        stepsPulses[channel][0]++;
      }
    }
    if (stepsPulses[channel][0] > 16) {
      stepsPulses[channel][0] = 16;
    }
    if (stepsPulses[channel][0] < 1) {
      stepsPulses[channel][1] =  1;
    }
  }
  else {
    if (digitalRead(encoder0PinB) == HIGH) {

      // check channel A to see which way encoder is turning
      if (digitalRead(encoder0PinA) == HIGH) {
        encoder0Pos = encoder0Pos + 1;         // CW
        stepsPulses[channel][1]--;
      }
      else {
        encoder0Pos = encoder0Pos - 1;         // CCW
        stepsPulses[channel][1]++;
      }
    }

    // Look for a high-to-low on channel B

    else {
      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinA) == LOW) {
        encoder0Pos = encoder0Pos + 1;          // CW
        stepsPulses[channel][1]--;
      }
      else {
        encoder0Pos = encoder0Pos - 1;          // CCW
        stepsPulses[channel][1]++;
      }
    }
    interruptTime = millis();
    if (stepsPulses[channel][1] > stepsPulses[channel][0]) {
      stepsPulses[channel][1] = 0;
    }
    if (stepsPulses[channel][1] < 0) {
      stepsPulses[channel][1] =  stepsPulses[channel][0];
    }
  }

  remainder[16];
  count[16];
  state = 0;
  compute_bitmap(stepsPulses[channel][0], stepsPulses[channel][1], channel);

}


void color(int r, int g, int b) {
  analogWrite(6, 255 - r);
  analogWrite(5, 255 - g);
  analogWrite(4, 255 - b);
}

void displayPixels() {
  int r, g, b;
  switch (channel) {
    case 0: {
        r = 255;
        g =  150;
        b =  0;
        break;
      }
    case 1: {
        r = 10;
        g = 255;
        b = 10;
        break;
      }
    case 2: {
        r = 0;
        g = 255;
        b = 255;
        break;
      }
    case 3: {
        r = 255;
        g = 64;
        b = 0;
        break;
      }
    case 4: {
        r = 255;
        g = 27;
        b = 27;
        break;
      }
    case 5: {
        r = 255;
        g = 27;
        b = 255;
        color(255, 27, 255);
        break;
      }
  }

  for (int i = 0; i < 16; i++) {

    if (stateArray[channel][15 - i - 1] == 1) {
      pixels.setPixelColor(i, r / 4, g / 4, b / 4);
    }
    else {
      pixels.setPixelColor(i, 2, 2, 2);
    }
  }
  if (stateArray[channel][15] == 1) {
    pixels.setPixelColor(15, r / 4, g / 4, b / 4);
  }
  for (int i = 0; i < (16 - stepsPulses[channel][0]); i++) {
    pixels.setPixelColor(i, 0, 0, 0);
  }

  pixels.setPixelColor(15 - beat[channel], 64, 64, 64);
  pixels.show();
}

void changeChannel() {
  channel++;
  hit = millis();
  if (channel == 6) {
    channel = 0;
  }
  Serial.println(channel);

  switch (channel) {
    case 0: {
        color(255, 150, 0);
        break;
      }
    case 1: {
        color(10, 255, 10);
        break;
      }
    case 2: {
        color(0, 255, 255);
        break;
      }
    case 3: {
        color(255, 64, 0);
        break;
      }
    case 4: {
        color(255, 27, 27);
        break;
      }
    case 5: {
        color(255, 27, 255);
        break;
      }
  }
}

void pauseFunc() {
  pause = !pause;
  if (pause) {
    digitalWrite(PAUSE_LED, HIGH);
    Serial.println("Pause");
  }
  else {
    digitalWrite(PAUSE_LED, LOW);

    Serial.println("Play");
  }
}
void writeArray() {
  int i = 0;
  for (int k = 0; k < 6; k++) {
    for (int n = 0; n < 2; n++) {
      EEPROM.write(i, stepsPulses[k][n]);
      i++;
    }
  }
  digitalWrite(PAUSE_LED, HIGH);
  delay(50);
  digitalWrite(PAUSE_LED, LOW);
  delay(50);
  digitalWrite(PAUSE_LED, HIGH);
  delay(50);
  digitalWrite(PAUSE_LED, LOW);
  delay(50);
  digitalWrite(PAUSE_LED, HIGH);
  delay(50);
  digitalWrite(PAUSE_LED, LOW);
  delay(50);
  if(pause){
    digitalWrite(PAUSE_LED, HIGH);
  }
}

void calculateAll(){
  for(int i = 0; i < 6; i++){
    remainder[16];
  count[16];
  state = 0;
  compute_bitmap(stepsPulses[i][0], stepsPulses[i][1], i);
  }
}

