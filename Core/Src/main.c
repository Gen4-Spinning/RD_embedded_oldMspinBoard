/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Structs.h"
#include "Initialize.h"
#include "CommonConstants.h"
#include "HMI_Fns.h"
#include "HMI_Constants.h"
#include "StateFns.h"
#include "logicDefines.h"
#include "functionDefines.h"
#include "eeprom.h"
#include "VFD.h"
#include "Control.h"
#include "../../Drivers/Encoder/encoder.h"
#include "machineControl.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c3;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim9;
TIM_HandleTypeDef htim13;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
struct State S;
struct Uart U;
struct FlyerSettings fsp; 
struct UpdateFlyerSettings ufsp;
struct Diagnostics D;

encoders encs;
ringDoubler rd;
machineSettingsTypeDef ms;
// HMI STRUCTS
struct HMI_InfoBase hsb;
struct HMI_RunPacket hrp;
struct HMI_StopPacket hsp;
struct HMI_DiagPacket hdp;
VFD vfd;


struct Motor M[6];
struct Error E;

char BufferRec [MAX_PACKET_SIZE_FROM_HMI];//BUFFER  FOR RECEIVE
const char* startBufferRec = BufferRec; // Pointer to start of Buffer Rec
char BufferTransmit[130] ;// Buffer for Transmit

/*char MOTORARRAY[8] = {0,RD_SPINDLE,RD_FRONTROLLER_FRONT,RD_BACKROLLER_FRONT,RD_FRONTROLLER_BACK,RD_BACKROLLER_BACK,RD_LIFT_FRONT,RD_LIFT_BACK};
char MOTORARRAY_HMI[8] = {0,HMI_RF_SPINDLE,,HMI_RF_BACKROLLER_FRONT,HMI_RD_FRONTROLLER_BACK,HMI_RD_BACKROLLER_BACK,HMI_RD_LIFT_FRONT,HMI_RD_LIFT_BACK};
 */

//Order of names in Array have to be same as order of motors(motor1,motor2..etc) //TO CHECK ORDER WITH SIVA
char MOTORARRAY[6] = {0,RD_FRONTROLLER_RIGHT,RD_FRONTROLLER_LEFT,RD_SPINDLE,RD_LIFT_FRONT,RD_LIFT_BACK};
char MOTORARRAY_HMI[6] = {0,HMI_RD_FRONTROLLER_RIGHT,HMI_RD_FRONTROLLER_LEFT,HMI_RD_SPINDLE,HMI_RD_LIFT_RIGHT,HMI_RD_LIFT_LEFT};

//Logic Variables
int keyPress = 0;
int allMotorsOn = 2;
extern int Cap1,Cap2,Cap3;
int stopSecondaryMotor = 0;
int restartMachine = 0;
int startFlag = 0;
int pulseCount = 0;
int pulseCount1 = 0; // Lift front
int pulseCount2 = 0; // Lift Back
int pulseCount3 = 0; // Lift front
int pulseCount4 = 0; // Lift Back
int bobbinBed_dir = 1;

//variables for checking bed flatness
extern int finalCountBedTimer;
extern int leftMotorSlow;
extern int rightMotorSlow;

//machine States
int homeFlag = 0; // flag to indicate to interrupt to switch on only 2 motors.


//main running variables
int currentLayer = 0;
int rampRpm = 5;
//flag for switching on REceive buffer properly
char firstTimeRXOn = 0;

//diag variables - these are defined in the diagnostic state File
extern unsigned int idxMotor_diag;
extern long diag_pwm;
extern int testMode;

//offset variable for motors
int flyer_DC_Offset = 100;
int bobbin_DC_Offset = 100;
int backRoller_DC_Offset = 100;
int frontRoller_DC_Offset = 100;
int rightMotor_DC_Offset = 100;
int leftMotor_DC_Offset = 100;
int filter1;
//production variables

float totalProduction_dt = 0.02;
float totalProduction = 0.1;
int doffPercent = 0;

