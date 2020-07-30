#define dimPin 0
#define btnSlowlyPin 1
#define zeroPin 2
#define btnFasterPin 3
#define btnMinPin 4
#define btnMaxPin 5

#include <core_timers.h>
#include <avr/eeprom.h>

volatile uint8_t Dimm0, DimCurrent; // 1 - 156

void setup() {
  pinMode(dimPin, OUTPUT);
  digitalWrite(dimPin, LOW);

  pinMode(btnSlowlyPin, INPUT_PULLUP);
  pinMode(btnFasterPin, INPUT_PULLUP);
  pinMode(btnMinPin, INPUT_PULLUP);
  pinMode(btnMaxPin, INPUT_PULLUP);

  pinMode(zeroPin, INPUT);                 // настраиваем порт на вход для отслеживания прохождения сигнала через ноль
  attachInterrupt(0, detect_up, RISING);   // настроить срабатывание прерывания на восходящий уровень

  Dimm0 = eeprom_read_byte(0);
}

void loop() {
  // опрос кнопок выбора режима работы
  if (digitalRead(btnMinPin) == 0){ // если переключатель скорости работы выбран на минимальную скорость 
    interrupts();
    DimCurrent = Dimm0;
  } else if (digitalRead(btnMaxPin) == 0){ // максимальная скорость работы
    noInterrupts();
    digitalWrite(dimPin, HIGH);
  } else { // средняя скорость работы
    interrupts();
    Dimm0 > 5 ? DimCurrent = Dimm0 - 5 : DimCurrent = 1;
  }

  // опрос кнопок изменения диммирования
  if (digitalRead(btnSlowlyPin) == 0){  
    if (Dimm0 > 1) {
      Dimm0--;
      eeprom_write_byte(0, Dimm0);
    }
  } else if (digitalRead(btnFasterPin)){
    if (DimCurrent < 156) {
      Dimm0++;
      eeprom_write_byte(0, Dimm0);
    }
  }
}

//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------
void detect_up() {    // обработка внешнего прерывания на пересекание нуля снизу                                                        
  digitalWrite(dimPin, LOW);
  
  Timer0_SetOutputCompareMatchA(DimCurrent);        
  Timer0_EnableOutputCompareInterruptA();
  Timer0_ClockSelect(Timer0_Prescale_Value_1024);// запустить таймер
}

void timer_interrupt() {      
  digitalWrite(dimPin, HIGH);
  Timer0_SetToPowerup(); // остановить таймер
}
//----------------------ОБРАБОТЧИКИ ПРЕРЫВАНИЙ--------------------------
