#define dimPin 0
#define btnSlowlyPin 1
#define zeroPin 2
#define btnFasterPin 3
#define btnMinPin 4
#define btnMaxPin 5
#include <core_timers.h>

volatile uint8_t Dimmer; // 1 - 161

void setup() {
  pinMode(dimPin, OUTPUT);
  digitalWrite(dimPin, LOW);

  pinMode(btnSlowlyPin, INPUT_PULLUP);
  pinMode(btnFasterPin, INPUT_PULLUP);
  pinMode(btnMinPin, INPUT_PULLUP);
  pinMode(btnMaxPin, INPUT_PULLUP);

  pinMode(zeroPin, INPUT);                 // настраиваем порт на вход для отслеживания прохождения сигнала через ноль
  attachInterrupt(0, detect_up, RISING);   // настроить срабатывание прерывания на восходящий уровень

  Dimmer = 50;
}

void loop() {
//  if (digitalRead(BTN1_PIN) == 0){
//    if (Dimmer != 99) Dimmer = 99;
//  } else if (digitalRead(BTN2_PIN) == 0){
//    if (Dimmer != 0) Dimmer = 0;
//  } else {
//    if (Dimmer != 85) Dimmer = 85;
//  }
}

//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------
void detect_up() {    // обработка внешнего прерывания на пересекание нуля снизу                                                        
  digitalWrite(dimPin, LOW);
  
  Timer0_SetOutputCompareMatchA(Dimmer);        
  Timer0_EnableOutputCompareInterruptA();
  Timer0_ClockSelect(Timer0_Prescale_Value_1024);// запустить таймер
}

void timer_interrupt() {      
  digitalWrite(dimPin, HIGH);
  Timer0_SetToPowerup(); // остановить таймер
}
//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------
