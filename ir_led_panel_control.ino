
#include <SoftwareSerial.h>

#define IR_IN  8
#define LED_OUT 4

int Pulse_Width = 0;
int ir_code = 0x00;
char  adrL_code = 0x00;
char  adrH_code = 0x00;
int state = HIGH;
int lastChange = 0;

SoftwareSerial bluetooth(11, 12); // RX, TX
#define BLUETOOTH_SPEED 9600

void timer1_init(void)
{
  TCCR1A = 0X00; 
  TCCR1B = 0X05;
  TCCR1C = 0X00;
  TCNT1 = 0X00;
  TIMSK1 = 0X00;	
}
void remote_deal(void)
{   
    Serial.println(ir_code);
    Serial.println(adrL_code,HEX);
    
    if(ir_code==-256){
      int current = millis();
      if(current - lastChange > 500){
        lastChange = current;
        state = !state;
        Serial.println("Toggle planel state");
        toggleLedPanel();
      }
    }
}
char logic_value()
{
  TCNT1 = 0X00;
  while(!(digitalRead(IR_IN))); 
  Pulse_Width=TCNT1;
  TCNT1=0;
  if(Pulse_Width>=7&&Pulse_Width<=10)
  {
    while(digitalRead(IR_IN));
    Pulse_Width=TCNT1;
    TCNT1=0;
    if(Pulse_Width>=7&&Pulse_Width<=10)
      return 0;
    else if(Pulse_Width>=25&&Pulse_Width<=27) 
      return 1;
  }
  return -1;
}
void pulse_deal()
{
  int i;
  int j;
  ir_code=0x00;
  adrL_code=0x00;
  adrH_code=0x00;

  
  for(i = 0 ; i < 16; i++)
  {
    if(logic_value() == 1) 
        ir_code |= (1<<i);
  }
  
  for(i = 0 ; i < 8; i++)
  {
    if(logic_value() == 1) 
      adrL_code |= (1<<i);
  }
  
  for(j = 0 ; j < 8; j++)
  {
    if(logic_value() == 1) 
        adrH_code |= (1<<j);
  }
}
void remote_decode(void)
{
  TCNT1=0X00;       
  while(digitalRead(IR_IN))
  {
    if(TCNT1>=1563)  
    {
      ir_code=0x00ff;
      adrL_code=0x00;
      adrH_code=0x00;
      return;
    }  
  }

  
  TCNT1=0X00;

  while(!(digitalRead(IR_IN))); 
  Pulse_Width=TCNT1;
  TCNT1=0;
  if(Pulse_Width>=140&&Pulse_Width<=141)
  {

    while(digitalRead(IR_IN));
    Pulse_Width=TCNT1;
    TCNT1=0;
    if(Pulse_Width>=68&&Pulse_Width<=72)//4.5ms
    {  
      pulse_deal();
      return;
    }
    else if(Pulse_Width>=34&&Pulse_Width<=36)//2.25ms
    {
      while(!(digitalRead(IR_IN)));
      Pulse_Width=TCNT1;
      TCNT1=0;
      if(Pulse_Width>=7&&Pulse_Width<=10)//560us
      {
        return; 
      }
    }
  }
}
void toggleLedPanel(){
  digitalWrite(LED_OUT,state);
}
void setup()
{
  Serial.begin(9600);
  bluetooth.begin(BLUETOOTH_SPEED);
  pinMode(IR_IN,INPUT);
  pinMode(LED_OUT,OUTPUT);
  digitalWrite(LED_OUT,HIGH);
  Serial.flush();
}
void bluetoothCheck(){
  if(bluetooth.available()){
    int data = bluetooth.read();
    Serial.println(data);
    if(data == '1'){
     state = LOW;
    }else if(data=='0'){state=HIGH;}
    toggleLedPanel();
  }
}
void checkLuminosity(){
  state = analogRead(1)<120?LOW:HIGH;
  toggleLedPanel();
}
void loop()
{  
  timer1_init();
  while(1)
  {
    remote_decode(); 
    remote_deal();
    bluetoothCheck();
    checkLuminosity();  
  }
  
}



