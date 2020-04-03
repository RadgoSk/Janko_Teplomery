#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <OneWire.h>

#define MAX_POCET_DS18B20   4
#define OWIRE_PIN           8
#define DS18B20_12_BIT_UNDF 0



LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);
OneWire OneWireBus(OWIRE_PIN);

byte Teplomer_ID[MAX_POCET_DS18B20][8];
byte Scratchpad[MAX_POCET_DS18B20][9];

typedef struct {
  byte ID[8];
  byte ScratchPad[9];
  signed short Teplota_v_C;
  }DS18B20;

DS18B20 Teplomery[MAX_POCET_DS18B20];

int PocetTeplomerov = 0;  // počet ID

//deklaracie lokalnych funkcii
void MeranieTeplotyVsetky(void);
void VypocitajTeplotu(void);
void ZobrazTeplotuVsetky(void);
void CitanieScratchPads(void);
//....................................................
void setup() 
  {
 // lcd.setBacklight(LOW); //nema efekt
  lcd.begin(20,4);

  lcd.setCursor(0,0);
  lcd.print("Trapenie Teplomery");
  lcd.setCursor(0,1);
  lcd.print("Shield 2020/1");
  lcd.setCursor(0,2);
  lcd.print("Verzia 1.01");
  lcd.setCursor(0,3);
  lcd.print("21.03.2020");
  delay (2000);
  lcd.clear();

  //zistujeme pocet zariadeni na zbernici,
  //treba adekvatne upravit velkost pola Teplomery_ID

  int i = 0;  // počet riadkov 
  int j = 0;  // počítadlo bajtov
  
  // ak najde na zbernici ID inkrementuje  "PocetTeplomerov",inak opustí podmienku While 
  // (hodnota "PocetTeplomerov" sa rovná poctu "ID") 
  
  while(OneWireBus.search(Teplomery[PocetTeplomerov].ID))
    {
      PocetTeplomerov++;    
    }
  }
//....................................................
void loop()
{
  
  if(PocetTeplomerov!=0){
    MeranieTeplotyVsetky();
    CitanieScratchPads();
    VypocitajTeplotu();
    ZobrazTeplotuVsetky();
    delay(2000);
  }
  else{
    
    lcd.setCursor(0,1);
    lcd.print("Devices not found");
    };
}
//.............................................................
void ZobrazTeplotuVsetky()
{
  int z,j;
  char riadok[20];
  char cislo[6];
  
lcd.clear();

  for(z=0; z<PocetTeplomerov;z++)
  {
    dtostrf((float)Teplomery[z].Teplota_v_C/10,6,1,cislo);
    
    lcd.setCursor(0,z);
    sprintf(riadok,"Teplomer %d:%s", z+1,cislo);
    lcd.print(riadok);
  }
}
//.........................................................
void CitanieScratchPads(void)
{
  int j,z;
  for(z=0; z<PocetTeplomerov; z++)
    {
     OneWireBus.reset();
     OneWireBus.write(0x55); //Komunikacia iba s jednym ROM ID, musi nasledovat 64 bitovy ROM kod
     
     for(j=0; j<8; j++)
    {
      OneWireBus.write(Teplomery[z].ID[j]);
    }
    
    OneWireBus.write(0xBE);
    for(j=0; j<9; j++) Teplomery[z].ScratchPad[j]=OneWireBus.read();
    }
}

//**************************************************************************  
void MeranieTeplotyVsetky()       //  funkcia 
{
  OneWireBus.reset();
  OneWireBus.write(0xCC); //Skip ROM prikaz, pre "ovladanie" vsetkych zariadeni/teplomerov na zbernici
  OneWireBus.write(0x44); //meranie teploty

  for(int i=0;i<12;i++)
    {
    ProgressBar();
    }
      OneWireBus.reset();
}
//****************************************************************************
  void ProgressBar(void)
{
  delay(70);
 // lcd.print((char)255);
}
//****************************************************************************
void VypocitajTeplotu(void)
{
  unsigned int meas;   //desatinna cast
  signed short cel;    //cela cast desatinneho cisla 
  unsigned char subzero;
  int z;
  
  unsigned int hodnota_teplomera;
  int v_help;
  int desatina_pom;
  float desatina;
  int cele;
  int rounding[] = {
        0,
        1,              //0.0625*1 after rounding = 0.1
        1,              //0.125 after rounding = 0.1
        2,              //0.1875 after rounding = 0.2
        3,              //0.25 after rounding = 0.
        3,              //0.3125 after rounding = 0.3
        4,              //0.375 after rounding = 0.4 ...
        4, 5, 6, 6, 7, 8, 8, 9, 9
    };

   for(z=0; z<PocetTeplomerov; z++)
   {

   //conversion SP1 a SP2 to integer
    meas = Teplomery[z].ScratchPad[0];
    meas |= ((unsigned int)Teplomery[z].ScratchPad[1]) << 8;

    //check for negative temperature
    if( meas & 0x8000)
    {
        subzero = 1;    //priznak pre zaporne cislo
        meas = ~meas;   //0xffff; //convert to positive => complement++
        meas++;         //2nd complement
    }
    else
    subzero = 0;

    meas &= ~(DS18B20_12_BIT_UNDF);
   // do tohto momentu je v meas zdruzenych 2x 8 bitov LSB a MSB 

    cel = 10 * ( (unsigned char)(meas >> 4) );  // ulozi celu cast nameranej teploty a vynasobi x 10
    meas = (unsigned char)(meas & 0x000F);  // get the fractional part

    meas = rounding[meas];

    if(subzero)
        Teplomery[z].Teplota_v_C = -cel;     //multiply with 10 (-21.6 -> -216)
    else
        Teplomery[z].Teplota_v_C = cel;      //multiply with 10 (21.6 -> -216)

    Teplomery[z].Teplota_v_C+=meas;
    
   }
}
