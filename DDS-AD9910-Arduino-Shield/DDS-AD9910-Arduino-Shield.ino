/* 
 *  
 *  Для любой модуляции нужно сначала вызывать фнукцию calcBestStepRate перед PrepRegistersToSaveWaveForm, 
 *  зачастую это так и сдлеано внутри функций SaveAMWavesToRAM и SaveFMWavesToRAM
 * 
 * 
 */
#include "main.h"

#define FIRMWAREVERSION 2.0 //01.03.2020 Rel.2

#define LOW_FREQ_LIMIT  100000 
#define HIGH_FREQ_LIMIT  490000000 

#define M_ADR 24
#define K_ADR 28
#define H_ADR 32
#define A_ADR 36

#define MOD_INDEX_ADR 40
#define MOD_FREQK_ADR 44
#define MOD_FREQH_ADR 48
#define MOD_AM_DEPTH_ADR 52
#define MOD_FM_DEVK_ADR 56
#define MOD_FM_DEVH_ADR 60

#define MAIN_SETTINGS_FLAG_ADR 100 // defualt settings 
// ADR 101 reserved for clock settings
#define MODULATION_SETTINGS_FLAG 102 // defualt settings

#define INIT_M 100
#define INIT_K 0
#define INIT_H 0
#define INIT_A 0 

#define INIT_MOD_INDEX 0
#define INIT_MFREQ_K 1
#define INIT_MFREQ_H 0
#define INIT_AM_DEPTH 50
#define INIT_FM_DEV_K 3
#define INIT_FM_DEV_H 0


//*********************************

int M, K, H, A, MenuPos;

uint32_t kley;

void setup()
{
  display = Adafruit_SSD1306(128, 64, &Wire);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  DisplayHello();
  delay(2000);
  
  Serial.begin(115200);
  Serial.println(F("DDS AD9910 Arduino Shield by GRA & AFCH. (gra-afch.com)"));
  Serial.print(F("Firmware v.:"));
  Serial.println(FIRMWAREVERSION);

  downButton.Update();

  if (downButton.depressed == true) //если при включении была зажата кнопка DOWN, то затираем управляющие флаги в EEPROM, которые восстановят заводские значения всех параметров
  {
    EEPROM.write(CLOCK_SETTINGS_FLAG_ADR, 255); //flag that force save default clock settings to EEPROM 
    EEPROM.write(MAIN_SETTINGS_FLAG_ADR, 255); //flag that force save default main settings to EEPROM 
    EEPROM.write(MODULATION_SETTINGS_FLAG, 255); //flag that force save default modulation settings to EEPROM 
  }
  
  LoadMain();
  LoadClockSettings();
  LoadModulationSettings();
  MakeOut();

  MenuPos=0;
  modeButton.debounceTime   = 25;   // Debounce timer in ms
  modeButton.multiclickTime = 1;  // Time limit for multi clicks
  modeButton.longClickTime  = 1000; // time until "held-down clicks" register

  upButton.debounceTime   = 75;   // Debounce timer in ms*/
  upButton.multiclickTime = 1;  // Time limit for multi clicks*/
  upButton.longClickTime  = 1000; // time until "held-down clicks" register*/

  downButton.debounceTime   = 70;   // Debounce timer in ms
  downButton.multiclickTime = 1;  // Time limit for multi clicks
  downButton.longClickTime  = 1000; // time until "held-down clicks" register

  UpdateDisplay();
}

  int ModMenuPos=0;
  int ModIndex=0;
  String ModName[3]={"None", "AM", "FM"};
  int MFreqK=0;
  int MFreqH=0;
  int AMDepth=0;
  int FMDevK=0;
  int FMDevH=0;

