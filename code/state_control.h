#ifndef CODE_STATE_CONTROL_H_
#define CODE_STATE_CONTROL_H_


typedef enum {
    normal,
    fork
}state;

extern state car_state;

void state_update();
void show_state();


#endif /* CODE_STATE_CONTROL_H_ */
