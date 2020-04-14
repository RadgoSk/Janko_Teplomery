#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <OneWire.h>

//Definicia konstant - v anglictine DEFINES[difajns]
#define MAX_POCET_DS18B20   4
#define OWIRE_PIN           8
#define DS18B20_12_BIT_UNDF 0
#define DS18B20_750ms       15  //15*50mstimer = 750ms
#define DS18B20_Interval_1S 20

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);
OneWire OneWireBus(OWIRE_PIN);

//vytvorenie vlastneho datoveho typu(nieco ako standardne INT alebo BYTE), my si vytvarame strukturu ktora obsahuje vsetky INFO o jednom teplomeri
typedef struct {
  byte ID[8];
  byte ScratchPad[9];
  signed short Teplota_v_C;
  }DS18B20;

//vytvorime si pole pre ulozenie informacii pre vsetky teplomery. Cize pole struktur (ukazoval som ti strukturu ulozenia dat v exceli)
DS18B20 Teplomery[MAX_POCET_DS18B20];

//premenna pre pocet najdenych teplomer, pre istotu jej na zaciatku priradime NULU, ale Ccko by malo vsetky premenne nulovat automaticky
int PocetTeplomerov = 0;  // počet ID

<<<<<<< Updated upstream
//deklaracie lokalnych funkcii, kedze kompilator potebuje mat prehlad o pouzitych lokalnych funkciach este pred funkciou MAIN(v arduino pred LOOP)musime mu to povedat.
=======
//SW timer premenne
// 50ms timer
unsigned char MeranieTeplomera_50ms = DS18B20_750ms; // max cas 50ms x 255 = 12750 ms
unsigned char IntervalTeplomera_50ms = DS18B20_Interval_1S; //20 pre 1sekundu

//deklaracie lokalnych funkcii, kedze kompilator potebuje mat prehlad o pouzitych lokalnych
//funkciach este pred funkciou MAIN(v arduino pred LOOP)musime mu to povedat.
>>>>>>> Stashed changes
//Mohli by sme tu dat rovno DEFINICIE tych funkcii aj s telom ale takto je to prehladnejsie
 

//...........................................................................................................
//.................................................SETUP.....................................................
//...........................................................................................................
void setup() 
  {
  //nastavenie HW timerov
  setup_InterruptTimers();
  
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

   
  // ak najde na zbernici ID inkrementuje  "PocetTeplomerov",inak opustí podmienku While 
  // (hodnota "PocetTeplomerov" sa rovná poctu "ID") 
  
  while(OneWireBus.search(Teplomery[PocetTeplomerov].ID))
    {
      PocetTeplomerov++;    
    }
  }
<<<<<<< Updated upstream
//...........................................................................................................
//###################################### HLAVNA SLUCKA (v Ccku funkcia MAIN)#################################
//...........................................................................................................
void loop()
{
  
    MeranieTeplotyVsetky();
    CitanieScratchPads();
    VypocitajTeplotu();
    ZobrazTeplotuVsetky();
    delay(2000);
}

//####################################################################################
//#############################  DEFINICIE LOKALNYCH FUNKCII #########################
//####################################################################################
void ZobrazTeplotuVsetky()
{
  int z;
  char riadok[20];
  char cislo[6];
  
lcd.clear();
=======

//......................................................................................
//------------------------------ INTERUPT TIMERS ---------------------------------------
//...................................................................................... 
>>>>>>> Stashed changes


<<<<<<< Updated upstream
//####################################################################################
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

//####################################################################################
void MeranieTeplotyVsetky()       //  funkcia 
{
  OneWireBus.reset();
  OneWireBus.write(0xCC); //Skip ROM prikaz, pre "ovladanie" vsetkych zariadeni/teplomerov na zbernici
  OneWireBus.write(0x44); //meranie teploty
=======

ISR(TIMER0_COMPA_vect){   //timer0 8bit
>>>>>>> Stashed changes

}

<<<<<<< Updated upstream
//####################################################################################
void ProgressBar(void)
{
  delay(70);
 // lcd.print((char)255);
}

//####################################################################################
void VypocitajTeplotu(void)
{
  unsigned int meas;     //desatinna cast vypocitanej teploty
  signed short cel;      //cela cast vypocitanej teploty 
  unsigned char subzero; //je cislo zaporne alebo kladne? ak 1 potom zaporne, ak 0 tak kladne
  int z;                 //premenna pouzita pri prehladavani jednotlivymi teplomermi
  
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
        
        //ASI TOTO SPOSOBOVALO PROBLEM PRI MINUSIVEJ TEPLOTE
        //meas++;         //2nd complement
=======

// *********** TIMER1 premenne***************


ISR(TIMER1_COMPA_vect){   //timer1 16bit interrupt 20Hz
  if(MeranieTeplomera_50ms > 0){
    MeranieTeplomera_50ms--;
    }
  if(IntervalTeplomera_50ms > 0){
    IntervalTeplomera_50ms--;
>>>>>>> Stashed changes
    }

}
  
ISR(TIMER2_COMPA_vect){   //timer 2 8bit

<<<<<<< Updated upstream
    cel = 10 * ( (unsigned char)(meas >> 4) );  // ulozi celu cast nameranej teploty a vynasobi x 10
    meas = (unsigned char)(meas & 0x000F);  // get the fractional part
=======
}
>>>>>>> Stashed changes

//......................................................................
//############### HLAVNA SLUCKA (v Ccku funkcia MAIN)###################
//......................................................................
void loop()
{

<<<<<<< Updated upstream
    if(subzero)
        Teplomery[z].Teplota_v_C = -cel;     //multiply with 10 (-21.6 -> -216)
    else
        Teplomery[z].Teplota_v_C = cel;      //multiply with 10 (21.6 -> -216)

    Teplomery[z].Teplota_v_C+=meas;
    
   }
=======
    if(IntervalTeplomera_50ms == 0){
      MeranieTeplotyVsetky();
      
      IntervalTeplomera_50ms = DS18B20_Interval_1S;
      MeranieTeplomera_50ms = DS18B20_750ms;
      }
    if(MeranieTeplomera_50ms == 0){
      MeranieTeplomera_50ms = DS18B20_750ms;
      CitanieScratchPads();
      VypocitajTeplotu();
      ZobrazTeplotuVsetky();
    }
>>>>>>> Stashed changes
}