void loop ()
{

  int functionUpButton=0;
  int functionDownButton=0;

  static int LastUpButtonState=1;
  static int LastDownButtonState=1;
    
  char strBuffer1[10];
 
  while (1)
  {
    LastUpButtonState=upButton.depressed;
    LastDownButtonState=downButton.depressed;
    modeButton.Update();
    upButton.Update();
    downButton.Update();

    if (upButton.clicks != 0) functionUpButton = upButton.clicks;

    if ((functionUpButton == 1 && upButton.depressed == false) ||
        (functionUpButton == -1 && upButton.depressed == true))
    {
      if (MenuPos==0) {if (Check (M+1, K, H)) M=Inc(M);}
      if (MenuPos==1) {if (Check (M, K+1, H)) K=Inc(K);}
      if (MenuPos==2) {if (Check (M, K, H+1)) H=Inc(H);}
      if (MenuPos==3) 
      {
        A=A-1;
        if (A<0) A=0;
      }
      if (MenuPos==4) Modultaion_Menu();
      UpdateDisplay();
    } 
    if (upButton.depressed == false) functionUpButton=0; 
    
    
    if (downButton.clicks != 0) functionDownButton = downButton.clicks;

    if ((functionDownButton == 1 && downButton.depressed == false) ||
        (functionDownButton == -1 && downButton.depressed == true))
    {
      if (MenuPos==0) {if (Check(M-1, K, H)) M=Dec(M);}
      if (MenuPos==1) {if (Check(M, K-1, H)) K=Dec(K);}
      if (MenuPos==2) {if (Check(M, K, H-1)) H=Dec(H);}
      if (MenuPos==3) 
      {
        A=A+1;
        if (A>84) A=84;
      }
      if (MenuPos==4) Modultaion_Menu();
      UpdateDisplay();
    }
    if (downButton.depressed == false) functionDownButton=0;

    if (modeButton.clicks > 0)
    {
      MenuPos++;
      if (MenuPos>4) MenuPos=0;
      UpdateDisplay();
      SaveMain();
    }

    if (modeButton.clicks < 0) DDS_Clock_Config_Menu();

    if ((LastUpButtonState!=upButton.depressed) &&
     (upButton.depressed==false))
     {
      MakeOut();
      SaveMain();
     }

    if ((LastDownButtonState!=downButton.depressed) &&
     (downButton.depressed==false))
     {
      MakeOut();
      SaveMain();
     }
  }
}

/************
 * MakeOut - решает какую из функци генерации выбрать (в зависимости от значения ModIndex)
 ********/
void MakeOut()
{
  switch (ModIndex)
  {
  case 0: //модуляция отключена
    //Freq_Out(M*1000000L + K*1000L + H, A*-1);
    SingleProfileFreqOut(M*1000000L + K*1000L + H, A*-1);
    break;
  case 1: // AM амплитудная модуляция
    SaveAMWavesToRAM(M*1000000L + K*1000L + H, MFreqK*1000L+MFreqH, AMDepth, A*-1);
    break;
  case 2: // FM частотная модуляция
    SaveFMWavesToRAM(M*1000000L + K*1000L + H, MFreqK*1000L+MFreqH, FMDevK*1000L+MFreqH);
    break;
  }
}

void DisplayHello()
{
  display.clearDisplay();
  display.cp437(true); 
  display.setTextSize(2);      
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);
  display.print("Hint:");
  display.setTextSize(1); 
  display.setCursor(62, 0);
  display.print("Firmware");
  display.setCursor(62, 8);
  display.print("ver.: ");
  display.print(FIRMWAREVERSION);
  display.setCursor(0, 16);    

  display.setTextSize(2); 
  display.print("Hold ");
  //display.setTextColor(BLACK, WHITE);
  display.print("MODE");
  display.setTextColor(WHITE);
  display.println(" to enter");
  display.println("setup");
  
  display.display();
}

void UpdateDisplay()
{
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println(F("DDS AD9910"));
  
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 16);     // Start at top-left corner

  display.println(F("Frequency, [Hz]:"));

  display.setTextSize(2);      // Normal 1:1 pixel scale
  if (MenuPos==0) display.setTextColor(BLACK, WHITE); 
    else display.setTextColor(WHITE);
  display.setCursor(1, 26);   

  display.print(PreZero(M));
  if (MenuPos==1) display.setTextColor(BLACK, WHITE); 
    else display.setTextColor(WHITE);
  display.print(PreZero(K));
  if (MenuPos==2) display.setTextColor(BLACK, WHITE); 
    else display.setTextColor(WHITE);
  display.println(PreZero(H));

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 42);     // Start at top-left corner

  display.print(F("Amplitude:"));

  display.print("  Mod:"); //NONE");
  if (MenuPos==4) display.setTextColor(BLACK, WHITE); 
    else display.setTextColor(WHITE);
  display.println(ModName[ModIndex]);

  display.drawLine(68,42, 68,64, WHITE);

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(1, 50);     // Start at top-left corner

  if (MenuPos==3) display.setTextColor(BLACK, WHITE); 
    else display.setTextColor(WHITE);
  if (DACCurrentIndex==0)
  {
    display.print("-");  
    display.print(PreZero(A));
  } else
  {
    if ((-1*A+4)>0) display.print("+");  
      else display.print("-");  
      display.print(PreZero(abs(-1*A+4)));
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(48, 56);
  display.println("dBM");

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); 
  display.setCursor(72, 56);     // Start at top-left corner
  
  display.println(F("GRA&AFCH"));
  display.display();
}

