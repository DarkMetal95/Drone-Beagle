#!/bin/bash 

echo am33xx_pwm > /sys/devices/bone_capemgr.9/slots
echo bone_pwm_P9_14 > /sys/devices/bone_capemgr.9/slots
echo bone_pwm_P8_19 > /sys/devices/bone_capemgr.9/slots

sleep 10

echo 0 > /sys/devices/ocp.3/pwm_test_P8_19.16/polarity
echo 20000000 > /sys/devices/ocp.3/pwm_test_P8_19.16/period
echo 700000 > /sys/devices/ocp.3/pwm_test_P8_19.16/duty

echo 0 > /sys/devices/ocp.3/pwm_test_P9_14.15/polarity
echo 20000000 >	/sys/devices/ocp.3/pwm_test_P9_14.15/period
echo 700000 > /sys/devices/ocp.3/pwm_test_P9_14.15/duty

echo bone_pwm_P9_16 > /sys/devices/bone_capemgr.9/slots
echo bone_pwm_P8_13 > /sys/devices/bone_capemgr.9/slots

sleep 10

echo 700000 > /sys/devices/ocp.3/pwm_test_P9_16.17/duty
echo 700000 > /sys/devices/ocp.3/pwm_test_P8_13.18/duty

