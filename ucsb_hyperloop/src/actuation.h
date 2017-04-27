#ifndef actuation_h
#define actuation_h

#define MAX_THROTTLE_VOLTAGE 5 //[V]

void performActuation();
void set_motor_throttle(uint8_t motor_num, float voltage);

#endif