String PreZero(int Digit)
{
  if ((Digit<100) && (Digit>=10)) return "0"+String(Digit);
  if (Digit<10) return "00"+String(Digit);
  return String(Digit);
}

int Inc(int val)
{
  uint32_t FreqVal=M*1000000 + K*1000 + H;
  val++;
  if (val>999) val=999;
  return val;
}

int Dec(int val)
{
 uint32_t FreqVal=M*1000000 + K*1000 + H;
 val--;
 if (val<0) val=0;
 return val;
}


/*****************************************************************
 * 
 * **************************************************************/


bool Check (int _M, int _K, int _H)
{
  long F_Val;
  F_Val = _M*1000000L + _K*1000L + _H;
  if((F_Val >= LOW_FREQ_LIMIT) && (F_Val <= HIGH_FREQ_LIMIT)) return 1;
    else return 0;  
}

  static const unsigned char PROGMEM cos_values[]={22,22,22,21,21,20,20,19,18,17,16,15,13,12,11,10,9,7,6,5,4,3,2,2,1,1,0,0,0,0,0,1,1,2,2,3,4,5,6,7,9,10,11,12,13,15,16,17,18,19,20,20,21,21,22,22};
  static const unsigned char PROGMEM sin_values[]={11,12,13,15,16,17,18,19,20,20,21,21,22,22,22,22,22,21,21,20,20,19,18,17,16,15,13,12,11,10,9,7,6,5,4,3,2,2,1,1,0,0,0,0,0,1,1,2,2,3,4,5,6,7,9,10,11};
  static const unsigned char PROGMEM hifreq[]={22,19,11,3,0,3,10,16,18,14,8,2,0,2,5,8,9,6,3,1,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,1,3,6,9,8,6,2,0,2,8,14,18,16,10,3,0,3,11,19};
  
void displayModulationMenu()
{
  display.clearDisplay();

  if (ModIndex==1) 
  {
    for (int x=0;x<56;x++)
    {
     display.drawPixel(x+35, pgm_read_byte_near(cos_values + x)+40, WHITE);
     display.drawPixel(x+35, pgm_read_byte_near(cos_values + x)+41, WHITE);

      display.drawPixel(x+35, 40+22-pgm_read_byte_near(cos_values + x), WHITE);
      display.drawPixel(x+35, 41+22-pgm_read_byte_near(cos_values + x), WHITE); 

    //display.drawPixel(x+25, pgm_read_byte_near(hifreq + x)+40, WHITE);
   // if (x<55) display.drawLine(x+25, pgm_read_byte_near(hifreq + x)+40, x+25+1, pgm_read_byte_near(hifreq + x+1)+40, WHITE);
    }

    for (int x=0;x<56;x=x+3)
    {
      display.drawLine(x+35, pgm_read_byte_near(cos_values + x)+40, x+35, 40+22-pgm_read_byte_near(cos_values + x), WHITE); 
    }
  }

  if (ModIndex==2)
  {
    for (int x=0;x<56;x++)
    {
      display.drawPixel(x+35, 40+22-pgm_read_byte_near(sin_values + x), WHITE);
      display.drawPixel(x+35, 41+22-pgm_read_byte_near(sin_values + x), WHITE); 
    }

    for (int x=0;x<53;x=x+6-(pgm_read_byte_near(sin_values + x)/6))
    {
      display.drawLine(x+35, 40, x+35+3-(pgm_read_byte_near(sin_values + x)/6), 40+22, WHITE);
      display.drawLine(x+35+3-(pgm_read_byte_near(sin_values + x)/6), 40+22, x+35+6-(pgm_read_byte_near(sin_values + x+1)/6), 40, WHITE);
    }
  }
  
  display.setTextSize(2);     
  display.setTextColor(WHITE); 
  display.setCursor(0,0);
  display.print("Modulation");

  display.setTextSize(1);     
  display.setTextColor(WHITE); 
  display.setCursor(0,16);
  display.print("Type:");
  display.print(" ");
  if (ModMenuPos==0) display.setTextColor(BLACK, WHITE);
  display.print(ModName[ModIndex]);
  display.setTextColor(WHITE);

  if (ModIndex!=0)
  {
    display.setCursor(0,24);
    display.print("MFreq:");
    display.setTextColor(WHITE);
    display.print(" ");
    if (ModMenuPos==1) display.setTextColor(BLACK, WHITE);
    display.print(PreZero(MFreqK));
    display.setTextColor(WHITE);
    if (ModMenuPos==2) display.setTextColor(BLACK, WHITE);
    display.print(PreZero(MFreqH));
    display.setTextColor(WHITE);
    display.print("Hz");

    display.setCursor(0,32);
    if (ModIndex==1) 
    {
      display.print("Depth:");
      display.print(" ");
      if (ModMenuPos==3) display.setTextColor(BLACK, WHITE);
      display.print(AMDepth);
      display.setTextColor(WHITE);
      display.print("%");
    } 
    if (ModIndex==2)
    {
      display.print("Deviation:");
      display.print(" ");
      if (ModMenuPos==3) display.setTextColor(BLACK, WHITE);
      display.print(PreZero(FMDevK));
      display.setTextColor(WHITE);
      if (ModMenuPos==4) display.setTextColor(BLACK, WHITE);
      display.print(PreZero(FMDevH));
      display.setTextColor(WHITE);
      display.print("Hz");
    }
  }

  display.setCursor(0, 55);
  if (ModMenuPos==5) display.setTextColor(BLACK, WHITE);
  display.println("SAVE");

  display.setTextColor(WHITE);
  display.setCursor(103, 55);
  if (ModMenuPos==6) display.setTextColor(BLACK, WHITE);
  display.println("EXIT");

  display.display();
}

