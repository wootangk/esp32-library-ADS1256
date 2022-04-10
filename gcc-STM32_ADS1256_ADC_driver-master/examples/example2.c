/******************************************************************************
 * ������:      example2.c
 * �����:       Celeron (c) 2018
 * ����������:  ����� ����������� ������ (����� ���� �� ��������� �������) - 
                ���������� ����������������� ����������, ���������������� ������ ������ ���...
 ******************************************************************************/

// ������: � ���� ����, ������������ ����� ������ ������� �� ��������� ������������� ���� ���������� (������� �������, ������ � �.�.)
// � �����, ���� ���� ��� ����������� � ������ FreeRTOS.


//-------------------------------------
// ����� "���� ���"

  volatile static TDataRegistrator ADS1256_TEST_OriginalDataRegistrator = 0;
  volatile static uint32_t         ADS1256_TEST_Counter = 0;

  // �������� ��������� ����� � ������� (�������, ����� ��� ������� ����� �������)
  void ADS1256_TEST_DataRegistratorWrapper(const int32_t value)
  {
    ADS1256_TEST_Counter++;
    
    // ���� ��������� "����������� ������", �� ��������� ��� ���������� ������
    if(ADS1256_TEST_OriginalDataRegistrator)
      (*ADS1256_TEST_OriginalDataRegistrator)(value);   
  };


