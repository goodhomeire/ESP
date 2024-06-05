#include "EC11.h"

Skey ec11;

static esp_timer_handle_t key_tic;


static int8_t io_sw = -1;
static int8_t io_sa = -1;
static int8_t io_sb = -1;

static uint16_t key_time = 10000;
static uint16_t double_time = 10000;
static uint8_t key_scan_buf = 0;
static uint8_t last_buf = 0x03;

static uint8_t speed_up_feel_num = 0;
static uint16_t speed_up_count = 0;

static uint8_t fist_flg = 1;

void Skey::key_mode_read()
{
    if(fist_flg == 1)
    {
        fist_flg = 0;
        key_scan_buf = 0;
        return;
    }
    if (key_scan_buf < 20)
    {

        if (speed_up_flg)
            speed_up_feel_num = speed_up_feel_count;
        ec11.ec_buf = -1 - speed_up_count;
        _take_type_ec_sw = ec11_task_is_encode;
        task();
    }
    else
    {
        if (speed_up_flg)
            speed_up_feel_num = speed_up_feel_count;
        ec11.ec_buf = 1 + speed_up_count;
        _take_type_ec_sw = ec11_task_is_encode;
        task();
    }
    key_scan_buf = 0;
}

void key_timer(void *arg)
{

    if (ec11.state_count < 255)
    {
        ec11.state_count++;
        if (ec11.state_count == shake_t)
            ec11.state = ec11.state_buf;
    }
    if (!ec11.state && key_time < 10000)
    {
        key_time++;
        if (key_time == key_time_out)
        {
            ec11.sw_buf = sw_long;
            ec11._take_type_ec_sw = ec11_task_is_key;
            ec11.task();
        }
    }

    if (double_time < 10000)
    {
        double_time++;
        if (double_time == ec11.double_click_count)
        {
            double_time = 10000;
            ec11.sw_buf = sw_click;
            ec11._take_type_ec_sw = ec11_task_is_key;
            ec11.task();
        }
    }

    if (ec11.state)
    {
        if (key_time > 5 && key_time < key_time_out)
        {
            if (ec11.sw_buf == sw_clr && double_time > key_double_out)
            {
                double_time = 0;
            }
            else if (double_time < key_double_out)
            {
                double_time = 10000;
                ec11.sw_buf = sw_double;
                ec11._take_type_ec_sw = ec11_task_is_key;
                ec11.task();
            }
        }
        key_time = 0;
    }

    if (speed_up_feel_num)
    {
        speed_up_feel_num--;
        if (speed_up_count < (ec11.speed_up_max_num - 1))
            speed_up_count++;
    }
    else
    {
        speed_up_count = 0;
    }
}

IRAM_ATTR void io_sw_int(void *arg)
{
    ec11.state_buf = gpio_get_level((gpio_num_t)io_sw)<<1;
    ec11.state_count = 0;
}

IRAM_ATTR void io_sa_sb_int(void *arg)
{
    uint8_t tmp = gpio_get_level((gpio_num_t)io_sb) << 1;
    tmp |= gpio_get_level((gpio_num_t)io_sa);

    if (tmp != 3)
    {
        if (tmp == 0)
        {
            key_scan_buf &= 0x3f;
        }
        else
        {
            if (last_buf != tmp)
            {
                last_buf = tmp;
                if (key_scan_buf & 0xc0)
                {
                    key_scan_buf |= tmp;
                }
                else
                {
                    key_scan_buf |= tmp << 4;
                    ec11.key_mode_read();
                    key_scan_buf = 0xc0;
                }
            }
        }
    }
}

void Skey::detach()
{
    esp_timer_stop(key_tic);
    work_flg = 0;
    int_close();
    attch_p = NULL;
}
void Skey::task()
{
    if (attch_p == NULL)
        return;
    if (_take_type_ec_sw == ec11_task_is_key)
        attch_p(_take_type_ec_sw, sw_buf);
    else
        attch_p(_take_type_ec_sw, ec_buf);
    sw_buf = sw_clr;
    ec_buf = 0;
}

void Skey::int_work()
{
    if (work_flg)
    {
        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_ANYEDGE;
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pin_bit_mask = (1ULL << io_sw);
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        gpio_config(&io_conf);
        gpio_install_isr_service(0);
        gpio_isr_handler_add((gpio_num_t)io_sw, io_sw_int, NULL);

        io_conf.pin_bit_mask = (1ULL << io_sa);
        gpio_config(&io_conf);
        gpio_isr_handler_add((gpio_num_t)io_sa, io_sa_sb_int, NULL);

        io_conf.pin_bit_mask = (1ULL << io_sb);
        gpio_config(&io_conf);
        gpio_isr_handler_add((gpio_num_t)io_sb, io_sa_sb_int, NULL);
        
        
    }
}

void Skey::int_close()
{
    gpio_isr_handler_remove((gpio_num_t)io_sw);
    gpio_isr_handler_remove((gpio_num_t)io_sa);
    gpio_isr_handler_remove((gpio_num_t)io_sb);
    
}

static void timer_init(void) {
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &key_timer,
        .arg = NULL,
        .name = "periodic_timer"
    };

    // 创建定时器
    esp_timer_create(&periodic_timer_args, &key_tic);

    // 启动定时器，每100毫秒调用一次回调函数
    esp_timer_start_periodic(key_tic, 1000);  // 100000微秒 = 100毫秒
}

void Skey::begin(uint8_t sw, uint8_t sa, uint8_t sb ,void (*func)(ec11_task_result_type task_type, int16_t rusult_value))
{
    io_sw = sw;
    io_sa = sa;
    io_sb = sb;
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << sw);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = (1ULL << sa);
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = (1ULL << sb);
    gpio_config(&io_conf);

    attch_p = func;
    work_flg = 1;
    key_time = 10000;
    double_time = 10000;
    key_scan_buf = 0;
    last_buf = 0;
    if (io_sw != -1)
        io_conf.pin_bit_mask = (1ULL << io_sw);
        gpio_config(&io_conf);
    if (io_sa != -1)
        io_conf.pin_bit_mask = (1ULL << io_sa);
        gpio_config(&io_conf);
    if (io_sb != -1)
        io_conf.pin_bit_mask = (1ULL << io_sb);
        gpio_config(&io_conf);
    int_work();
    timer_init();
}
void Skey::speed_up(bool val)
{
    speed_up_flg = val;
}

void Skey::speed_up_max(uint16_t val)
{
    speed_up_max_num = val;
}

void Skey::speed_up_feel(uint8_t val)
{
    speed_up_feel_count = val;
}

void Skey::double_click(bool val)
{
    if (val)
        double_click_count = key_double_out;
    else
        double_click_count = 1;
}

ec11_key_result_type Skey::get_key_result()
{
    return sw_buf;
}

int16_t Skey::get_encode_value()
{
    return ec_buf;
}
