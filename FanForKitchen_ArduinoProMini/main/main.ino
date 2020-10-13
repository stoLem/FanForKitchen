#define dimPin 4
#define zeroCrossPin 2
#define bladePin 3
#define blades 7 // количество лопастей
#define butSlowModePin 6
#define butFastModePin 7

#define FAN_SPEED_SLOW 1450
#define FAN_SPEED_MID 1720
#define FAN_SPEED_FAST 2500
#define periodCounter 262 // мс, время подсчета лопастей, максимально 262 мс

#define PID_INTEGER
#include "GyverPID/GyverPID.h"
#include "GyverTimers.h"

volatile uint16_t dimmNullPeriod; // dimmNullPeriod  0 - 10000
volatile uint16_t bladesCount;
uint16_t fanSpeed; // оборотов в минуту
uint16_t bladeSpeed; // сколько лопастей должно пройти под датчиков за время periodCounter
volatile boolean ifCountingBlades; // флаг, ведётся ли подсчёт лопастей

GyverPID regulator(3, 20, 1); // (kp, ki, kd, dt)

void setup() {
  //Serial.begin(115200);

  pinMode(butSlowModePin, INPUT_PULLUP);
  pinMode(butFastModePin, INPUT_PULLUP);

  pinMode(bladePin, INPUT);
  attachInterrupt(1, ISR_BladeCounter, RISING); // настроить срабатывание прерывания на восходящий уровень
  
  pinMode(zeroCrossPin, INPUT);                 // настраиваем порт на вход для отслеживания прохождения сигнала через ноль
  attachInterrupt(0, ISR_ZeroCross, RISING);    // настроить срабатывание прерывания на восходящий уровень

  pinMode(dimPin, OUTPUT);
  digitalWrite(dimPin, LOW);  

  fanSpeed = FAN_SPEED_MID; // оборотов в минуту
  bladeSpeed = (uint16_t) ((uint32_t)fanSpeed * blades * periodCounter / 60000); // сколько лопастей пройдёт под датчиком за время счёта
  dimmNullPeriod = 5000;
  
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
  if (!ifCountingBlades) {
    //Serial.print(bladesCount); Serial.print(' ');    
    //Serial.println(dimmNullPeriod);
    //Serial.println(bladeSpeed); 

    regulator.input = bladesCount;   // сообщаем регулятору текущую температуру
    dimmNullPeriod = regulator.getResult();

    bladesCount = 0;
    ifCountingBlades = true;
    Timer1.restart();
  }

//  // настройка
//  if (Serial.available() > 1) {
//    char incoming = Serial.read();
//    float value = Serial.parseFloat();
//    switch (incoming) {
//      case 'p': regulator.Kp = value; break;  
//      case 'P': Serial.println(regulator.Kp); break;   
//      case 'i': regulator.Ki = value; break;
//      case 'I': Serial.println(regulator.Ki); break; 
//      case 'd': regulator.Kd = value; break;
//      case 'D': Serial.println(regulator.Kd); break; 
//      case 's': 
//        fanSpeed = value;
//        bladeSpeed = (uint16_t) ((long)fanSpeed * blades * periodCounter / 60000);
//        regulator.setpoint = bladeSpeed;
//        break;
//    }
//  }

  // опрос кнопок выбора режима работы
  if (digitalRead(butSlowModePin) == 0) { // если переключатель скорости работы выбран на минимальную скорость 
    if (fanSpeed != FAN_SPEED_SLOW) {
      fanSpeed = FAN_SPEED_SLOW;
      bladeSpeed = (uint16_t) ((uint32_t)fanSpeed * blades * periodCounter / 60000);
      regulator.setpoint = bladeSpeed;
    }
  } else if (digitalRead(butFastModePin) == 0) { // максимальная скорость работы
    if (fanSpeed != FAN_SPEED_FAST) {
      fanSpeed = FAN_SPEED_FAST;
      bladeSpeed = (uint16_t) ((uint32_t)fanSpeed * blades * periodCounter / 60000);
      regulator.setpoint = bladeSpeed;
    }
  } else { // средняя скорость работы
    if (fanSpeed != FAN_SPEED_MID) {
      fanSpeed = FAN_SPEED_MID;
      bladeSpeed = (uint16_t) ((uint32_t)fanSpeed * blades * periodCounter / 60000);
      regulator.setpoint = bladeSpeed;
    }
  }
}

//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------
ISR(TIMER1_A) {  
  ifCountingBlades = false;
  Timer1.pause();
}

void ISR_BladeCounter() { // обработка внешнего прерывания при прохождении лопасти под датчиком
  if (ifCountingBlades) {
    bladesCount++;
  }
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