void Modultaion_Menu()
{
  int functionUpButton=0;
  int functionDownButton=0;

  static int LastUpButtonState=1;
  static int LastDownButtonState=1;

  upButton.debounceTime=25;
  downButton.debounceTime=25;

  while (1)
  {
    LastUpButtonState=upButton.depressed;
    LastDownButtonState=downButton.depressed;
    
    modeButton.Update();
    upButton.Update();
    downButton.Update();

    if (modeButton.clicks !=0) ModMenuPos++;
    if (ModMenuPos>6) ModMenuPos=0;
    if ((ModIndex==0) && (ModMenuPos==1)) ModMenuPos=5; //jump to save buton if modulation is none and MODE button was pressed
    if ((ModIndex==1) && (ModMenuPos==4)) ModMenuPos=5; //jump to save buton if modulation is AM and in Depth position and MODE button was pressed

    if (upButton.clicks != 0) functionUpButton = upButton.clicks;
    if ((functionUpButton == 1 && upButton.depressed == false) ||
        (functionUpButton == -1 && upButton.depressed == true))
    {
      switch (ModMenuPos)
      {
        case 0: 
          ModIndex++;
          if (ModIndex>2) ModIndex=0;
          break;
        case 1: 
          MFreqK++;
          if (MFreqK>100) MFreqK=0;
          if ((MFreqK==0) && (MFreqH<10)) MFreqK=1;
          break;
        case 2:
          MFreqH++;
          if (MFreqH>999) MFreqH=0;
          if ((MFreqK==0) && (MFreqH<10)) MFreqH=10;
          break;
        case 3: 
          if (ModIndex==1) 
            {
              AMDepth++;
              if (AMDepth>100) AMDepth=0;
            }
          if (ModIndex==2) 
            {
              FMDevK++;
              if (FMDevK>100) FMDevK=0;
            } 
          break;
        case 4:
          if (ModIndex==2) 
            {
              FMDevH++;
              if (FMDevH>999) FMDevH=0;
            }
          break;
        case 5: SaveModulationSettings(); DisplaySaved(); MakeOut(); delay(1000); ModMenuPos=6; break;
        case 6: UpdateDisplay(); LoadModulationSettings(); MakeOut(); return; break;
      }
    }
    if (upButton.depressed == false) functionUpButton=0;
    
    //******DOWN*******************
    if (downButton.clicks != 0) functionDownButton = downButton.clicks;
    if ((functionDownButton == 1 && downButton.depressed == false) ||
        (functionDownButton == -1 && downButton.depressed == true))
    {
      switch (ModMenuPos)
      {
        case 0: 
          ModIndex--;
          if (ModIndex<0) ModIndex=2;
          break;
        case 1: 
          MFreqK--;
          if (MFreqK<0) MFreqK=100;
          if ((MFreqK<1) && (MFreqH<10)) MFreqK=100;
          break;
        case 2:
          MFreqH--;
          if (MFreqH<0) MFreqH=999;
          if ((MFreqK<1) && (MFreqH<10)) MFreqH=999;
          break;
        case 3: 
          if (ModIndex==1) 
            {
              AMDepth--;
              if (AMDepth<0) AMDepth=100;
            }
          if (ModIndex==2) 
            {
              FMDevK--;
              if (FMDevK<0) FMDevK=100;
            } 
          break;
        case 4:
          if (ModIndex==2) 
            {
              FMDevH--;
              if (FMDevH<0) FMDevH=999;
            }
          break;
        case 5: SaveModulationSettings(); DisplaySaved(); MakeOut(); delay(1000); ModMenuPos=6; break;
        case 6: UpdateDisplay(); LoadModulationSettings(); MakeOut(); return; break;
      }
    }
    if (downButton.depressed == false) functionDownButton=0;

    /// Updating DDS when button was released
    if ((LastUpButtonState!=upButton.depressed) &&
     (upButton.depressed==false))
     {
      MakeOut();
     }

    if ((LastDownButtonState!=downButton.depressed) &&
     (downButton.depressed==false))
     {
      MakeOut();
     }
    //***End Updating DDS 
    displayModulationMenu();
  }
}

