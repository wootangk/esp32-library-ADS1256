#ifndef ADS1256_h
#define ADS1256_h

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

class ADS1256{
    public:
        ADS1256();
        void initADS();
        long readADS(byte channel);
        long readADSDiff(byte positiveCh, byte negativeCh);
    private:
        byte status_reg;
        byte status_data;
        byte adcon_reg;
        byte adcon_data;
        byte drate_reg;
        byte drate_data;
        long adc_val;
        byte data;
};
#endif