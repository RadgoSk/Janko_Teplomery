void ZobrazTeplotuVsetky()
{
  int z;
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
  //zmena
}

void ZobrazUvodnaObrazovka(){
  lcd.begin(20,4);
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("Trapenie Teplomery");
  lcd.setCursor(0,2);
  lcd.print("Verzia 2.00");
  lcd.setCursor(0,3);
  lcd.print("14.04.2020");
  delay(2000);
  
}
