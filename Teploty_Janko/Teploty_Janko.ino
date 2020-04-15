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

//SW timer premenne
// 50ms timer
unsigned char MeranieTeplomera_50ms = DS18B20_750ms; // max cas 50ms x 255 = 12750 ms
unsigned char IntervalTeplomera_50ms = DS18B20_Interval_1S; //20 pre 1sekundu



//...........................................................................................................
//.................................................SETUP.....................................................
//...........................................................................................................
void setup(){
    
  ZobrazUvodnaObrazovka();
  
  //zistujeme pocet zariadeni na zbernici,
  //treba adekvatne upravit velkost pola Teplomery_ID
  // ak najde na zbernici ID inkrementuje  "PocetTeplomerov",inak opustí podmienku While 
  // (hodnota "PocetTeplomerov" sa rovná poctu "ID") 
  
  while(OneWireBus.search(Teplomery[PocetTeplomerov].ID))
    {
      PocetTeplomerov++;    
    }

  //nastavenie HW timerov
  setup_InterruptTimers();

  MeranieTeplotyVsetky();
  }

//......................................................................................
//------------------------------ INTERUPT TIMERS ---------------------------------------
//...................................................................................... 

ISR(TIMER0_COMPA_vect){   //timer0 8bit
}

ISR(TIMER1_COMPA_vect){   //timer1 16bit interrupt 20Hz
//  if(MeranieTeplomera_50ms > 0){
//    MeranieTeplomera_50ms--;
//    }
  if(IntervalTeplomera_50ms > 0){
    IntervalTeplomera_50ms--;
    }
}
  
ISR(TIMER2_COMPA_vect){   //timer 2 8bit
}


//......................................................................
//############### HLAVNA SLUCKA (v Ccku funkcia MAIN)###################
//......................................................................
void loop()
{
    if(IntervalTeplomera_50ms == 0){
      IntervalTeplomera_50ms = DS18B20_Interval_1S;
      
      CitanieScratchPads();
      VypocitajTeplotu();
      ZobrazTeplotuVsetky();
      MeranieTeplotyVsetky();
      
    }
}
