#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include <MeAuriga.h>

MeGyro gyro(1,0x69); 

//Encoder Motor
MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);

int moveSpeed = 0;

void isr_process_encoder1(void)
{
  if(digitalRead(Encoder_1.getPortB()) == 0)
  {
    Encoder_1.pulsePosMinus();
  }
  else
  {
    Encoder_1.pulsePosPlus();;
  }
}
void isr_process_encoder2(void)
{
  if(digitalRead(Encoder_2.getPortB()) == 0)
  {
    Encoder_2.pulsePosMinus();
  }
  else
  {
    Encoder_2.pulsePosPlus();
  }
}

void move(int direction, int speed)
{
      int leftSpeed = 0;
      int rightSpeed = 0;
      if(direction == 1){
            leftSpeed = -speed;
            rightSpeed = speed;
      }else if(direction == 2){
            leftSpeed = speed;
            rightSpeed = -speed;
      }else if(direction == 3){
            leftSpeed = -speed;
            rightSpeed = -speed;
      }else if(direction == 4){
            leftSpeed = speed;
            rightSpeed = speed;
      }
      Encoder_1.setTarPWM(leftSpeed);
      Encoder_2.setTarPWM(rightSpeed);
}


float c1[4] = {0, 0, 0, 0};
float c2[4] = {0, 0, 0, 0};
float e1[4] = {0, 0 ,0, 0};
float e2[4] = {0, 0, 0, 0};




void setup() {
  //Set Pwm 8KHz
    TCCR1A = _BV(WGM10);
    TCCR1B = _BV(CS11) | _BV(WGM12);
    TCCR2A = _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS21);
    attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
    attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
    Serial.begin(9600);
        gyro.begin();

}


void Forward(void)
{
  Encoder_1.setMotorPwm(-moveSpeed);
  Encoder_2.setMotorPwm(moveSpeed);
}

void Backward(void)
{
  Encoder_1.setMotorPwm(moveSpeed);
  Encoder_2.setMotorPwm(-moveSpeed);
}
float ref = 0;
float com = 0;

void loop(){
    //move(1,255);

      float fork = gyro.getAngle(1);
      float theta = gyro.getAngle(3);
      gyro.fast_update();
  
      e1[0] = ref - fork;
      e2[0] = com - Encoder_1.getCurrentSpeed();
  //MOVE + BALANCE
  //moveSpeed = 3*(c1[0] + c2[0]);

  //BALANCE
   moveSpeed = 5*(c1[0] );
  

  c1[0] = c1[1]*0.2406 + c1[2] * 0.9367 - c1[3] * 0.3038 + e1[0]*1.109 - e1[1]*1.843 +e1[2] * 1.267 - e1[3]*0.4807;
  //c1[0] = c1[1]*2.757 + c1[2] * 2.528 - c1[3] * 0.7712 + e1[0]*26.48 -e1[1]*76.46 +e1[2] * 73.83 - e1[3]*23.85;
  c2[0] = c2[1]*1.873 - c2[2]*0.8731 + e2[0]*0.2534 + e2[1]*0.003418 - e2[2]*0.02192;
   
  for(int i = 3; i>0; i--)
  { 
    c1[i] = c1[i-1];
    e1[i] = e1[i-1];
    e2[i] = e2[i-1];
  }

  if(c1[0]<-255) c1[0] = -255;
  if(c1[0]>255) c1[0] = 255;
    if(c2[0]<-255) c2[0] = -255;
  if(c2[0]>255) c2[0] = 255;
    if(moveSpeed<-255) moveSpeed = -255;
  if(moveSpeed>255) moveSpeed= 255;

  Serial.println(moveSpeed);
  move(1,moveSpeed);
    //Encoder_1.setMotorPwm(-moveSpeed);
    //Encoder_2.setMotorPwm(moveSpeed);
    Encoder_1.loop();
  Encoder_2.loop();
}
