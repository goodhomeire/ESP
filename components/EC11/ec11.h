#ifndef _EC11_H_
#define _EC11_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define SW GPIO_NUM_6
#define SA GPIO_NUM_5
#define SB GPIO_NUM_4

typedef enum
{
    sw_clr,
    sw_click,  //单击
    sw_double, //双击
    sw_long    //长按
} ec11_key_result_type;

typedef enum
{
    ec11_task_is_key,   //按键事件
    ec11_task_is_encode //编码事件
} ec11_task_result_type;

#define key_time_out 300   //长按多少ms识别为长按
#define key_double_out 400 //双击间隔在多少ms内有效 如不需要双击功能 值设为1
#define shake_t 5          //抖动抑制时间  默认5ms

class Skey
{

public:
    /*
        初始化编码器，绑定SW、SA\SB的IO口，如发现旋转方向反了，将SA\SB的位置调换即可
        按键事件回调函数注册
            ec11_task_result_type:
                ec11_task_is_key,   //按键事件
                ec11_task_is_encode //编码事件
                用于回调函数内判断事件输入类型
            rusult_value：
                如果为按键事件，则值为
                    sw_click,  //单击
                    sw_double, //双击
                    sw_long    //长按
                如果为编码事件，则值为一般逆时针负数顺时针正数
    */
    void begin(uint8_t sw, uint8_t sa, uint8_t sb, void (*func)(ec11_task_result_type task_type, int16_t rusult_value));

    //按键事件注销
    void detach();

    //启用按键中断  写入EEPROM时必须关闭
    void int_work();
    //关闭按键中断
    void int_close();

    //加速算法是否启用 val:true/false 默认关闭false
    void speed_up(bool val);

    //加速算法最大加速值,1-65535  默认100
    void speed_up_max(uint16_t val);

    //加速算法手感  默认15  0-255
    void speed_up_feel(uint8_t val);

    //双击识别是否启用 val:true/false 默认开启true
    void double_click(bool val);

    /*
        回调函数触发任务后，用于获取按键输入类型，回调函数结束后将清零
        sw_click,   //单击
        sw_double,  //双击
        sw_long     //长按
    */
    ec11_key_result_type get_key_result();

    /*
        回调函数触发任务后，用于获取旋转编码的输入值，回调函数结束后将清零
        一般逆时针负数顺时针正数
    */
    int16_t get_encode_value();

private:
    void (*attch_p)(ec11_task_result_type task_type, int16_t rusult_value) = NULL;
    void task();
    void key_mode_read();
    friend void io_sw_int(void *arg);
    friend void io_sa_sb_int(void *arg);
    friend void key_timer(void *arg);
    bool work_flg = false;
    bool state = 1;
    bool state_buf = 1;
    uint8_t state_count = 0;
    bool speed_up_flg = false;
    uint16_t speed_up_max_num = 100;
    uint8_t speed_up_feel_count = 15;
    uint16_t double_click_count = key_double_out;
    ec11_task_result_type _take_type_ec_sw = ec11_task_is_key;

    //编码值调用识别 未启用加速算法:ec_left左转(值-1) ec_right右转(值1)   启用加速算法后值为±1至±speed_up_max()所设置值
    int16_t ec_buf = 0;
    //按键值调用识别 sw_click单击 sw_double双击 sw_long长按
    ec11_key_result_type sw_buf = sw_clr;
};

extern Skey ec11;

#ifdef __cplusplus
}
#endif
#endif