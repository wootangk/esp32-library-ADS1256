#include "stm32l1xx.h"
#include "ASD1256.h"
#include "Binary_values_8_bit.h"
#include "Binary_values_16_bit.h"
#include "main.h"

uint8_t reg_val = 0;

void InitADS1256(void){
    GPIO_WriteBit(GPIOA, PDWN, Bit_SET);    
}

void WriteRegister(uint8_t Reg, uint8_t Val){
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
    SPI_I2S_SendData(SPI1, ((uint16_t)b01010000)|Reg);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
    SPI_I2S_SendData(SPI1, (uint16_t)b00000000);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
    SPI_I2S_SendData(SPI1, (uint16_t)Val);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
}

void SendSimpleCommand(uint16_t comm){
    SPI_I2S_SendData(SPI1, comm);
}

uint8_t ReadRegister(uint8_t Reg){
    uint8_t reg_val;
    SPI_I2S_SendData(SPI1, ((uint16_t)b00010000)|Reg);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
    SPI_I2S_SendData(SPI1, (uint16_t)b00000001);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
    DelayUS(SPI_t6);
    SPI_I2S_SendData(SPI1, (uint16_t)b00000000);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
    reg_val = SPI_I2S_ReceiveData(SPI1);
    SPI_I2S_SendData(SPI1, (uint16_t)b00000000);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
    reg_val = SPI_I2S_ReceiveData(SPI1);
    return(reg_val);
}

void ReadADC_Value(uint8_t* buf){
    SPI_I2S_SendData(SPI1, (uint16_t)b00000000);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
    buf[0] = (uint8_t)SPI_I2S_ReceiveData(SPI1);
    SPI_I2S_SendData(SPI1, (uint16_t)b00000000);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
    buf[1] = (uint8_t)SPI_I2S_ReceiveData(SPI1);
    SPI_I2S_SendData(SPI1, (uint16_t)b00000000);
    while(!(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE))){}
    buf[2] = (uint8_t)SPI_I2S_ReceiveData(SPI1);
}