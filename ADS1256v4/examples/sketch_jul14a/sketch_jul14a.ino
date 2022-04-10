#include<ADS1256.h>
ADS1256 ads;
void setup() {
  Serial.begin(9600);
  ads.initADS();
}

void loop() {
  float result1=ads.readADS(0);
  float result2=ads.readADSDiff(0,1);
  result1=result1/1677721.5;    //convert output become real value(2^24==16777215)
  result2=result2/1677721.5; 
  Serial.print(result1,8);
  Serial.print("    ");
  Serial.println(result2,8);
}