void SaveModulationSettings()
{
  EEPROM.put(MOD_INDEX_ADR, ModIndex);
  EEPROM.put(MOD_FREQK_ADR,MFreqK);
  EEPROM.put(MOD_FREQH_ADR,MFreqH);
  EEPROM.put(MOD_AM_DEPTH_ADR,AMDepth);
  EEPROM.put(MOD_FM_DEVK_ADR,FMDevK);
  EEPROM.put(MOD_FM_DEVH_ADR,FMDevH);

  EEPROM.write(MODULATION_SETTINGS_FLAG, 55);
}

void LoadModulationSettings()
{
  if (EEPROM.read(MODULATION_SETTINGS_FLAG)!=55)
  {
    ModIndex=INIT_MOD_INDEX;
    MFreqK=INIT_MFREQ_K;
    MFreqH=INIT_MFREQ_H;
    AMDepth=INIT_AM_DEPTH;
    FMDevK=INIT_FM_DEV_K;
    FMDevH=INIT_FM_DEV_H;
    SaveModulationSettings();
  } else
  {
    EEPROM.get(MOD_INDEX_ADR, ModIndex);
    EEPROM.get(MOD_FREQK_ADR, MFreqK);
    EEPROM.get(MOD_FREQH_ADR, MFreqH);
    EEPROM.get(MOD_AM_DEPTH_ADR, AMDepth);
    EEPROM.get(MOD_FM_DEVK_ADR, FMDevK);
    EEPROM.get(MOD_FM_DEVH_ADR, FMDevH);
  }
}

void SaveMain()
{
  EEPROM.put(M_ADR, M);
  EEPROM.put(K_ADR, K);
  EEPROM.put(H_ADR, H);
  EEPROM.put(A_ADR, A);
  EEPROM.write(MAIN_SETTINGS_FLAG_ADR,55);
 }

/**************************************************************************
 * 
 *************************************************************************/
void LoadMain()
{
  if (EEPROM.read(MAIN_SETTINGS_FLAG_ADR)!=55)
  {  
    M=INIT_M;
    K=INIT_K;
    H=INIT_H;
    A=INIT_A;
    SaveMain();
    #if DBG==1
    Serial.println(F("Loading init values"));
    Serial.print("M=");
    Serial.println(M);
    Serial.print("K=");
    Serial.println(K);
    Serial.print("H=");
    Serial.println(H);
    Serial.print("A=");
    Serial.println(A);
    #endif
  }
  else
  {
    EEPROM.get(M_ADR, M);
    EEPROM.get(K_ADR, K);
    EEPROM.get(H_ADR, H);
    EEPROM.get(A_ADR, A);
    #if DBG==1
    Serial.println(F("Value from EEPROM"));
    Serial.print("M=");
    Serial.println(M);
    Serial.print("K=");
    Serial.println(K);
    Serial.print("H=");
    Serial.println(H);
    Serial.print("A=");
    Serial.println(A);
    #endif
  }
}
