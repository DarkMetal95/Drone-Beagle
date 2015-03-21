#ifndef PWM_H
#define PWM_H

/* 
 * Defines
 */

#define PWM_DUTY_FILE1			"/sys/devices/ocp.3/pwm_test_P8_13.18/duty"
#define PWM_DUTY_FILE3			"/sys/devices/ocp.3/pwm_test_P8_19.16/duty"
#define PWM_DUTY_FILE2			"/sys/devices/ocp.3/pwm_test_P9_14.15/duty"
#define PWM_DUTY_FILE4			"/sys/devices/ocp.3/pwm_test_P9_16.17/duty"
#define PWM_SPEED			1000000
#define INCREMENT			1000

void set_pwm_speed(long int speed1, long int speed2, long int speed3, long int speed4, FILE *motor1, FILE *motor2, FILE *motor3, FILE *motor4);

#endif
