//LCD pinout
//PB7-4 - 4 pin DATA bus
#define LCD_E      GPIO_Pin_15
#define LCD_RW     GPIO_Pin_14
#define LCD_RS     GPIO_Pin_13

//Timing constants
#define pulse_us      100
#define long_pulse_ms 100

void InitLCD(void);
void LCD_WriteData(uint8_t d);
void LCD_WriteCommand(uint8_t c);
void DelayMS(uint16_t t); //milliseconds delay
void DelayUS(uint16_t t); //microseconds delay
void ShowValue(int32_t* v);