int RunLeft = 0;
int RunRight = 0;
//homing variables
extern int rightMotorOn;
extern int leftMotorOn;
extern char homing_Normal_StateFront;
extern char homing_Normal_StateBack;

extern unsigned int windingLength1;
extern unsigned int bindingLength1;
extern unsigned int windingPps1;
extern unsigned int bindingPps1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C3_Init(void);
static void MX_UART4_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM9_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM13_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void Winding_RIGHT(int step,int length);
void Binding_RIGHT(int step,int length);
void Winding_LEFT(int step,int length);
void Binding_LEFT(int step,int length);
void Homing_RIGHT(int step,int length);
void Homing_LEFT(int step,int length);
void ApplyPwms(void);
void AllSignalVoltageLow(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int prescaler = 0;
int directionChange = 0;
int directionChange1 = 0;
unsigned int chaseCount = 0;
int actualSpindleSpeed = 0;

extern int DoffOverRightHomingReached ;
extern int DoffOverLeftHomingReached ;
extern int doffOver;

uint8_t setVFD = 0;
uint8_t spindleSpeed_in = 0;
uint8_t startVFD = 0;
uint8_t stopVFD = 0;
uint16_t tim8=0;

void Winding_RIGHT(int step,int length){ // for RIGHT SIDE
	if(step >= 1){prescaler = 840000/step;}
	__HAL_TIM_SET_PRESCALER(&htim5,prescaler);
	HAL_GPIO_WritePin(GPIOA,STP1_DIR_Pin,GPIO_PIN_SET); //direction UP
	HAL_TIM_PWM_Start_IT(&htim5,TIM_CHANNEL_4);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4,50);
	
	if(pulseCount1 >= length ){
		pulseCount1 = 0;
		directionChange = directionChange * -1;
		chaseCount++;
	}
}

void Binding_RIGHT(int step,int length){ // for RIGHT SIDE
	if(step >= 1){prescaler = 840000/step;}
	__HAL_TIM_SET_PRESCALER(&htim5,prescaler);
	HAL_GPIO_WritePin(GPIOA,STP1_DIR_Pin,GPIO_PIN_RESET); //direction down
	HAL_TIM_PWM_Start_IT(&htim5,TIM_CHANNEL_4);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4,50);
	
	if(pulseCount1 >= length){
		pulseCount1 = 0;
		directionChange = directionChange * -1;
		chaseCount++;
	}
}

void Winding_LEFT(int step,int length){
	if(step >= 1){prescaler = 840000/step;}
	__HAL_TIM_SET_PRESCALER(&htim13,prescaler);
	HAL_GPIO_WritePin(STP2_DIR_NEW_GPIO_Port,STP2_DIR_NEW_Pin,GPIO_PIN_SET); //direction UP
	HAL_TIM_PWM_Start_IT(&htim13,TIM_CHANNEL_1);
	__HAL_TIM_SetCompare(&htim13, TIM_CHANNEL_1,50);
	
	if(pulseCount2 >= length ){
		pulseCount2 = 0;
		directionChange1 = directionChange1 * -1;
	}
}

void Binding_LEFT(int step,int length){
	if(step >= 1){prescaler = 840000/step;}
	__HAL_TIM_SET_PRESCALER(&htim13,prescaler);
	HAL_GPIO_WritePin(STP2_DIR_NEW_GPIO_Port,STP2_DIR_NEW_Pin,GPIO_PIN_RESET); //direction down
	HAL_TIM_PWM_Start_IT(&htim13,TIM_CHANNEL_1);
	__HAL_TIM_SetCompare(&htim13, TIM_CHANNEL_1,50);
	
	if(pulseCount2 >= length){
		pulseCount2 = 0;
		directionChange1 = directionChange1 * -1;
	}
}

void Homing_RIGHT(int step,int length)
{
	prescaler = 840000/step;
	__HAL_TIM_SET_PRESCALER(&htim5,prescaler);
	HAL_TIM_PWM_Start_IT(&htim5,TIM_CHANNEL_4);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4,50);
	
}

