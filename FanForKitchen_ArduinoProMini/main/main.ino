#define dimPin 4
#define zeroCrossPin 2
#define bladePin 3
#define blades 7 // количество лопастей
#define butSlowModePin 6
#define butFastModePin 7
#define periodCounter 262 // мс, время подсчета лопастей, максимально 262 мс
#define INIT_ADDR 1023  // номер ячейки ключа первого запуска
#define INIT_KEY 85     // ключ первого запуска. 01010101

#define PID_INTEGER
#include "GyverPID.h"
#include "GyverTimers.h"
#include "GyverFilters.h"
#include "GyverButton.h"
#include <EEPROM.h>

enum {
  SLOW,
  MEDIUM,
  FAST
} speedMode;
uint16_t FAN_SPEED_SLOW; // = 1400;
uint16_t FAN_SPEED_MEDIUM; // = 1720;
uint16_t FAN_SPEED_FAST; // = 2500;
uint16_t bladesCount;
uint16_t fanSpeed; // оборотов в минуту
uint16_t bladeSpeed; // сколько лопастей должно пройти под датчиков за время periodCounter
uint16_t dimmNullPeriod; // dimmNullPeriod  0 - 10000
uint8_t isBladePrev; // значение считанного сигнала на предыдущем шаге
uint8_t isBladeNow; // значение считанного сигнала

volatile boolean ifCountingBlades; // флаг, ведётся ли подсчёт лопастей
boolean isEditMode; // флаг, что мы в режиме выставлеения скорости оборотов

GMedian<5, uint8_t> filterBlade;
GButton butSlowMode(butSlowModePin, HIGH_PULL);
GButton butFastMode(butFastModePin, HIGH_PULL);
uint32_t lastTimeClick;

GyverPID regulator(3, 20, 1); // (kp, ki, kd, dt)

void setup() {
  //pinMode(butSlowModePin, INPUT_PULLUP);
  //pinMode(butFastModePin, INPUT_PULLUP);

  pinMode(bladePin, INPUT);

  pinMode(zeroCrossPin, INPUT);                 // настраиваем порт на вход для отслеживания прохождения сигнала через ноль
  attachInterrupt(0, ISR_ZeroCross, RISING);    // настроить срабатывание прерывания на восходящий уровень

  pinMode(dimPin, OUTPUT);
  digitalWrite(dimPin, LOW);

  pinMode(LED_BUILTIN, OUTPUT);

  // прочитаем из EEPROM значения скоростей SLOW, MEDIUM, FAST
  if (EEPROM.read(INIT_ADDR) != INIT_KEY) { // первый запуск
    EEPROM.write(INIT_ADDR, INIT_KEY);    // записали ключ
    EEPROM.put(0, 1400); // FAN_SPEED_SLOW
    EEPROM.put(2, 1700); // FAN_SPEED_MEDIUM
    EEPROM.put(4, 2500); // FAN_SPEED_FAST
  } 
  EEPROM.get(0, FAN_SPEED_SLOW);
  EEPROM.get(2, FAN_SPEED_MEDIUM);
  EEPROM.get(4, FAN_SPEED_FAST);

  //fanSpeed = FAN_SPEED_MEDIUM; // оборотов в минуту
  //SetSpeed(FAN_SPEED_MEDIUM);
  //bladeSpeed = (uint16_t) ((uint32_t)fanSpeed * blades * periodCounter / 60000); // сколько лопастей пройдёт под датчиком за время счёта
  //dimmNullPeriod = 5000;

  regulator.setDirection(REVERSE);  // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
  regulator.setLimits(0, 10000);    // пределы
  regulator.setpoint = bladeSpeed;  // сообщаем регулятору величину, которую он должен поддерживать
  regulator.setDt(periodCounter);   // установка времени итерации в мс

  // настроим таймер
  Timer1.setPeriod((uint32_t) periodCounter * 1000); // установим таймер на подсчет лопастей в мкс
  Timer1.enableISR(CHANNEL_A);

  Timer2.enableISR(CHANNEL_A);
}

