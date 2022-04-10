#include "stm32l1xx.h"
#include "main.h"
#include "LCD.h"
#include "Binary_values_8_bit.h"
#include "Binary_values_16_bit.h"

extern GPIO_InitTypeDef GPIO_InitStruct;
extern TIM_TimeBaseInitTypeDef Timer_InitStruct;

void InitLCD(void){
  GPIO_WriteBit(GPIOC, LCD_RS, Bit_RESET);
  GPIO_WriteBit(GPIOC, LCD_RW, Bit_RESET);
  GPIO_WriteBit(GPIOC, LCD_E, Bit_RESET);  
  DelayMS(long_pulse_ms);
  
  LCD_WriteCommand(b00100010);  
  
  GPIO_Write(GPIOB, b0000000011000000);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_E, Bit_SET);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_E, Bit_RESET);
  DelayUS(pulse_us);
  
  LCD_WriteCommand(b00001100);
  LCD_WriteCommand(b00000001);
    
  DelayMS(long_pulse_ms);  
  LCD_WriteCommand(b00000110);
  
  LCD_WriteData(b00110000);
  LCD_WriteData(b00110001);
  LCD_WriteData(b00110010);
  LCD_WriteData(b00110011);
  
  LCD_WriteData(b00100000);
  
  LCD_WriteData(b01101000);
  LCD_WriteData(b01110101);
  LCD_WriteData(b01101010);
  LCD_WriteData(b01100001);
  LCD_WriteData(b01101011);
  
  DelayMS(250);
}

void LCD_WriteData(uint8_t c){
  GPIO_WriteBit(GPIOB, LCD_RS, Bit_SET);
  uint16_t cc = c;
  uint16_t ouput_data;
  ouput_data = GPIO_ReadOutputData(GPIOB);
  ouput_data &= 0xFF00;
  cc &= 0x00F0;
  ouput_data |= cc;
  GPIO_Write(GPIOB, ouput_data);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_E, Bit_SET);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_E, Bit_RESET);  
  cc = c;
  cc &= 0x000F;
  cc <<= 4;
  ouput_data = GPIO_ReadOutputData(GPIOB);
  ouput_data &= 0xFF00;
  cc &= 0x00F0;
  ouput_data |= cc;
  GPIO_Write(GPIOB, ouput_data);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_E, Bit_SET);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_E, Bit_RESET);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_RS, Bit_RESET);
  DelayUS(pulse_us);
}

void LCD_WriteCommand(uint8_t d){
  uint16_t dd = d;
  uint16_t ouput_data;
  ouput_data = GPIO_ReadOutputData(GPIOB);
  ouput_data &= 0xFF00;
  dd &= 0x00F0;
  ouput_data |= dd;
  GPIO_Write(GPIOB, ouput_data);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_E, Bit_SET);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_E, Bit_RESET);  
  dd = d;
  dd &= 0x000F;
  dd <<= 4;
  ouput_data = GPIO_ReadOutputData(GPIOB);
  ouput_data &= 0xFF00;
  dd &= 0x00F0;
  ouput_data |= dd;
  GPIO_Write(GPIOB, ouput_data);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_E, Bit_SET);
  DelayUS(pulse_us);
  GPIO_WriteBit(GPIOB, LCD_E, Bit_RESET);
  DelayUS(pulse_us);
}

void ShowValue(int32_t* v){
  uint8_t arr[10];
  int32_t vv;
  uint8_t sign = 0; //0 - positive, 1 - negative
  LCD_WriteCommand(b00000010);//Return home
  DelayMS(10);
  vv = *v;
  if(vv < 0){
    sign = 1;
    vv = -vv;
  } 
  
  for(uint8_t i=0; i<10; i++){
    arr[i] = vv%10;
    vv /= 10;
  }
  
  if(sign){
    LCD_WriteData(b00101101); //-
  }else{
    LCD_WriteData(b00101011); //+
  }
  
  for(int8_t i=6; i>=0; i--){
    LCD_WriteData(b00110000+arr[i]); //a digit 0..9
    if((i%3) == 0) LCD_WriteData(b00100000); //space
  }
  LCD_WriteData(b01110101); //u
  LCD_WriteData(b01010110); //V
}

