const int TrigPin = 9;
const int EchoPin = 11;
const int buzzer=5;
int buzzPeriod = 0;
int lastBuzz = 0;
int state = LOW;
float cm;
void setup()
{
  Serial.begin(9600);
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(buzzer,OUTPUT);
}
void buzz(){
  int current = millis();
  Serial.println(current-lastBuzz);
  if(buzzPeriod>0){
    if(current-lastBuzz>=buzzPeriod){
      state = !state;
      lastBuzz = current;
    }
  }
  else{
    state = LOW;
  }
  digitalWrite(buzzer,state);
}
void loop()
{
digitalWrite(TrigPin, LOW); //Low high and low level take a short time to TrigPin pulse
delayMicroseconds(2);
digitalWrite(TrigPin, HIGH);
delayMicroseconds(10);
digitalWrite(TrigPin, LOW);

cm = pulseIn(EchoPin, HIGH) / 58.0; //Echo time conversion into cm
cm = (int(cm * 100.0)) / 100.0; //Keep two decimal places
if(cm < 20){
  buzzPeriod = (cm/40.0)*1000;
  Serial.print(buzzPeriod);
  Serial.println("sec");
}
else
  buzzPeriod = 0;
buzz();
} 
