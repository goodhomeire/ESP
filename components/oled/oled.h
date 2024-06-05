#ifndef __OLED_H__
#define __OLED_H__
#ifdef __cplusplus
extern "C" {
#endif
/*
    此库用于oled屏幕的显示，参看启帆科创代码，修改为适合自己的代码。
*/
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "oled_font.h"

// oled device config
const uint8_t OLED_ADDR = 0x3c;

#define SDAPIN GPIO_NUM_1
#define SCLPIN GPIO_NUM_0

//屏幕缓存参数
#define XLevelL 0x00   // 驱动地址偏移
#define XLevelH 0x10   // 驱动地址偏移
#define YLevel 0xB0    // 驱动地址偏移
#define Brightness 127 // 屏幕初始化亮度0-255
#define WIDTH 128      // 宽度
#define HEIGHT 32      // 高度
#define PAGE_SIZE 4    // 竖直分辨率/8

//屏幕内容运动方向
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

//屏幕显示参数
#define INVERSE 1
#define NORMAL 0

#define white 1 //白字黑底(默认)
#define black 0

#define AUTO_line_feed 1 //自动换行
#define NO_line_feed 0


//定义字符和汉字空间大小
#define Font_Size_8 8
#define Font_Size_16 16
#define CH_Size_16 16  
#define CH_Size_24 24
#define CH_Size_32 32

//oled mode
#define OLED_CMD 0
#define OLED_DATA 1

class Oled
{
private:
    /* data */
public:
    void choose_clr(uint8_t x, uint8_t y, uint8_t width, uint8_t height); 
    void choose_refresh(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
    
    /*设定显示函数边界 xy为左上角,xxyy为右下角,超出部分将在显示字符串\中文\位图 显示函数中不显示
    x分辨率为1 y为8
    */
    void xy_set(uint8_t x, uint8_t y, uint8_t xx, uint8_t yy);
    /*
    指定区域起点和宽(width)高(height)  指定单次滚动格数cont(y滚动为1-8格，x滚动不限制)
    指定滚动方向对该区域进行滚动  x分辨率为1  y和width为8
    指定总共滚动多少次roll_cont（roll_cont*cont格）
    roll_mode有上下左右四个方向，参数如下：
    UP
    DOWN
    LEFT
    RIGHT
    */
    void roll(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t cont, uint8_t roll_mode, uint8_t roll_cont);
    // 写一个字符,size:8/16,mode 0:白底黑字/1：黑底白字,x分辨率为1/y为1
    void write_char(int16_t x, int16_t y, char chr, uint8_t Size, uint8_t mode);
    // 写字符串,size:8/16,mode 0:白底黑字/1：黑底白字,fill:1:多余自动换行 0:多余不显示,x分辨率为1/y为1
    void str(int16_t x, int16_t y, const void *chr, uint8_t Size, uint8_t mode, uint8_t fill);
    /*num最大为±2^31，显示指定位数（len)的整数,size:8/16,返回实际显示了多少位,align为对齐方式，左对齐或右对齐，对齐参数如下：
        LEFT
        RIGHT  */
    uint8_t num(int16_t x, int16_t y, long num, uint8_t len, uint8_t Size, uint8_t align, uint8_t mode);
    // 显示汉字字符串，支持16/24/32大小（hsize），字库请在oledfont.h中自行添加,mode 0:白底黑字/1：黑底白字,fill：1多余自动换行，0多余不显示
    void chinese(int16_t x, int16_t y, const void *str, uint8_t hsize, uint8_t mode, uint8_t fill);
    // 显示汉字字符串，支持16/24/32大小（hsize），字库请在oledfont.h中自行添加,mode 0:白底黑字/1：黑底白字,fill：1多余自动换行，0多余不显示

    // 任意位置画点 mode:0黑点/1白点
     void point(uint8_t x, uint8_t y, uint8_t mode);

    // 指定左上角起点,画任意宽(width)高(height)的实心矩形,可用作画直线, mode:0黑色/1白色
    void line(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t mode);
    // xy为左上角位置,显示指定宽(width)高(height)的BMP位图,XY可以是负数,任意边超出部分自动去除,mode:0反色/1不反色

    void BMP(int16_t x, int16_t y, int16_t width, int16_t height, const void *p, uint8_t mode);

    /**
     * @brief 在Y位置显示BMP位图，X的位置由BMP数组的第一个字节确定，宽高由第2、3个字节确定
     * 
     * @param y Y轴
     * @param p 数组
     */
    void BMP(int16_t y, const void *p);

    /*
        xy为左上角位置,自动识别BMP位图长宽显示（需在取模后数组开头分别添加宽度、高度两个数值）
        XY可以是负数,任意边超出部分自动去除,mode:0反色/1不反色
    */
    void BMP(int16_t x, int16_t y, const void *p, uint8_t mode);
    /*
        xy为左上角位置,自动识别BMP位图长宽显示（需在取模后数组开头分别添加宽度、高度两个数值）
        XY可以是负数,任意边超出部分自动去除
    */
    void BMP(int16_t x, int16_t y, const void *p);
     // 设置自动换行显示行距 默认0
    void row_spacing(uint8_t x);

    // 其余函数
    /*************************************/
    // 初始化驱动总线速度和引脚,具体参数配置请在Oled.h内进行宏定义配置
    void begin();
    // 初始化屏幕
    void init();
    // 清空缓存GRAM（清屏）
    void clr();
    // 设置背光亮度0-255；需屏幕开启状态下可用，复位后需重新调用
    void light(uint8_t light);
    // 刷新屏幕  数据写入到GRAM,必须调用此函数才能更新到屏幕
    void refresh();

    // 显示打开
    void display_on();
    // 显示关闭
    void display_off();
};

extern Oled oled;
#ifdef __cplusplus
}
#endif

#endif