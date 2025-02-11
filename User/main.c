#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "RP.h"
#include "Motor.h"
#include "Encoder.h"
#include "Serial.h"


uint8_t KeyNum;
int16_t PWM;
int16_t Speed;

float Target,Actual,Out;
float Kp=0.2,Ki=0.2,Kd=0;
float Error0,Error1,ErrorInt;

int main(void)
{
	OLED_Init();
	Key_Init();
	Motor_Init();
	Encoder_Init();
	Timer_Init();
	
	OLED_Printf(0,0,OLED_8X16,"Speed Control");
	OLED_Update();
	
	while (1)
	{
		KeyNum = Key_GetNum();
		if (KeyNum == 1)
		{
			Target += 10;
		}
		if (KeyNum == 2)
		{
			Target -= 10;
		}
		if (KeyNum == 3)
		{
			Target = 0;
		}
		
		OLED_Printf(0, 16, OLED_8X16, "Tar:%+04.0f", Target);
		OLED_Printf(0, 32, OLED_8X16, "Act:%+04.0f", Actual);
		OLED_Printf(0, 48, OLED_8X16, "Out:%+04.0f", Out);
		OLED_Update();
	}
}

void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count;
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		Key_Tick();
		Count ++;
		if (Count >= 40)
		{
			Count = 0;
			
			
			//位置式PID
			Actual =Encoder_Get();
			
			Error1=Error0;
			Error0=Target-Actual;
			
			ErrorInt+=Error0;
			
			Out=Kp * Error0 + Ki * ErrorInt + Kd * (Error0-Error1);
			//输出限幅
			if(Out>100){Out=100;}
			if(Out<-100){Out=-100;}
			
			Motor_SetPWM(Out);
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}



