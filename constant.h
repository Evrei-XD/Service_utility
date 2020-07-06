#ifndef CONSTANT_H
#define CONSTANT_H

static const int RECEIVE = 0;
static const int SEND = 1;
static const int TENSO_CALIB = 2;
static const int MIO_SET_0 = 3;
static const int MAX_BATTERY_ENERGY_TEORETICAL = 31968; // = 7,4V * 1,2A/h * 3600s
static const float MIN_BATTERY_ENERGY_TEORETICAL = 26784; // = 6,2V * 1,2A/h * 3600s
static const int NUMBER_CURRENT_ITERATION_FOR_CUT_OFF = 5;
static const int MAX_CICLOGRAMM_STAGE_TIME = 3000; //задаёт в миллисекундах максимальное время нахождения на определённом этапе циклограммы(игнорирует все остальные параметры)
static const int MIN_CICLOGRAMM_STRENGHT = 100; //задаёт в граммах минимальную силу на тензодатчике, по достижении которой начинает отсчитыватися время сжатия
static const bool VANIA_MATVEI = true; //true-Vania  false-Matvei reseive schem
// 36288 - 26784 = 9504‬ * 3,3636

// 50%
// 15984‬ теоретическое значение
// 15983,8‬ расчётное

// 30%
// 9590,4 ‬теоретическое значение
// 9590,3 расчётное

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
static const int TEST_HDLC_ON = 5;
static const int TEST_HDLC_OFF = 6;

//CONTROL_MODE registers
static const int HDLC = 1;
static const int SERVO = 2;
static const int DC = 3;

//minimum delay between packets
static const int PROSITY = 30;

//cyclogram station
enum {
    FIRST_STATE = 1,
    SECOND_STATE = 2,
    THRID_STATE = 3,
    FOURTH_STATE = 4,
    FIFTH_STATE = 5};


#endif // CONSTANT_H