void Homing_LEFT(int step,int length)
{
	prescaler = 840000/step;
	__HAL_TIM_SET_PRESCALER(&htim13,prescaler);
	HAL_TIM_PWM_Start_IT(&htim13,TIM_CHANNEL_1);
	__HAL_TIM_SetCompare(&htim13, TIM_CHANNEL_1,50);
	
}

void LiftTest(int step,int length,char liftMotor,int liftDirection)
{
	prescaler = 840000/step;

	if (liftMotor == HMI_RD_LIFT_RIGHT){
		__HAL_TIM_SET_PRESCALER(&htim5,prescaler);
		if (liftDirection == LIFT_UP){
			HAL_GPIO_WritePin(GPIOA,STP1_DIR_Pin,GPIO_PIN_SET); //direction UP
		}
		if (liftDirection == LIFT_DOWN){
			HAL_GPIO_WritePin(GPIOA,STP1_DIR_Pin,GPIO_PIN_RESET); //direction DOWN
		}
		
		HAL_TIM_PWM_Start_IT(&htim5,TIM_CHANNEL_4);
		__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4,50);
		
		if(pulseCount1 >= (length * 100)){
			HAL_TIM_PWM_Stop_IT(&htim5,TIM_CHANNEL_4);
			pulseCount1 = 0;
			D.liftOver = 1;
			}
		}
	
	//changed from tim9 CH1 to Tim14 Ch1
	if (liftMotor == HMI_RD_LIFT_LEFT){
		__HAL_TIM_SET_PRESCALER(&htim13,prescaler);
		if (liftDirection == LIFT_UP){
			HAL_GPIO_WritePin(STP2_DIR_NEW_GPIO_Port,STP2_DIR_NEW_Pin,GPIO_PIN_SET); //direction UP
		}
		if (liftDirection == LIFT_DOWN){
			HAL_GPIO_WritePin(STP2_DIR_NEW_GPIO_Port,STP2_DIR_NEW_Pin,GPIO_PIN_RESET); //direction DOWN
		}
		
		HAL_TIM_PWM_Start_IT(&htim13,TIM_CHANNEL_1);
		__HAL_TIM_SetCompare(&htim13, TIM_CHANNEL_1,50);
		
		if(pulseCount2 >= (length * 100)){
			HAL_TIM_PWM_Stop_IT(&htim13,TIM_CHANNEL_1);
			pulseCount2 = 0;
			D.liftOver = 1;
		}
	}
}
	
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){

	if (htim->Instance==TIM5){
		pulseCount1++;
	}
	
	if (htim->Instance==TIM13){
		pulseCount2++;
	}
	
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)  {
			// A 1 sec TIMER
	if (htim->Instance==TIM7){ //check if the interrupt comes from TIM7
		LedToggle(LED2);
		U.TXtransfer = 1;
		S.oneSecTimer += 1;
	}

	//50ms timer for running control loop.
	if (htim->Instance==TIM8){
		tim8++;
		if(testMode == 1){
			UpdateRPM(&encs);
			if (D.typeofTest == HMI_DIAG_CLOSED_LOOP){
				followTargets(idxMotor_diag);
				ApplyPwms();
			}
		}
		if (allMotorsOn == 1){
			UpdateRPM(&encs);
			followTargets(MOTOR1);
			followTargets(MOTOR2);
			ApplyPwms();
		}
	}

	//100ms timer
	// 3 main control things happens here -> Test Mode, Homing and main Running
	if (htim->Instance==TIM6){
	   if (testMode == 1){
		   if (D.liftMode == 1){
			   LiftTest(100,D.liftDistance,D.liftMotor,D.liftDirection);
		   }
		   else{
			   MotorDrive(ENABLE_D);
			   if (D.typeofTest == HMI_DIAG_CLOSED_LOOP){
				   updateTargets(idxMotor_diag,1);
			   }
			   if (D.typeofTest == HMI_DIAG_OPEN_LOOP){
				   UpdateMotorVoltage(idxMotor_diag,diag_pwm); // inside this rpm goes into MotorIndex
				   ApplyPwms();
			   }
		   }
	   }
	
	   if (homeFlag == 1){
		   if (RunRight == 1){
			   if (homing_Normal_StateFront != OFF){
				   Homing_RIGHT(100,300);
			   }
		   }

		   if (RunLeft == 1){
			   if (homing_Normal_StateBack != OFF){ // for back side
				   Homing_LEFT(100,300);
			   }
		   }
	   }
				
	   if(allMotorsOn == 1){
		   LedToggle(LED3);
		   actualSpindleSpeed = (int)(M[MOTOR1].presentRpm * 10.92)/3.42;//CHECK
		   if(startFlag == 1){
			   calculateSpindleMotorSpeed(&vfd,1);
			   ContinuousRDCalculations(&ms,&rd);
			   M[MOTOR1].intTarget = rd.calenderMotorRPM;
			   M[MOTOR2].intTarget = rd.calenderMotorRPM;
		   }

		   if(startFlag == 0){
			   calculateSpindleMotorSpeed(&vfd,0);
			   ContinuousRDCalculations(&ms,&rd);
			   M[MOTOR1].intTarget = rd.calenderMotorRPM;
			   M[MOTOR2].intTarget = rd.calenderMotorRPM;

				if((vfd.presentRPM <= 100) && (stopSecondaryMotor == 1))
				{
					ResetSecondaryMotor(); // reset all motor struct variables (Kp,Ki,Kd,err,pwm)
					ResetEncoderVariables(&encs); //cap1,filter,rpm
					M[MOTOR1].pwm = 0;
					M[MOTOR1].setRpm = 0;
					M[MOTOR2].setRpm = 0;
					M[MOTOR2].pwm = 0;
					allMotorsOn = 0;
					S.keyState = ENABLE_KEYPRESS;
					MotorDrive(DISABLE_D);
					AllSignalVoltageLow();
				}
		}

		if (RunRight == 1){
			if (DoffOverRightHomingReached == 1){
				__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4,1);
				HAL_TIM_PWM_Stop_IT(&htim5,TIM_CHANNEL_4);
			} else{
				if(directionChange == 1){
					if(rd.bindingPps >=1){
					Binding_RIGHT(rd.bindingPps,rd.bindingStrokePps);
					}
				}
				if(directionChange == -1){
					if(rd.windingPps >1){
					Winding_RIGHT(rd.windingPps,rd.windingStrokePps);
					}
				}
			}
		}

		// for the back side
		if (RunLeft == 1){
			if (DoffOverLeftHomingReached == 1){
				__HAL_TIM_SetCompare(&htim13, TIM_CHANNEL_1,1);
				HAL_TIM_PWM_Stop_IT(&htim13,TIM_CHANNEL_1);
			}else{
				if(directionChange1 == 1){
					if(rd.bindingPps >=1){
					Binding_LEFT(rd.bindingPps,rd.bindingStrokePps);
					}
				}
				if(directionChange1 == -1){
					if(rd.windingPps >1){
						Winding_LEFT(rd.windingPps,rd.windingStrokePps);
					}
				}
			}
		}

	   }

		if (allMotorsOn == 0){
			//turn off the lifts
			__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4,1);
			HAL_TIM_PWM_Stop_IT(&htim5,TIM_CHANNEL_4);
			__HAL_TIM_SetCompare(&htim13, TIM_CHANNEL_1,1);
			HAL_TIM_PWM_Stop_IT(&htim13,TIM_CHANNEL_1);
		}

	} //closes tim6
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_I2C3_Init();
  MX_UART4_Init();
  MX_TIM5_Init();
  MX_TIM9_Init();
  MX_TIM8_Init();
  MX_TIM13_Init();
  /* USER CODE BEGIN 2 */

  /*Tim8 is 50 ms interrupt for running the control loop
  Tim6 is the 100ms interrupt to do setting target*/

  InitializeStateStruct(); // sets state to idle and paired =0
  InitializeDiagnosticsStruct();
  InitializeUartStruct();
  InitializeFlyerSettings();
  ReadRFSettingsFromEeprom(); //UpdateFlyerFrameSettings from EEPROM
  RunRight = fsp.rightSideOn;
  RunLeft = fsp.leftSideOn;
  InitializeUpdateFlyerSettings(); // just a struct to holdtheupdated values from the APP before we update the eeprom
  ReadOldDoffSettingsFromEeprom();

  //Setup the ms parameters
  ms.inputYarnCountNe = 20;
  ms.outputYarnDia = 0.40;
  ms.packageHeight = 200;
  ms.spindleSpeed= 8000;
  ms.diaBuildFactor = 0.12;
  ms.tpi = 14;
  ms.windingClosenessFactor = 108;
  ms.windingOffsetCoils = 2;

  VFD_setSpindleSpeed(&vfd, SPINDLE_SPEED_8000); //set this based on settings
  SetupRDCalculations(&ms,&rd);
	
	//Initialize the HMI Structs
  Create_HMI_BasePacket();
  Create_HMI_Run_Packet();
  Create_HMI_StopPacket();
  Create_HMI_DiagPacket();
			
  //Interrupts on HMI UART- UART1
  __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);//interrupt on receive buffer not empty(buffer full)
  HAL_UART_Receive_IT(&huart1,(uint8_t *)BufferRec,MAX_PACKET_SIZE_FROM_HMI);	 // start the receive here.
 // __HAL_UART_ENABLE_IT(&huart1,UART_IT_TXE );//interrupt on Transmit  buffer empty
		
  HAL_TIM_Base_Start_IT(&htim6); // MAIN TIMER FOR MOTOR CONTROL PWM - 100ms
  HAL_TIM_Base_Start_IT(&htim8); // MAIN TIMER FOR control loop - 50ms

  // Switch on all the timers for the PWM , and set them low immediately
  AllTimerOn();
  AllSignalVoltageLow();
	
  //Init the Motor Struct
  MotorStructInit();
	
  	//STEPPER MOTORS
	//HAL_GPIO_WritePin(GPIOA,STP1_EN_Pin,GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(GPIOE,STP2_EN_Pin,GPIO_PIN_RESET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {	
		

	 /* if (setVFD){
		  VFD_setSpindleSpeed(&vfd, spindleSpeed_in);
		  setVFD = 0;
	  }

	  if (startVFD){
		  VFD_startInductionMotor(&vfd);
		  startVFD = 0;
	  }

	  if (stopVFD){
		  VFD_stopInductionMotor(&vfd);
		  stopVFD = 0;
	  }*/

  	if (S.current_state == IDLE_STATE){
		IdleState();
	}
		
	if (S.current_state == HOMING_STATE){
		HomingState();
	}
		
	if (S.current_state == RUN_STATE){
		RunState();
	}
		
	if (S.current_state == HALT_STATE){
		HaltState();
	}

	if (S.current_state == PAUSE_STATE){
		PauseState();
	}
		
	if (S.current_state == UPDATESETTINGS){
		SettingsState();
	}
		
	if (S.current_state == DIAGNOSTIC_STATE){
		DiagnosticsState();
	}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 4;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 3000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 4;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 3000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 839;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 100;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */
  HAL_TIM_MspPostInit(&htim5);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 9999;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 839;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 9999;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 8400;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 9999;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 419;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 839;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 99;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */
  HAL_TIM_MspPostInit(&htim9);

}

