#include <AD9850.h>
#include <SimpleRotary.h>
#include <LiquidCrystal.h>
#define MODESWITCH 3

const uint8_t rs = 12,en = 13, d4 = 11, d5 = 10, d6 = 9, d7 = 8, W_CLK_PIN = 7,FQ_UD_PIN = 6, DATA_PIN = 5, RESET_PIN = 4;

SimpleRotary rotary(A0, A1, A2);
SimpleRotary rotary1(A3, A4, A5);

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

uint8_t fPosPointer = 0, row = 0;
uint8_t mappedPixelPoint[8] = {9, 8, 7, 6, 5, 4, 3, 2};

uint16_t sweepSteps[12] = {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000};
uint8_t sweepStepsPointer = 0;
uint8_t stepStatus, pushStatus, stepStatus1, pushStatus1;

unsigned sweepMultiplicand = 0;

uint8_t f11=0;
uint8_t f12=0; 
uint8_t f13=0;
uint8_t f14=0;
uint8_t f15=1;
uint8_t f16=0;
uint8_t f17=0;
uint8_t f18=0;

uint8_t f21=0;
uint8_t f22=0;
uint8_t f23=0;
uint8_t f24=0;
uint8_t f25=0;
uint8_t f26=1;
uint8_t f27=0;
uint8_t f28=0;

bool prevMode = false; 

double trimFreq = 124999500;
//-------------------wave class holds all the necessary configurations---------------------//
class wave{
  private:
    uint32_t f1, f2;
    bool normalMode;

  public:
    wave(uint32_t i1, uint32_t i2, bool x){
      f1 = i1;
      f2 = i2;
      normalMode = x;
    }
    void changeMode(){
      if(this->normalMode){
          this->normalMode = false;
      }else{
          this->normalMode = true;
          row = 0;
      }  
    }
    bool getMode(){
      return this->normalMode;  
    }
    void setf1(uint8_t i, uint32_t fIncrease){
      if(i == 1){
        if((f1+fIncrease)>10000000){
            f1 = 10000000;
            f11 = 0;
            f12 = 0;
            f13 = 0;
            f14 = 0;
            f15 = 0;
            f16 = 0;
            f17 = 0;
            f18 = 1; 
        }else{
            f1+=fIncrease;  
        }  
      }else{
        if(f1==10000000){
            f1 = 9999999;
            f11 = 9;
            f12 = 9;
            f13 = 9;
            f14 = 9;
            f15 = 9;
            f16 = 9;
            f17 = 9;
            f18 = 0;  
        }else{
            f1 -= fIncrease;  
        }
      }
    }
    void setf2(uint8_t i, uint32_t fIncrease){
      if(i == 1){
        if((f2+fIncrease)>10000000){
            f2 = 10000000;
            f21 = 0;
            f22 = 0;
            f23 = 0;
            f24 = 0;
            f25 = 0;
            f26 = 0;
            f27 = 0;
            f28 = 1;  
        }else{
            f2+=fIncrease;  
        }  
      }else{
        if(f2==10000000){
            f2 = 9999999;
            f21 = 9;
            f22 = 9;
            f23 = 9;
            f24 = 9;
            f25 = 9;
            f26 = 9;
            f27 = 9;
            f28 = 0;  
        }else{
            f2 -= fIncrease;  
        }
      }
    }
    uint32_t getf1(){
      return this->f1;  
    }
    uint32_t getf2(){
      return this->f2;  
    }
};

//-------------------------creating ine object of wave class------------------------------//
wave sine(10000, 100000, true);

void setup() {
  Serial.begin(115200);
  pinMode(MODESWITCH, INPUT);
  digitalWrite(MODESWITCH, LOW);
  
  rotary.setTrigger(HIGH);
  rotary.setDebounceDelay(2);
  rotary.setErrorDelay(200);
  rotary1.setTrigger(HIGH);
  rotary1.setDebounceDelay(2);
  rotary1.setErrorDelay(200);
  //---------------------DDS initialization--------------------//
  DDS.begin(W_CLK_PIN, FQ_UD_PIN, DATA_PIN, RESET_PIN);
  DDS.calibrate(trimFreq);
  delay(1000);
  //--------------------LCD initialization--------------------//
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Tesla");
  delay(400);
  for(int i=5; i<16; i++){
      lcd.print(".");
      delay(400);
  }
  lcd.clear();
}

