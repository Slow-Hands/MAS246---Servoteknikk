#ifndef DISPLAY_H                       
#define DISPLAY_H                       

#include <Arduino.h>                    
#include <LiquidCrystal.h>              

class Display {                         
  public:                               
    Display();                          
    void begin();                       
    void vis(int etasje, int pil);      

  private:                              
    LiquidCrystal _lcd;                 
    int _forrigeEtasje;                 
    int _forrigePil;                    
};                                      

#endif                                  