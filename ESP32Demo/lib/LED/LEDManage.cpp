#include <LEDManage.h>

// 定义三基色
gpio_num_t LED_R = GPIO_NUM_13;
gpio_num_t LED_G = GPIO_NUM_12;
gpio_num_t LED_B = GPIO_NUM_14;



#pragma region 初始化调用方法

void initGPIO(gpio_num_t r,gpio_num_t g,gpio_num_t b,LEDShowType showType) {
  LED_B=b;
  LED_G=g;
  LED_R=r;
  //为七彩切换时
  if(showType==SevenColorChange){
    // 三基色
    pinMode(LED_R, OUTPUT); 
    pinMode(LED_G, OUTPUT); 
    pinMode(LED_B, OUTPUT); 
  }else{
    //为渐变色时
    // 初始化三个颜色都是不亮，都是低电平
    analogWrite(LED_R, 255);
    analogWrite(LED_G, 255);
    analogWrite(LED_B, 255);
  }
}
#pragma endregion


#pragma region 切换颜色

// 切换颜色
void ChangeColor(int data_color) {
  switch (data_color) {
    case Color_R:  // 红色
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, HIGH);
      break;
    case Color_G:  // 绿色
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, HIGH);
      break;
    case Color_B:  // 蓝色
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, LOW);
      break;
    case Color_RG:  // 黄色
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, HIGH);
      break;
    case Color_RB:  // 紫色
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, LOW);
      break;
    case Color_GB:  // 青色
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, LOW);
      break;
    case Color_RGB:  // 白色
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, LOW);
      break;
    default:
      break;
  }
}
#pragma endregion

#pragma region LED灯渐变色显示
//LED灯渐变色
void LEDGradientColor(){
  int i = 0,PWMValue=0;
	// 红色逐渐变亮
	PWMValue = 255;
	for(i = 0;i<255;i++)
	{
		analogWrite(LED_R, PWMValue--);
		analogWrite(LED_G, 255);
		analogWrite(LED_B, 255);
		delay(10);
	}
	// 红色逐渐变暗
	PWMValue = 0;
	for(i = 0;i<255;i++)
	{
		analogWrite(LED_R, PWMValue++);
		analogWrite(LED_G, 255);
		analogWrite(LED_B, 255);
		delay(10);
	}
	// 绿色逐渐变亮
	PWMValue = 255;
	for(i = 0;i<255;i++)
	{
		analogWrite(LED_R, 255);
		analogWrite(LED_G, PWMValue--);
		analogWrite(LED_B, 255);
		delay(10);
	}
	// 绿色逐渐变暗
	PWMValue = 0;
	for(i = 0;i<255;i++)
	{
		analogWrite(LED_R, 255);
		analogWrite(LED_G, PWMValue++);
		analogWrite(LED_B, 255);
		delay(10);
	}
  // 蓝色逐渐变亮
	PWMValue = 255;
	for(i = 0;i<255;i++)
	{
		analogWrite(LED_R, 255);
		analogWrite(LED_G, 255);
		analogWrite(LED_B, PWMValue--);
		delay(10);
	}
	// 蓝色逐渐变暗
	PWMValue = 0;
	for(i = 0;i<255;i++)
	{
		analogWrite(LED_R, 255);
		analogWrite(LED_G, 255);
		analogWrite(LED_B, PWMValue++);
		delay(10);
	}
}

#pragma endregion



#pragma region LED灯七彩切换
void LEDChangeColor() {
  int color_index=0;
  for(int i=0;i<Color_RGB+1;i++){
    ChangeColor(color_index);
    color_index++;
    //if(color_index>7)
    if(color_index == Color_RGB+1)
    {
      //color_index = 0;
      color_index = Color_R;
    }
    delay(1000);
  }

}
#pragma endregion