void loop() {
  stepStatus = 0;
  pushStatus = 0;
  stepStatus1 = 0;
  pushStatus1 = 0;
  stepStatus = rotary.rotate();
  pushStatus = rotary.push();
  stepStatus1 = rotary1.rotate();
  pushStatus1 = rotary1.push();

  //------------------------rotate handling part-----------------//
  if(pushStatus1 == 1){
    row = row^0x01;
  }
  if(stepStatus1 == 1){
    if(sweepStepsPointer<11){
        sweepStepsPointer++;
    }
  }
  if(stepStatus1 == 2){
    if(sweepStepsPointer>0){
        sweepStepsPointer-=1; 
     }
  }
 
  if(stepStatus == 1){
    if(row == 0){
    if(fPosPointer == 0){
        if((f11+1)<10){
          f11++;
          sine.setf1(1, 1);
        }
     }else if(fPosPointer == 1){
        if((f12+1)<10){
            f12++;
            sine.setf1(1, 10);
        }
     }else if(fPosPointer == 2){
        if((f13+1)<10){
          f13++;
          sine.setf1(1, 100);
        }
     }else if(fPosPointer == 3){
        if((f14+1)<10){
          f14++;
          sine.setf1(1, 1000);
        }
     }else if(fPosPointer == 4){
        if((f15+1)<10){
            f15++;
            sine.setf1(1, 10000);
        }
     }else if(fPosPointer == 5){
        if((f16+1)<10){
            f16++;
            sine.setf1(1, 100000);
        }
     }else if(fPosPointer == 6){
        if((f17+1)<10){
          f17++;
          sine.setf1(1, 1000000);
        }
     }else if(fPosPointer == 7){
        if(f18==0){
          f18++;
          sine.setf1(1, 10000000);
        }
     }
   }else{
     if(fPosPointer == 0){
        if((f21+1)<10){
          f21++;
          sine.setf2(1, 1);
        }
     }else if(fPosPointer == 1){
        if((f22+1)<10){
            f22++;
            sine.setf2(1, 10);
        }
     }else if(fPosPointer == 2){
        if((f23+1)<10){
          f23++;
          sine.setf2(1, 100);
        }
     }else if(fPosPointer == 3){
        if((f24+1)<10){
          f24++;
          sine.setf2(1, 1000);
        }
     }else if(fPosPointer == 4){
        if((f25+1)<10){
            f25++;
            sine.setf2(1, 10000);
        }
     }else if(fPosPointer == 5){
        if((f26+1)<10){
            f26++;
            sine.setf2(1, 100000);
        }
     }else if(fPosPointer == 6){
        if((f27+1)<10){
          f27++;
          sine.setf2(1, 1000000);
        }
     }else if(fPosPointer == 7){
        if(f28==0){
          f28++;
          sine.setf2(1, 10000000);
        }
     } 
   }
  }else if(stepStatus == 2){
    if(row == 0){
     if(fPosPointer == 0){
        if((f11)!=0){
          f11--;
          sine.setf1(2, 1);
        }else if(sine.getf1()==10000000){
          sine.setf1(2, 1);  
        }
     }else if(fPosPointer == 1){
        if((f12)!=0){
            f12--;
            sine.setf1(2, 10);
        }else if(sine.getf1()==10000000){
            sine.setf1(2, 1);
        }
     }else if(fPosPointer == 2){
        if((f13)!=0){
          f13--;
          sine.setf1(2, 100);
        }else if(sine.getf1()==10000000){
          sine.setf1(2, 1);  
        }
     }else if(fPosPointer == 3){
        if((f14)!=0){
          f14--;
          sine.setf1(2, 1000);
        }else if(sine.getf1()==10000000){
          sine.setf1(2, 1);  
        }
     }else if(fPosPointer == 4){
        if((f15)!=0){
            f15--;
            sine.setf1(2, 10000);
        }else if(sine.getf1()==10000000){
            sine.setf1(2, 1);
         }
     }else if(fPosPointer == 5){
        if((f16)!=0){
            f16--;
            sine.setf1(2, 100000);
        }else if(sine.getf1()==10000000){
            sine.setf1(2, 1);  
        }
     }else if(fPosPointer == 6){
        if((f17)!=0){
          f17--;
          sine.setf1(2, 1000000);
        }else if(sine.getf1()==10000000){
          sine.setf1(1, 1);  
        }
     }else if(fPosPointer == 7){
        if(f18==1){
          f18--;
          sine.setf1(2, 10000000);
        }else if(sine.getf1()==10000000){
          sine.setf1(2, 1); 
        }
     }
    }else{
     if(fPosPointer == 0){
        if((f21)!=0){
          f21--;
          sine.setf2(2, 1);
        }else if(sine.getf2()==10000000){
          sine.setf2(2, 1);  
        }
     }else if(fPosPointer == 1){
        if((f22)!=0){
            f22--;
            sine.setf2(2, 10);
        }else if(sine.getf2()==10000000){
            sine.setf2(2, 1);
        }
     }else if(fPosPointer == 2){
        if((f23)!=0){
          f23--;
          sine.setf2(2, 100);
        }else if(sine.getf2()==10000000){
          sine.setf2(2, 1);  
        }
     }else if(fPosPointer == 3){
        if((f24)!=0){
          f24--;
          sine.setf2(2, 1000);
        }else if(sine.getf2()==10000000){
          sine.setf2(2, 1);  
        }
     }else if(fPosPointer == 4){
        if((f25)!=0){
            f25--;
            sine.setf2(2, 10000);
        }else if(sine.getf2()==10000000){
            sine.setf2(2, 1);
         }
     }else if(fPosPointer == 5){
        if((f26)!=0){
            f26--;
            sine.setf2(2, 100000);
        }else if(sine.getf2()==10000000){
            sine.setf2(2, 1);  
        }
     }else if(fPosPointer == 6){
        if((f27)!=0){
          f27--;
          sine.setf2(2, 1000000);
        }else if(sine.getf2()==10000000){
          sine.setf2(1, 1);  
        }
     }else if(fPosPointer == 7){
        if(f28==1){
          f28--;
          sine.setf2(2, 10000000);
        }else if(sine.getf2()==10000000){
          sine.setf2(2, 1); 
        }
     }      
    }
  }if( pushStatus == 1){
     if(fPosPointer < 7){
       fPosPointer++;  
     }else{
        fPosPointer = 0;  
     }
  }
  
  if(sine.getMode()){
    lcd.setCursor(0, 0);
    lcd.print("f=");
    lcd.print(f18);
    lcd.print(f17);
    lcd.print(f16);
    lcd.print(f15);
    lcd.print(f14);
    lcd.print(f13);
    lcd.print(f12);
    lcd.print(f11);
  }else{
    lcd.setCursor(0, 0);
    lcd.print("f=");
    lcd.print(f18);
    lcd.print(f17);
    lcd.print(f16);
    lcd.print(f15);
    lcd.print(f14);
    lcd.print(f13);
    lcd.print(f12);
    lcd.print(f11);
    lcd.setCursor(0, 1);
    lcd.print("f=");
    lcd.print(f28);
    lcd.print(f27);
    lcd.print(f26);
    lcd.print(f25);
    lcd.print(f24);
    lcd.print(f23);
    lcd.print(f22);
    lcd.print(f21);
    lcd.setCursor(11, 0);
    lcd.print("Step");
    lcd.setCursor(11, 1);
    lcd.print(sweepSteps[sweepStepsPointer]);
    lcd.print("   ");
  }
  lcd.setCursor(mappedPixelPoint[fPosPointer], row);
  lcd.cursor();
  delay(10);

  if(sine.getMode()){
      DDS.setfreq(sine.getf1(),0);
  }else{
      DDS.setfreq(sine.getf1()+(sweepSteps[sweepStepsPointer]*sweepMultiplicand), 0);
      sweepMultiplicand++;
      if((sine.getf1()+(sweepSteps[sweepStepsPointer])*sweepMultiplicand)>sine.getf2())sweepMultiplicand=0;
  }

  if(digitalRead(MODESWITCH)^prevMode){
        prevMode = ~prevMode; 
        sine.changeMode();
  }
}
