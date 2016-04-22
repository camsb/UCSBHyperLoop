#include "sensor_data.h"

void TIMER1_IRQHandler(void){
  gatherData = 1;
}
