void set_pwm_speed(long int speed1, long int speed2, long int speed3, long int speed4, FILE *motor1, FILE *motor2, FILE *motor3, FILE *motor4)
{
	char speed_c1[] = "PWM_SPEED";
	char speed_c2[] = "PWM_SPEED";
	char speed_c3[] = "PWM_SPEED";
	char speed_c4[] = "PWM_SPEED";

	rewind(motor1);
	rewind(motor2);
	rewind(motor3);
	rewind(motor4);

	speed1 += INCREMENT;
	speed2 += INCREMENT;
	speed3 += INCREMENT;
	speed4 += INCREMENT;
	sprintf(speed_c1, "%ld", speed1);
	sprintf(speed_c2, "%ld", speed2);
	sprintf(speed_c3, "%ld", speed3);
	sprintf(speed_c4, "%ld", speed4);
	fputs(speed_c1, motor1);
	fputs(speed_c2, motor2);
	fputs(speed_c3, motor3);
	fputs(speed_c4, motor4);
}