#include "stm32l1xx.h"
#include "LCD.h"
#include "main.h"
#include "ASD1256.h"
#include "Binary_values_8_bit.h"
#include "Binary_values_16_bit.h"

//ADS1256 pinout
/*
ADC    -    MCU

SCLK   -    PA5 (SPI1_SCK)
DIN    -    PA7 (SPI1_MOSI) 
DOUT   -    PA6 (SPI1_MISO)
DRDY   -    PA4 - MCU input, active low
CS     -    PA2 - MCU output, active low
PDWN   -    PA3 - MCU output, active low
*/

//t_CLKIN = 130,2083333 ns
//SPI period 1 us (baud rate 1 MHz)

//Onboard LEDs
#define LED_BLUE   GPIO_Pin_8
#define LED_GREEN  GPIO_Pin_9

#define AVG_NUM 1
#define ZERO_NUMBER 400

void InitDevice(void);

GPIO_InitTypeDef GPIO_InitStruct;
TIM_TimeBaseInitTypeDef Timer_InitStruct;
SPI_InitTypeDef SPI_InitStruct;
USART_InitTypeDef USART_InitStruct;

int32_t value, val_d, usart_data;
int32_t ADC_offset;
uint16_t ADC_counter = 0;
double temp;
uint16_t rabbish; //SPI and USART unusable data

int main()
{
  InitDevice();
  DelayMS(long_pulse_ms);
  InitLCD();
  InitADS1256();
  DelayMS(2000);
  GPIO_WriteBit(GPIOA, ADC_CS, Bit_RESET);
  WriteRegister(ADC_REG_DRATE,  _5_rate);
  WriteRegister(ADC_REG_MUX, b00001111);//AIN0 is positive, negative is common   
  WriteRegister(ADC_REG_STATUS, b00000010);//Buffer on
  WriteRegister(ADC_REG_ADCON, b00100110);//GAIN = 64
  
  SendSimpleCommand(ADC_SELFCAL);  //Calibrate ADC
  while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE))){}
  rabbish = SPI_I2S_ReceiveData(SPI1);
  while(GPIO_ReadInputDataBit(GPIOA, ADC_DRDY) == Bit_RESET){}
  while(GPIO_ReadInputDataBit(GPIOA, ADC_DRDY) == Bit_SET){}
  
  while(1){
    
    GPIO_WriteBit(GPIOC, LED_BLUE, Bit_SET); 
    
    SendSimpleCommand(ADC_RDATAC);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE))){}
    rabbish = SPI_I2S_ReceiveData(SPI1);
    
    for(uint8_t i = 0; i<10; i++){ //clear the USART buffer for sure
      if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)){
        rabbish = USART_ReceiveData(USART1);
      }
    }
             
    val_d = 0;
    for(uint16_t i = 0; i < AVG_NUM; i++){
      while(GPIO_ReadInputDataBit(GPIOA, ADC_DRDY) == Bit_RESET){}
      while(GPIO_ReadInputDataBit(GPIOA, ADC_DRDY) == Bit_SET){}
      DelayUS(SPI_t6);
      SPI_I2S_SendData(SPI1, (uint16_t)b00000000);
      //while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
      while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE))){}
      value = (uint8_t)SPI_I2S_ReceiveData(SPI1);
      value <<= 8;
      SPI_I2S_SendData(SPI1, (uint16_t)b00000000);
      //while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
      while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE))){}
      value |= (uint8_t)SPI_I2S_ReceiveData(SPI1);
      value <<= 8;
      SPI_I2S_SendData(SPI1, (uint16_t)b00000000);
      //while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
      while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE))){}
      value |= (uint8_t)SPI_I2S_ReceiveData(SPI1);
      if((value & 0x00800000) > 0) value |= 0xff000000;
      val_d += value;
    }     
    
    val_d /= AVG_NUM;    
    GPIO_WriteBit(GPIOC, LED_BLUE, Bit_RESET);
    
    ADC_counter++;
    if(ADC_counter == ZERO_NUMBER){
      ADC_counter = 0;
      ADC_offset = val_d;
    }
    
    val_d -= ADC_offset;
    
    //USART transmition
    usart_data = val_d&0xff000000;
    usart_data >>= 24;
    USART_SendData(USART1, (uint16_t)usart_data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
    
    usart_data = val_d&0x00ff0000;
    usart_data >>= 16;
    USART_SendData(USART1, (uint16_t)usart_data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
    
    usart_data = val_d&0x0000ff00;
    usart_data >>= 8;
    USART_SendData(USART1, (uint16_t)usart_data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
    
    usart_data = val_d&0x000000ff;
    USART_SendData(USART1, (uint16_t)usart_data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
   
    
    temp = (double)val_d; 
    temp *= 0.596046519;    
    val_d = (int32_t)temp;
    ShowValue(&val_d);
    GPIO_WriteBit(GPIOC, LED_GREEN, Bit_SET);
    DelayMS(1);
    GPIO_WriteBit(GPIOC, LED_GREEN, Bit_RESET);
  }
}

void InitDevice(void){
  //ENABLE the GPIO A, GPIO B and GPIO C
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, ENABLE);
  //Initialize the GPIO A outputs
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  //Initialize the GPIO A inputs
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  //Initialize the GPIO A SPI
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  //Initialize the GPIO A USART
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  //Initialize the GPIO B
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
   //Initialize the GPIO C
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
  //ENABLE TIMER 6
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  //Initialize TIMER 6  
  Timer_InitStruct.TIM_Prescaler = 0x0000;
  Timer_InitStruct.TIM_CounterMode = TIM_CounterMode_Down; 
  Timer_InitStruct.TIM_Period = 0xFFFF;
  Timer_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM6, &Timer_InitStruct);
  TIM_Cmd(TIM6, ENABLE);
  //ENABLE SPI1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_SYSCFG, ENABLE);
  //Initialize SPI1
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_CRCPolynomial = 0x0000;
  SPI_Init(SPI1, &SPI_InitStruct);
  SPI_Cmd(SPI1, ENABLE);
  //Alternate functions outputs congiguration for SPI
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
  //ENABLE USART
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_SYSCFG, ENABLE); //RCC_APB2Periph_SYSCFG enabled in SPI1, but let it be dublicated
  //Initialize USART1
  USART_InitStruct.USART_BaudRate = 9600;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);
  USART_Cmd(USART1, ENABLE);
  //Alternate functions outputs congiguration for USART
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
}

void DelayMS(uint16_t t){
  TIM_Cmd(TIM6, DISABLE);
  Timer_InitStruct.TIM_Prescaler = 0x7CFF;
  Timer_InitStruct.TIM_CounterMode = TIM_CounterMode_Down; 
  Timer_InitStruct.TIM_Period = t;
  TIM_SetCounter(TIM6, t);
  Timer_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM6, &Timer_InitStruct);
  TIM_Cmd(TIM6, ENABLE);
  TIM_ClearFlag(TIM6, TIM_FLAG_Update);
  while(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update) == 0){}
}

void DelayUS(uint16_t t){
  TIM_Cmd(TIM6, DISABLE);
  Timer_InitStruct.TIM_Prescaler = 0x001F;
  Timer_InitStruct.TIM_CounterMode = TIM_CounterMode_Down; 
  Timer_InitStruct.TIM_Period = t;
  TIM_SetCounter(TIM6, t);
  Timer_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM6, &Timer_InitStruct);
  TIM_Cmd(TIM6, ENABLE);
  TIM_ClearFlag(TIM6, TIM_FLAG_Update);
  while(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update) == 0){}
}