byte QTIpins[] = {3, 7, 5, 8};
#include <Servo.h> 
Servo servoLeft; 
Servo servoRight;
double setpoint, input;
boolean prevOverHash, overHash = false;
int hashCount = 0;
int points = 0;
int pointLED = 9;
int stopPos = 0;

boolean allOutputs = false;
boolean notFailed = true;
 
void setup()
{
Serial.begin(9600);
Serial.println("Beginning program...");
servoLeft.attach(13);
servoRight.attach(12);
setpoint = 2660;
pinMode(53, OUTPUT);
pinMode(51, OUTPUT);
pinMode(49, OUTPUT);
pinMode(pointLED, OUTPUT);
}
 
void loop()
{
lineFollow();
}
 
void lineFollow(){
input = computeInput();
long error = input-setpoint;
float mappedError = map(error, -500, 500, -200, 200);
mappedError = constrain(mappedError, -200, 200);
int left, right;
if (error < 0){
left = 200+mappedError;
right = 200;
}
else{
right = 200-mappedError;
left = 200;
}
maneuver(left, right);
}
 
void maneuver(int speedLeft, int speedRight)
{
servoLeft.writeMicroseconds(1500 + speedLeft);
servoRight.writeMicroseconds(1500 - speedRight);
delay(40);
}
 
long computeInput(){
long avg = 0;
long sum = 0;
int hashLine = 0;
for (int i = 0; i < 4; i++){
long time = rcTime(QTIpins[i]);
//Serial.println(time);
//delay(500);
avg+=time*(i+1)*1000;
sum+=time;
if (time > 150)
hashLine++;
}
//Serial.println("lol");
//delay(500);
if (hashLine == 4){
overHash = true;

 
}
else
overHash = false;
if (overHash){
hashCount++;
maneuver(0,0);
delay(500);
if(hashCount<=3){
  maneuver(100,100);
  delay(350);
  maneuver(0,0);
  cycleLights();
}
else if(hashCount==4){ // COMMUNICATION CODE GOES HERE
  stopAndReport(points);
//  maneuver(0,0);
//  servoLeft.detach();
//  servoRight.detach();
//  blinker(points);
  delay(3000);
  
//  stopPos = 5-points;
//  maneuver(100,100);
//  delay(200);
//    servoLeft.detach();
//    servoRight.detach();
//    digitalWrite(pointLED, HIGH);
}
else if(hashCount > 4){
  if (stopPos==0){
    servoLeft.detach();
    servoRight.detach();
    digitalWrite(pointLED, HIGH);
  }
  else {
    stopPos--;
    maneuver(0,0);
    delay(300);
    maneuver(200,200);
    delay(300);
    blinker(stopPos);
  }
}
//else if(hashCount == 4 + points){
//  servoLeft.detach();
//  servoRight.detach();
//}
}
prevOverHash = overHash;
return long(avg/sum);
}
 
long rcTime(int sensorIn){
long duration = 0;
pinMode(sensorIn, OUTPUT); 
digitalWrite(sensorIn, HIGH); 
delay(1); 
pinMode(sensorIn, INPUT); 
digitalWrite(sensorIn, LOW);
while(digitalRead(sensorIn)){
duration++;
}
return duration;
}

void cycleLights() {
  if(allOutputs){
      Serial.println("begin");
  } else{
    Serial.println();
  }
  digitalWrite(53, HIGH);
  digitalWrite(49, LOW);
  digitalWrite(51, LOW);
  if(allOutputs){
      Serial.print("Green");
      Serial.println(rcTime(2));
  }
  int greenval = rcTime(2);
  delay(1500);
  digitalWrite(53, LOW);
  digitalWrite(49, HIGH);
  digitalWrite(51, LOW);
  if(allOutputs){
      Serial.print("blue");
      Serial.println(rcTime(2));
  }
  int blueval = rcTime(2);
  delay(1500);
  digitalWrite(51, LOW);
  digitalWrite(49, LOW);
  digitalWrite(51, HIGH);
  if(allOutputs){
      Serial.print("Red");
      Serial.println(rcTime(2));
  }
  int redval = rcTime(2);
  delay(1500);
  if (blueval < 150) {
    points = points + 1; //white
    blinker(1);
    Serial.println("white");
  }
  else if (greenval < 500 && blueval >= 150) {
    points = points + 2;
    blinker(2);
    Serial.println("yellow");
  }
  else if (greenval > 800 && redval > 500) {
    points = points + 3;
    blinker(3);
    Serial.println("green");
  }
  else if (greenval > 800 && redval < 500) {
    points = points;
    Serial.println("red");
  }
  else if(notFailed){
    Serial.println("WTF");
    cycleLights();
    notFailed = false;
    for(int a = 0; a < 5; a++){
     digitalWrite(pointLED, HIGH); 
     delay(75);
     digitalWrite(pointLED, LOW); 
    }
  }
//  if (((abs(redval-31)) < 2) && ((abs(blueval-13)) < 3) && ((abs(greenval-64)) < 10)){
//	points = points + 3;
//blinker(3);
//  Serial.println("The color green"); //green
//  }
//  if (((abs(redval-12)) < 3) && ((abs(blueval-11)) < 4) && ((abs(greenval-47)) < 8)){
//	points = points + 0;
//   Serial.println(" The color red"); //red
//  }
//  if (((abs(redval-10)) < 2) && (abs(blueval-10) < 4) && ((abs(greenval-21)) < 3)){
//	points = points + 2;
//blinker(2);
//   Serial.println("The color is Yellow "); //yellow
//  }
//  if (((abs(redval-10)) < 2) && (abs(blueval-2) < 4) && ((abs(greenval-14)) < 3)){
//	points = points + 1;
//blinker(1);
//   Serial.println(" The color is white"); //white
//  }
}

void blinker (int times){
  for (int k = 0; k < times; k++){
    digitalWrite(pointLED, HIGH);
    delay(300);
    digitalWrite(pointLED, LOW);
    delay(300);
  }
}

void stopAndReport(int blinkNum){
  maneuver(0,0);
  servoLeft.detach();
  servoRight.detach();
  blinker(points);
  delay(500);
  digitalWrite(pointLED, HIGH);
}

void communicate(int posNum){
  
}






