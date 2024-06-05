#ifndef MOTOR_H
#define MOTOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "driver/ledc.h"


#define step_pin GPIO_NUM_10
#define dir_pin GPIO_NUM_21
#define cycle_cnt_pin GPIO_NUM_20
#define en_motor_pin GPIO_NUM_8


#define per_cycle_pwm 4096//电机每对外表现转一圈，相当于发送4096个脉冲信号。
#define cycle_per_rate = 64 //减速齿轮比，相当于电机转64圈对外表现为一圈



class Motor
{
private:
    /* data */
public:
/*需要实现的操作步进电机的接口
一、初始化函数：实现对电机控制引脚的初始化、检测圈数的引脚需要使能外部中断用于计圈
*/
void init();
void begin(uint8_t speed,uint8_t time_min);//rmp as unit
void stop(void);
void rollback(void);
void change_speed(uint8_t target_speed,uint8_t target_time);



    
};


extern Motor motor;


#endif