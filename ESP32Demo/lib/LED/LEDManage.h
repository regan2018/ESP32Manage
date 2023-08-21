#include <Arduino.h>

// 红、绿、蓝、黄、紫、青、白
enum LEDColor{Color_R,Color_G,Color_B,Color_RG,Color_RB,Color_GB,Color_RGB};

//七彩切换、渐变色
enum LEDShowType{ SevenColorChange, GradientChange};

// 切换颜色
void ChangeColor(int data_color);

//LED灯渐变色
void LEDGradientColor();


//初始化调用方法
void initGPIO(gpio_num_t r,gpio_num_t g,gpio_num_t b,LEDShowType showType);

//LED灯七彩切换
void LEDChangeColor();

