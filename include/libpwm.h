/* 
 * Defines
 */

#define PWM_DUTY_FILE1			"/sys/devices/ocp.3/pwm_test_P8_13.18/duty"
#define PWM_DUTY_FILE3			"/sys/devices/ocp.3/pwm_test_P8_19.16/duty"
#define PWM_DUTY_FILE2			"/sys/devices/ocp.3/pwm_test_P9_14.15/duty"
#define PWM_DUTY_FILE4			"/sys/devices/ocp.3/pwm_test_P9_16.17/duty"
#define PWM_SPEED			1000000

void set_pwm_speed(int speed1, int speed2, int speed3, int speed4, FILE *motor1, FILE *motor2, FILE *motor3, FILE *motor4);