TControlMode Control_MODE_AdcTest(void)
{
  // ��� ����� � ����� ���������� - �������� ������� ������ � �������� ���������
  keyResetStatusForAllButtons();
  
  // ���������� ����� "SDATAC: Stop Read Data Continuous" ���������
  //  (����������: ����� ���������� ���� �������, ��� ����� ������ ��������� � ����������!)
  ADS1256_API_StopDataContinuousModeSynchronous();

  static char s1[25] = {0};
  static char s2[25] = {0};
  static char s3[25] = {0};
  static char s4[25] = {0};

  ADS1256_TEST_Counter = 0;
  ADS1256_TEST_OriginalDataRegistrator = ADS1256_API_GetDataRegistrator();
  ADS1256_API_SetDataRegistrator( ADS1256_TEST_DataRegistratorWrapper );
  
  int32_t value   = 0; 
  int32_t average = 0;
  int32_t max     = INT32_MIN;
  int32_t min     = INT32_MAX;
  uint8_t mode    = 0;

  while(1)
  {
    
    // ����� �.1: ����������� ����� �����
    if(BUTTON_HAVE_FLAG( BUTTON_MENU_PREV, BUTTON_IS_HOLDDOWN ))
    {
      BUTTON_RESET(BUTTON_MENU_PREV);                             // ��������� �������... �������� ������ ������.

      // ����� �����: 
      //  "��������� ����� / ��������� � ���� ���"; 
      //  "��������� ����� / ��������� �������������� � �������� �������"; 
      //  "��������� ����������� / ��������� � ���� ���"
      //  "��������� ����������� / ��������� �������������� � �������� �������"
      mode = (mode+1)%4;

      if(mode&0x2)
      {
        // �������� "���������� ����" � ���������� ���
        ADC_AVG_ResetArray();
        ADS1256_TEST_Counter = 0;
        max = INT32_MIN;
        min = INT32_MAX;
        
        // ������������ ����� "RDATAC: Read Data Continuous"  (������: � ���� ������, ������� API-������� �������� ��� �������� ������! ������, ����� ������� ����� ADS1256_API_StopDataContinuousMode[Synchronous], ��� ��������� ��������� �����������...)
        ADS1256_API_RunDataContinuousMode();
      }
      else
      {
        // ���������� ����� "SDATAC: Stop Read Data Continuous" ���������
        //  (����������: ����� ���������� ���� �������, ��� ����� ������ ��������� � ����������!)
        ADS1256_API_StopDataContinuousModeSynchronous();

        // �������� "���������� ����" � ���������� ���
        ADC_AVG_ResetArray();
        ADS1256_TEST_Counter = 0;
        max = INT32_MIN;
        min = INT32_MAX;
      }
    }

    // ����� �.2: �������� ��������
    if(BUTTON_HAVE_FLAG( BUTTON_MENU_OK, BUTTON_IS_HOLDDOWN ))
    {
      BUTTON_RESET(BUTTON_MENU_OK);                           // ��������� �������... �������� ������ ������.
      
      // �������� "���������� ����" � ���������� ���
      ADC_AVG_ResetArray();
      ADS1256_TEST_Counter = 0;
      max = INT32_MIN;
      min = INT32_MAX;
    }
    
    // ����� �.3: ����� �� �����
    if(BUTTON_HAVE_FLAG( BUTTON_MENU_NEXT, BUTTON_IS_HOLDDOWN ))
    {
      BUTTON_RESET(BUTTON_MENU_NEXT);                                       // ��������� �������... �������� ������ ������.
      ADS1256_API_SetDataRegistrator(ADS1256_TEST_OriginalDataRegistrator); // ������������ ����������� ������, ������� ��� ���������� �� ����� � ����� �����
      return CONTROL_MODE_DEFAULT;                                          // ������� ��������� �� �����, ��-���������
    }
    
    
    // � ������ "���������� �������" ��������� ������� ��������� ����� � �������� ��� ������������
    if(!ADS1256_API_IfDataContinuousMode())
    {
      //value = ADS1256_API_ConvertDataOnce();
      value = ADS1256_API_ReadLastData();
      ADC_AVG_IncludeSample(value);           //������������: ��������� "���������� �����" ����� ����� ������, � ���� ��� (��� ���� ��������). � ��� �����, ������������ ���������� �������������� � �������� ������� (���� ���������).
      ADS1256_TEST_Counter++;
    }
    else
      value = 0;
    
    // �������������� ��������� ����������
    average = ADC_AVG_GetMovingAverage();
    if(average)
    {
      if(max < average)
        max = average;      
      if(min > average)
        min = average;
    }
    ADC_AVG_CalcArrayStatistics();
    
    
    // ��������� �������
    #define _CNV(value)  ((mode&0x1)?ADC_CNV_ConvertCode2Real(value)     :(value))
    #define _CNVD(value) ((mode&0x1)?ADC_CNV_ConvertDeltaCode2Real(value):(value))
    
    if(ADS1256_API_IfDataContinuousMode())
      sprintf(s1, "NOW  _DATAC_ C%6d", ADS1256_TEST_Counter);
    else
      sprintf(s1, "NOW %8d C%6d", _CNV(value),   ADS1256_TEST_Counter);
    
    sprintf(s2, "AVG %8d S%6d", _CNV(average), _CNVD(ADC_AVG_GetArrayStdDev()));
    sprintf(s3, "History Window Avg.D");
    sprintf(s4, "%6d %6d %6d", _CNV(ADC_AVG_GetHistoricalMax()) - _CNV(ADC_AVG_GetHistoricalMin()),  //��������� ������������: _CNVD(ADC_AVG_GetHistoricalMax() - ADC_AVG_GetHistoricalMin())
                               _CNV(ADC_AVG_GetArrayMax())      - _CNV(ADC_AVG_GetArrayMin())     ,
                               _CNV(max)                        - _CNV(min)                       );

    // �������������, ����� "�����" � "����������" �������� ������:
    //sprintf(s3, "H%3d %7d %-7d", _CNVD(ADC_AVG_GetHistoricalMax() - ADC_AVG_GetHistoricalMin()), _CNV(ADC_AVG_GetHistoricalMin()), _CNV(ADC_AVG_GetHistoricalMax()));
    //sprintf(s4, "A%3d %7d %-7d", _CNVD(ADC_AVG_GetArrayMax()      - ADC_AVG_GetArrayMin()),      _CNV(ADC_AVG_GetArrayMin()),      _CNV(ADC_AVG_GetArrayMax()));
    #undef _CNV
    
    
    // �������� �������
    Display_Show_Message( .Line1 = s1,
                          .Line2 = s2,
                          .Line3 = s3,
                          .Line4 = s4 );
    
    // �������� (�������� � ���������� ������)
    osDelay(20);
  }
}