void loop() {
  // расчёт значения диммировния
  if (!ifCountingBlades) {
    regulator.input = bladesCount;   // сообщаем регулятору текущую скорость лопастей
    dimmNullPeriod = regulator.getResult();

    bladesCount = 0;
    ifCountingBlades = true;
    Timer1.restart();
  }

  // подсчёт скорости вращения вентилятор в лопостях
  isBladePrev = isBladeNow;
  isBladeNow = filterBlade.filtered(digitalRead(bladePin));
  if (ifCountingBlades && isBladePrev != isBladeNow) {       
    bladesCount++;
  } 
 
  // опрос кнопок выбора режима работы
  butSlowMode.tick();
  butFastMode.tick();
  switch (butSlowMode.getClicks()) { 
    case 3:
      SetSpeed(SLOW);
      isEditMode = true;
      LedBlink(1);
      lastTimeClick = millis();
      break;
    case 4:
      SetSpeed(MEDIUM);
      isEditMode = true;
      LedBlink(2);
      lastTimeClick = millis();
      break;
    case 5:
      SetSpeed(FAST);
      isEditMode = true;
      LedBlink(3);
      lastTimeClick = millis();
      break;
  }
  switch (butFastMode.getClicks()) { 
    case 3:
      BlinkRateSpeed(FAN_SPEED_SLOW);
      break;
    case 4:
      BlinkRateSpeed(FAN_SPEED_MEDIUM);
      break;
    case 5:
      BlinkRateSpeed(FAN_SPEED_FAST);
      break;
  }
    
  if (!isEditMode) { // если мы не в режиме опроса кнопок, то обрабатываем нажатия обычным способом, иначе игнорируем
    if (butSlowMode.isHold()) { // если переключатель скорости работы выбран на минимальную скорость
      SetSpeed(SLOW);
    } else if (butFastMode.isHold()) { // максимальная скорость работы
      SetSpeed(FAST);
    } else { // средняя скорость работы
      SetSpeed(MEDIUM);
    }
  } else {
    if (butSlowMode.isClick()){
      switch (speedMode) {
        case SLOW:
          FAN_SPEED_SLOW -= 20;
          break;
        case MEDIUM:
          FAN_SPEED_MEDIUM -= 20;
          break;
        case FAST:
          FAN_SPEED_FAST -= 20;
          break;
      }
      SetSpeed(speedMode, true);
      lastTimeClick = millis();
    }
    if (butFastMode.isClick()){
      switch (speedMode) {
        case SLOW:
          FAN_SPEED_SLOW += 20;
          break;
        case MEDIUM:
          FAN_SPEED_MEDIUM += 20;
          break;
        case FAST:
          FAN_SPEED_FAST += 20;
          break;
      }
      SetSpeed(speedMode, true);
      lastTimeClick = millis();
    }
    
    // если в течении 15 сек не нажимались кнопки, запишем в EEPROM
    if (millis() - lastTimeClick > 15000) { 
      switch (speedMode) { // запишем в EEPROM значения скоростей SLOW, MEDIUM, FAST
        case SLOW:
          EEPROM.put(0, FAN_SPEED_SLOW);
          break;
        case MEDIUM:
          EEPROM.put(2, FAN_SPEED_MEDIUM);
          break;
        case FAST:
          EEPROM.put(4, FAN_SPEED_FAST);
          break;
      }
      isEditMode = false;
      LedBlink(3);      
    }
  }
}

//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------
ISR(TIMER1_A) {
  ifCountingBlades = false;
  Timer1.pause();
}

void ISR_ZeroCross() {    // обработка внешнего прерывания на пересекание нуля снизу
  digitalWrite(dimPin, LOW);
  Timer2.setPeriod(dimmNullPeriod);
}

ISR(TIMER2_A) {
  digitalWrite(dimPin, HIGH);
  Timer2.stop();
}
//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------
