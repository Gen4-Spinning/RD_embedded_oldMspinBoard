
#include "main.h"
#include "functionDefines.h"
#include "stm32f4xx_hal.h"
#include "logicDefines.h"


char InputVoltageSense(void)
{
	char signalOut = 0;
	if (HAL_GPIO_ReadPin(GPIOD,Sig5V_Pin) == 1)
	{
		HAL_Delay(5);
		if (HAL_GPIO_ReadPin(GPIOD,Sig5V_Pin) == 1)
		{
			signalOut = 1;
		}
	}
	else if(HAL_GPIO_ReadPin(GPIOD,Sig12V_Pin) == 1)
	{
		HAL_Delay(5);
		if (HAL_GPIO_ReadPin(GPIOD,Sig12V_Pin) == 1)
		{
			signalOut = 1;
		}
	}
	else if(HAL_GPIO_ReadPin(GPIOC,Sig48V_Pin) == 1)
	{
		HAL_Delay(5);
		if (HAL_GPIO_ReadPin(GPIOC,Sig48V_Pin) == 1)
		{
			signalOut = 1;
		}
	}
	else 
		signalOut = 0;
	return signalOut;
}

char InputSensor1(void)
{
	char sensorOut = 0;
	if (HAL_GPIO_ReadPin(GPIOB,Inp9_Pin) == 0){   // connected to J24 Homming sensor{
		HAL_Delay(250);
		if (HAL_GPIO_ReadPin(GPIOB,Inp9_Pin) == 0){
			sensorOut = 1;
		}
	}
	return sensorOut;
}

char InputSensor2(void)
{
	char sensorOut = 0;
	if (HAL_GPIO_ReadPin(GPIOD,Inp8_Pin) == 0){
		HAL_Delay(250);
		if (HAL_GPIO_ReadPin(GPIOD,Inp8_Pin) == 0){
			sensorOut = 1;
		}
	}
	return sensorOut;
}


char InputSensor3(void)
{
	char sensorOut = 0;
	if (HAL_GPIO_ReadPin(GPIOD,Inp3_Pin) == 0)
	{
		HAL_Delay(5);
		if (HAL_GPIO_ReadPin(GPIOD,Inp3_Pin) == 0)
		{
			sensorOut = 1;
		}
	}
	return sensorOut;
}

char Pushbutton(void)
{
	char key = 0;
	if (HAL_GPIO_ReadPin(GPIOA,Inp1_Pin) == 0)  //temp changed to inp4
	{
		HAL_Delay(100);
		if (HAL_GPIO_ReadPin(GPIOA,Inp1_Pin) == 0)
			{
				key = 1;
				
			}
	}
	return key;
}



char LeftCalib(void)
{
	char sensorOut = 0;
	if (HAL_GPIO_ReadPin(GPIOD,Inp3_Pin) == 1)   // Left side calibration sensor connected to J25
	{
		HAL_Delay(200);
		if (HAL_GPIO_ReadPin(GPIOD,Inp3_Pin) == 1)
		{
			sensorOut = 1;
		}
	}
	return sensorOut;
}

char RightCalib(void)
{
	char sensorOut = 0;
	if (HAL_GPIO_ReadPin(GPIOD,Inp5_Pin) == 1)   // Right side calibration sensor connected to J27
	{
		HAL_Delay(200);
		if (HAL_GPIO_ReadPin(GPIOD,Inp5_Pin) == 1)
		{
			sensorOut = 1;
		}
	}
	return sensorOut;
}



void LedOn(char index)
{
	switch(index)
	{
		case LED1:
			HAL_GPIO_WritePin(GPIOD,Led1_Pin,GPIO_PIN_SET);		
		break;
			
		case LED2:
			HAL_GPIO_WritePin(GPIOD,Led2_Pin,GPIO_PIN_SET);	
		break;
		
		case LED3:
			HAL_GPIO_WritePin(GPIOD,Led3_Pin,GPIO_PIN_SET);	
		break;
	}
}

void LedOff(char index)
{
	switch(index)
	{
		case LED1:
			HAL_GPIO_WritePin(GPIOD,Led1_Pin,GPIO_PIN_RESET);		
		break;
			
		case LED2:
			HAL_GPIO_WritePin(GPIOD,Led2_Pin,GPIO_PIN_RESET);	
		break;
		
		case LED3:
			HAL_GPIO_WritePin(GPIOD,Led3_Pin,GPIO_PIN_RESET);	
		break;
	}
}

void LedToggle(char index)
{
	switch(index)
	{
		case LED1:
			HAL_GPIO_TogglePin(GPIOD,Led1_Pin);	
		break;
			
		case LED2:
			HAL_GPIO_TogglePin(GPIOD,Led2_Pin);
		break;
		
		case LED3:
			HAL_GPIO_TogglePin(GPIOD,Led3_Pin);
		break;
	}
}


void TowerLamp(char index)
{
	switch(index)
	{
		case GREEN_OFF:
			HAL_GPIO_WritePin(GPIOB,TowerGreen_Pin,GPIO_PIN_SET);
		break;
		
		case GREEN_ON:
			HAL_GPIO_WritePin(GPIOB,TowerGreen_Pin,GPIO_PIN_RESET);
		break;
		
		case RED_OFF:
			HAL_GPIO_WritePin(GPIOB,TowerRed_Pin,GPIO_PIN_SET);
		break;
		
		case RED_ON:
			HAL_GPIO_WritePin(GPIOB,TowerRed_Pin,GPIO_PIN_RESET);
		break;
		
		case AMBER_OFF:
			HAL_GPIO_WritePin(GPIOB,TowerAmber_Pin,GPIO_PIN_SET);
		break;
		
		case AMBER_ON:
			HAL_GPIO_WritePin(GPIOB,TowerAmber_Pin,GPIO_PIN_RESET);
		break;
		
	}
}


void MotorDrive(char index) // Corrected - Enable is enable, disable is disable by switching the names. Rest are same
{
	switch(index)
	{
		case DISABLE_D:
			HAL_GPIO_WritePin(GPIOB,RelayJ15_Pin,GPIO_PIN_SET);
		break;
		
		case ENABLE_D:
			HAL_GPIO_WritePin(GPIOB,RelayJ15_Pin,GPIO_PIN_RESET);
		break;
		
		case BRAKE_ON_D:
			HAL_GPIO_WritePin(GPIOD,RelayJ17_Pin,GPIO_PIN_RESET);
		break;
		
		case BRAKE_OFF_D:
			HAL_GPIO_WritePin(GPIOD,RelayJ17_Pin,GPIO_PIN_SET);
		break;
		
		case REVERSE_D:
			HAL_GPIO_WritePin(GPIOD,RelayJ18_Pin,GPIO_PIN_SET);
		break;
		
		case FORWARD_D:
			HAL_GPIO_WritePin(GPIOD,RelayJ18_Pin,GPIO_PIN_RESET);
		break;
		
	}
}











