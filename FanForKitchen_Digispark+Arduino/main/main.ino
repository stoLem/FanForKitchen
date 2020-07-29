#define dimPin PB0
#define zeroPin PB2
#include <core_times.h>
volatile int tic, Dimmer;

digitalWriteF(uint8_t pin, uint8_t val);

void setup() {
  pinMode(dimPin, OUTPUT);
  digitalWriteF(dimPin, 0);
  pinMode(zeroPin, INPUT);                 // настраиваем порт на вход для отслеживания прохождения сигнала через ноль
  attachInterrupt(0, detect_up, FALLING);  // настроить срабатывание прерывания interrupt0 на pin 2 на низкий уровень

  StartTimer1(timer_interrupt, 40);        // время для одного разряда ШИМ
  StopTimer1();                            // остановить таймер

}

void loop() {
  if (digitalRead(BTN1_PIN) == 0){
    if (Dimmer != 99) Dimmer = 99;
  } else if (digitalRead(BTN2_PIN) == 0){
    if (Dimmer != 0) Dimmer = 0;
  } else {
    if (Dimmer != 85) Dimmer = 85;
  }
}

//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------
void timer_interrupt() {       // прерывания таймера срабатывают каждые 40 мкс
  tic++;                       // счетчик
  if (tic > Dimmer)            // если настало время включать ток
    digitalWriteF(dimPin, 1)  
}

void  detect_up() {    // обработка внешнего прерывания на пересекание нуля снизу
  tic = 0;                                  // обнулить счетчик
  ResumeTimer1();                           // перезапустить таймер
  attachInterrupt(0, detect_down, RISING);  // перенастроить прерывание
}

void  detect_down() {  // обработка внешнего прерывания на пересекание нуля сверху
  tic = 0;                                  // обнулить счетчик
  StopTimer1();                             // остановить таймер
  digitalWriteF(dimPin, 0);                  // вырубить ток
  attachInterrupt(0, detect_up, FALLING);   // перенастроить прерывание
}
//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------

digitalWriteF(uint8_t pin, uint8_t val){
  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  volatile uint8_t *out;

  out = portOutputRegister(port);

  if (val == LOW) {
    *out &= ~bit;
  } else {
    *out |= bit;
  }  
}
