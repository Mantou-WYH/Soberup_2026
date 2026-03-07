#ifndef CODE_BATTERY_DETECT_H_
#define CODE_BATTERY_DETECT_H_
#include "zf_driver_adc.h"
#include "zf_driver_pit.h"

void voltage_detect_init(void);
void detect_main(void);

#endif /* CODE_BATTERY_DETECT_H_ */
