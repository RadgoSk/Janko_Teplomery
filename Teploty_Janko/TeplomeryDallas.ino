void CitanieScratchPads(void)
{
  int j,z;
  for(z=0; z<PocetTeplomerov; z++)
    {
     OneWireBus.reset();
     OneWireBus.write(0x55); //Komunik. iba s jednym ROM ID, musi nasl. 64 bit. ROM kod
     
     for(j=0; j<8; j++)
    {
      OneWireBus.write(Teplomery[z].ID[j]);
    }
    
    OneWireBus.write(0xBE);
    for(j=0; j<9; j++) Teplomery[z].ScratchPad[j]=OneWireBus.read();
    }
}

//###########################################################################
void MeranieTeplotyVsetky()       //  funkcia 
{  
  OneWireBus.reset();
  OneWireBus.write(0xCC); //Skip ROM prikaz, pre ovl.všet.zar.(teplomerov na zbernici)
  OneWireBus.write(0x44); //meranie teploty

}

//###########################################################################
void VypocitajTeplotu(void)
{
  unsigned int meas;     //desatinna cast vypocitanej teploty
  signed short cel;      //cela cast vypocitanej teploty 
  unsigned char subzero; //je cislo zaporne alebo kladne? ak 1 potom zaporne, ak 0 tak kladne
  int z;                 //premenna pouzita pri prehladavani jednotlivymi teplomermi
  
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
    }
    else
    subzero = 0;

    meas &= ~(DS18B20_12_BIT_UNDF);
   // do tohto momentu je v meas zdruzenych 2x 8 bitov LSB a MSB 

    cel = 10 * ( (unsigned char)(meas >> 4) );  // ulozi celu cast zmer.teploty a vynasobi x 10
    meas = (unsigned char)(meas & 0x000F);  // get the fractional part

    meas = rounding[meas];

    Teplomery[z].Teplota_v_C = meas + cel;
    
    if(subzero)
        Teplomery[z].Teplota_v_C *= -1;     //multiplied with 10 (-21.6 -> -216)
    
   }
}
