#include <Arduino.h>

#define BUZZER_PIN 21
#define BUTTON_PIN 26
#define DEBOUNCE_DELAY 200  

hw_timer_t *My_timer = NULL;
volatile int noteIndex = 0;
int tempoLevels[5] = {1050000, 850000, 650000, 450000, 200000};  //ความเร็ว
int currentTempo = 2;  // ความเร็วเริ่มต้น

int notes[] = {1568, 1760, 1568, 1397, 1319, 1397, 1568, 1175, 1319, 1397};  
int noteDurations[] = {400, 400, 400, 400, 400, 400, 800, 400, 400, 800}; //ความยาวของโน๊ตที่จะเล่น

volatile unsigned long previousMillis = 0;
int duration = 0;
volatile unsigned long lastButtonPress = 0;  

void IRAM_ATTR onTimer() {
  tone(BUZZER_PIN, notes[noteIndex]);
  previousMillis = millis();
  duration = noteDurations[noteIndex];
  noteIndex = (noteIndex + 1) % 10;
}

void IRAM_ATTR onButtonPress() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastButtonPress > DEBOUNCE_DELAY) {  //DEBOUNCEปุ่มตอนกด
    lastButtonPress = currentMillis;
    currentTempo = (currentTempo + 1) % 5;
    Serial.print("Current Tempo: ");
    Serial.println(currentTempo);
    timerAlarm(My_timer, tempoLevels[currentTempo], true, 0);
  }
}

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), onButtonPress, FALLING);
  
  Serial.begin(115200);

  My_timer = timerBegin(1000000);
  timerAttachInterrupt(My_timer, &onTimer);
  timerAlarm(My_timer, tempoLevels[currentTempo], true, 0);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= duration) {
    previousMillis = currentMillis;
    noTone(BUZZER_PIN);
    tone(BUZZER_PIN, notes[noteIndex]);
    duration = noteDurations[noteIndex];
  }
}