/**
  * @brief TIM13 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM13_Init(void)
{

  /* USER CODE BEGIN TIM13_Init 0 */

  /* USER CODE END TIM13_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM13_Init 1 */

  /* USER CODE END TIM13_Init 1 */
  htim13.Instance = TIM13;
  htim13.Init.Prescaler = 839;
  htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim13.Init.Period = 99;
  htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim13, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM13_Init 2 */

  /* USER CODE END TIM13_Init 2 */
  HAL_TIM_MspPostInit(&htim13);

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, STP1_EN_Pin|STP1_DIR_Pin|STP2_DIR_NEW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, STP2_EN_Pin|STP2_DIR_Pin|OUT6_Pin|OUT5_Pin
                          |OUT4_Pin|OUT3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TowerGreen_Pin|TowerRed_Pin|TowerAmber_Pin|RelayJ15_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RelayJ17_Pin|RelayJ18_Pin|Led1_Pin|Led2_Pin
                          |Led3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Cap3_Pin Cap4_Pin Cap5_Pin Cap6_Pin
                           Cap7_Pin Cap1_Pin Cap2_Pin */
  GPIO_InitStruct.Pin = Cap3_Pin|Cap4_Pin|Cap5_Pin|Cap6_Pin
                          |Cap7_Pin|Cap1_Pin|Cap2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : Cap8_Pin Cap9_Pin Cap10_Pin */
  GPIO_InitStruct.Pin = Cap8_Pin|Cap9_Pin|Cap10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : STP1_EN_Pin STP1_DIR_Pin STP2_DIR_NEW_Pin */
  GPIO_InitStruct.Pin = STP1_EN_Pin|STP1_DIR_Pin|STP2_DIR_NEW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : STP2_EN_Pin STP2_DIR_Pin OUT6_Pin OUT5_Pin
                           OUT4_Pin OUT3_Pin */
  GPIO_InitStruct.Pin = STP2_EN_Pin|STP2_DIR_Pin|OUT6_Pin|OUT5_Pin
                          |OUT4_Pin|OUT3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : TowerGreen_Pin TowerRed_Pin TowerAmber_Pin RelayJ15_Pin */
  GPIO_InitStruct.Pin = TowerGreen_Pin|TowerRed_Pin|TowerAmber_Pin|RelayJ15_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : RelayJ17_Pin RelayJ18_Pin Led1_Pin Led2_Pin
                           Led3_Pin */
  GPIO_InitStruct.Pin = RelayJ17_Pin|RelayJ18_Pin|Led1_Pin|Led2_Pin
                          |Led3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : Sig5V_Pin Sig12V_Pin Inp3_Pin Inp4_Pin
                           Inp5_Pin Inp_Pin Inp7_Pin Inp8_Pin */
  GPIO_InitStruct.Pin = Sig5V_Pin|Sig12V_Pin|Inp3_Pin|Inp4_Pin
                          |Inp5_Pin|Inp_Pin|Inp7_Pin|Inp8_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : Sig48V_Pin Inp2_Pin */
  GPIO_InitStruct.Pin = Sig48V_Pin|Inp2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : Inp1_Pin */
  GPIO_InitStruct.Pin = Inp1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Inp1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Inp9_Pin */
  GPIO_InitStruct.Pin = Inp9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Inp9_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void TimerLow(char index)
{
	switch(index)
	{
		case T2_CH4:
			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4,0);
		break;
		
		case T2_CH3:
			__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3,0);
		break;
		
		/*case T1_CH4:
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4,0);
		break;
		
		case T1_CH3:
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3,0);
		break;
		
		case T1_CH2:
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_2,0);
		break;
		
		case T1_CH1:
			__HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1,0);
		break;
		

		case T3_CH4:
			__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4,0);
		break;*/
	}
}

uint16_t PWMLow = 0;

void AllSignalVoltageLow(void){
	PWMLow++;
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4,1);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3,1);
}

void ApplyPwms(void){
 if ((M[MOTOR1].pwm <= MAX_PWM ) && (M[MOTOR1].pwm >= MIN_PWM)){
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_4,M[MOTOR1].pwm);
 }
	
 if ((M[MOTOR2].pwm <= MAX_PWM ) && (M[MOTOR2].pwm >= MIN_PWM)){
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3,M[MOTOR2].pwm);
 }
	
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
