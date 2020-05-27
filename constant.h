#ifndef CONSTANT_H
#define CONSTANT_H

static const int RECEIVE = 0;
static const int SEND = 1;
static const int TENSO_CALIB = 2;
static const int MIO_SET_0 = 3;

//Registers
static const int MOVEMENT = 1;
static const int SHAKE_TIME = 2;
static const int COOL_TIME = 3;
static const int STOP_CURRENT = 4;
static const int STOP_STRENGHT = 5;
static const int SINGLE_MOVEMENT = 6;
static const int CONTROL_MODE = 7;
static const int SERVO_POSITION = 8;
static const int NOMINAL_TEMPERATURE = 9;
static const int STOP_TEMPERATURE = 10;
static const int START_SERVO_ANGLE = 11;
static const int STOP_SERVO_ANGLE = 12;

//SINGLE_MOVEMENT registers
static const int OPEN = 1;
static const int CLOSE = 2;
static const int INVERT = 3;
static const int CANCEL_INVERT = 4;

//CONTROL_MODE registers
static const int HDLC = 1;
static const int SERVO = 2;
static const int DC = 3;

//
static const int PROSITY = 30;

#endif // CONSTANT_H
