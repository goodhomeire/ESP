#include "motor.h"

Motor motor;
uint32_t current_speed = 0;//should between 50 ~ 75;
uint8_t default_speed 60
uint64_t current_time = 0; 
uint32_t current_cycle = 0;
uint8_t cycle_stash = 0;
uint32_t current_freq = (per_cycle_pwm*default_speed)/60

IRAM_ATTR void cycle_cnt()
{
    cycle_stash+=1;
    if(cycle_stash==64)
    {
        current_cycle++;
        cycle_stash=0;
    }
}
void Motor::init()
{
    //step_pin and dir interal pulldown
    /*cycle_pin should extrnal pullup  enable_pin should set to high to disable motor*/
    gpio_config_t io_config;
    io_config.mode = GPIO_MODE_OUTPUT;
    io_config.pin_bit_mask = (1ULL<<dir_pin);
    io_config.pull_down_en = GPIO_PULLUP_DISABLE;
    io_config.pull_up_en = GPIO_PULLDOWN_DISABLE;
    io_config.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_config);
    gpio_set_level(dir_pin,1);

    io_config.pin_bit_mask = (1ULL<<en_motor_pin);
    gpio_config(&io_config);
    gpio_set_level(dir_pin,1);

    io_config.mode = GPIO_MODE_INPUT;
    io_config.pin_bit_mask = (1ULL<<cycle_cnt_pin);
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;
    io_config.intr_type = GPIO_INTR_POSEDGE;
    gpio_config(&io_config);
    gpio_isr_handler_add(cycle_cnt_pin, cycle_cnt, NULL);

    ledc_timer_config_t lecd_timer;
    lecd_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    lecd_timer.clk_cfg = LEDC_AUTO_CLK;
    lecd_timer.freq_hz = 
}


