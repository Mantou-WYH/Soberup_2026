#ifndef CODE_STATE_CONTROL_H_
#define CODE_STATE_CONTROL_H_


typedef enum {
    normal,
    fork
}state;

extern state car_state;

void state_update();


#endif /* CODE_STATE_CONTROL_H_ */
