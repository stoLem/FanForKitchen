/*
	Пример использования медианного фильтра.
*/

#include "median.h"

// указываем размер окна и тип данных в <>
GMedian<3, uint16_t> testFilter;    

void setup() {
  Serial.begin(9600);
}

void loop() {
  delay(80);
  int value = analogRead(0);
  // добавляем шум "выбросы"
  value += random(2) * random(2) * random(-1, 2) * random(50, 250);
  Serial.print(value);
  Serial.print(',');  
  value = testFilter.filtered(value);
  Serial.println(value);
}
