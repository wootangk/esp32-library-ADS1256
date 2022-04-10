#define ADC_DRDY   GPIO_Pin_4
#define ADC_CS     GPIO_Pin_2
#define PDWN      GPIO_Pin_3

//ADC commands
#define ADC_WAKEUP_0 b0000000000000000
#define ADC_RDATA    b0000000000000001
#define ADC_RDATAC   b0000000000000011
#define ADC_SDATAC   b0000000000001111
#define ADC_RREG     b0000000000010000
#define ADC_WREG     b0000000001010000
#define ADC_SELFCAL  b0000000011110000
#define ADC_SELFOCAL b0000000011110001
#define ADC_SELFGCAL b0000000011110010
#define ADC_SYSOCAL  b0000000011110011
#define ADC_SYSGCAL  b0000000011110100
#define ADC_SYNC     b0000000011111100
#define ADC_STANDBY  b0000000011111101
#define ADC_RESET    b0000000011111110
#define ADC_WAKEUP_1 b0000000011111111
//ADC registers
#define ADC_REG_STATUS 0x00
#define ADC_REG_MUX    0x01
#define ADC_REG_ADCON  0x02
#define ADC_REG_DRATE  0x03
#define ADC_REG_IO     0x04
#define ADC_REG_OFC0   0x05
#define ADC_REG_OFC1   0x06
#define ADC_REG_OFC2   0x07
#define ADC_REG_FSC0   0x08
#define ADC_REG_FSC1   0x09
#define ADC_REG_FSC2   0x0A
//ADC rates
#define _30000_rate    b11110000   
#define _15000_rate    b11100000
#define _7500_rate     b11010000
#define _3750_rate     b11000000
#define _2000_rate     b10110000
#define _1000_rate     b10100001
#define _500_rate      b10010010
#define _100_rate      b10000010
#define _60_rate       b01110010
#define _50_rate       b01100011
#define _30_rate       b01010011
#define _25_rate       b01000011
#define _15_rate       b00110011
#define _10_rate       b00100011
#define _5_rate        b00010011
#define _2_5_rate      b00000011


#define SPI_t3 1
#define SPI_t6 10
#define SPI_t16 2


void InitADS1256(void);
void WriteRegister(uint8_t Reg, uint8_t Val);
void SendSimpleCommand(uint16_t comm);
uint8_t ReadRegister(uint8_t Reg);
void ReadADC_Value(uint8_t* buf);