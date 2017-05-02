git remote add origin https://github.com/doominos/Individual-Project-Arduino-Code.git
git push -u origin master

#include<NewPing.h>// Calls the NewPing library for the UltraSonic Sensor
#include <VarSpeedServo.h> //Used to control the servo motors.
int light_f = 15; //A1
int light_l = 16; //A2
int light_r = 17; //A3
int light_b = 18; //A4
int button = 11; //D11
int fillerL = 3; //D3
int fillerR = 2; //D2
int LED = 10;
NewPing sonar(12,12,21); //Set the D12 as the pin used for ther ultrasonic sensor 
                          // and the maximum distance is set as 21 mm
int state,state_in,button_f,count,Msensor_f,walk_count,filler_countL,filler_countR,count1,count_side,count_gtgfast = 0;
int turn_state = 0; // Left when 0 right when 1
VarSpeedServo FMleg,BMleg,FLleg,FRleg,BLleg,BRleg; //Sets the servos using the VarSpeedServo library
void setup() {
  // put your setup code here, to run once:
  FMleg.attach (4); //D4
  BMleg.attach (5); //D5
  FLleg.attach (6); //D6
  FRleg.attach (7); //D7
  BLleg.attach (8); //D8
  BRleg.attach (9); //D9
  pinMode(button, INPUT_PULLUP); //Sets the button as an INPUT with pullup resistor
  pinMode(fillerL, INPUT_PULLUP); //Set left feeler as input
  pinMode(fillerR, INPUT_PULLUP); //Set right filler as input
  pinMode(light_f,INPUT); // Sets front light sensor as input
  pinMode(light_l,INPUT); // Sets front left sensor as input
  pinMode(light_r,INPUT); // Sets front right sensor as input
  pinMode(light_b,INPUT); // Sets front back sensor as input
  pinMode(LED, OUTPUT);
  
   //Sets D3 as external interrupt for left feeler
  attachInterrupt(digitalPinToInterrupt(fillerL), filler_touchL ,FALLING); 
  //Sets D2 as external interrupt for right feeler
  attachInterrupt(digitalPinToInterrupt(fillerR), filler_touchR ,FALLING);
  digitalWrite(LED,HIGH);
  
Idle(); //The robot starts at a stable position
  
}

void loop(){

  int turn_amount = 6; // Makes the robot rotate 90 degrees
  int light_amount = 751; // A voltage input of 3.66V is needed for the robot to react
  //Used to initialize values.
if (state_in == 0){
  
    turn_state = 0; 
    count_side = 0;
    filler_countL = 0;
    filler_countR = 0;
 state_in = 1;
  
}
//State waits for the button to be pressed
else if ( state_in == 1 ){
 if (digitalRead(button) == 0){ //Reads the digital input and waits for a LOW input
  state_in = 2;
 }
  else{
  state_in = 1;
  }
}

  else if ( state_in == 2 ){
      sonar.ping_timer(sense); // An ultrasonic burst is  sent out and sense() listens for return
      delay(211); //delays for 211 ms so that sensor can listen for return
    state_in = 3;
  }
  
  //Checks if ultrasonic sensor returns
  else if (state_in == 3){
    if(Msensor_f == 1){ //This means that there is an object in front and the robot will react
      filler_countL = 0; 
      filler_countR = 0;
      Msensor_f = 0;
      state_in = 20;
    }
    else{
      state_in = 4;
    }
  }

  //Checks whether left feeler has been pressed
   else if (state_in == 4){
      if(filler_countL == 1){
     count_side = count_side + 1;  
     //Once count_side reaches 3 times the robot rotates instead of moving to the side
    filler_countL = 0;
    filler_countR = 0;
    count = 0;
    state_in = 40;
      }
  else
  state_in = 5;
   }
   
   //Checks whether the right feeler has been pressed
   else if (state_in == 5){
    if(filler_countR == 1){
     count_side = count_side + 1;  
    filler_countR = 0;
    filler_countL = 0;
    count = 0;
    state_in = 80;
      }
  else
  state_in = 6;
   }
   
   //Checks for intense light on the left side
    else if (state_in == 6){
      if ( analogRead(light_l) > light_amount){// the robot will only react to 3.66v input
        count = 0;
        state_in = 90;
      }
      else{
      state_in = 7;
      }
    }
    //Checks for intense light on the right side
    else if (state_in == 7){
      if ( analogRead(light_r) > light_amount){// the robot will only react to 3.66v input
        count = 0;
        state_in = 95;
      }
      else{
      state_in = 8;
      }
    }
    //Checks for intense light right in front
    else if (state_in == 8){
      if ( analogRead(light_f) > light_amount){// the robot will only react to 3.66v input
        count = 0;
        state_in = 100;
      }
      else{
      state_in = 9;
      }
    }
    //Checks for intense light behind the robot
    else if (state_in == 9){
      if(analogRead(light_b) > light_amount){// the robot will only react to 3.66v input
        count_gtgfast = 11 ;
      }
      state_in = 10;
    }

    //Walks one step
   else if (state_in == 10){
    Walk();                 //Calls the walk forward function
    count_gtgfast = count_gtgfast - 1; //Used to maintain the quick movement when there is light behind
    delay(411-back_delay()); //delays the system for the servos to move into position
    state_in = 2;
   }
////////////////////////////////////////Sensor part
//The robot moves sideways according to turn state
  else if (state_in == 20){
    if (turn_state == 0){
      side_wayl(); //Calls sideways left movement
      delay(511-back_delay());//delays the system for the servos to move into position
    }
   else{
      side_wayr();//Calls sideways right movement
      delay(511-back_delay());//delays the system for the servos to move into position
   }
   state_in = 21;
  }

  //Checks whether the feelers are touched during sideways movement
  else if (state_in == 21){
    if (turn_state == 0) {
      if(filler_countL == 1){ //Left feeler is touched
      filler_countL = 0;
      filler_countR = 0;
        state_in = 200;
      }
       else{
        state_in = 22;  
       }
      }
    else{
      if(filler_countR == 1){//Right feeler is touched
        filler_countR = 0;
        filler_countL = 0;
        state_in = 211;
      }
       else{
        state_in = 22;  
       }
      }

}

//Repeats the sideways movement until robot is out of way of object
  else if (state_in == 22){
    if (count < 8){
      count = count + 1;
      state_in = 20;
    }
    else {
      count = 0;
      state_in = 23;
    }
  }

  
  else if (state_in == 23){
    sonar.ping_timer(sense);//Sends out ultrasonic burst and sense() listens for return
    delay(211); //delays for 211 ms so that sensor can listen for return
    state_in = 24;
  }

  //Checks whether there is return in ultrasonic sensor
   else if (state_in == 24){
    if(Msensor_f == 1){//Means that object is still blocking robot
      count1  = count1 + 1;
      Msensor_f = 0;
      state_in = 60;
    }
    //Returns state to moving forward if no return
    else{
      if (count1 > 2){
        change_turn();
      }
      count1 = 0;
      state_in = 2;
    }
  }
  ////////Sensor turning after side ways move
  //Robot will rotate so that it moves along object
  else if (state_in == 60){
    if(turn_state == 0){
      for(count = 0; count < turn_amount; count ++ ){ //turn 90 degrees,.
      turning_l(); //Calls the rotate left movement
      delay (511-back_delay());//delays the system for the servos to move into position
      state_in = 23;
      }
    }
    else{
      for(count = 0; count < turn_amount; count ++ ){// turn 90 degrees,.
      turning_r(); //Calls the rotate right movement
      delay (511-back_delay()); //delays the system for the servos to move into position
      state_in = 23;
      }
    }
    count = 0;
    
  }

 ////////Sensor turning when touch filler
 //Robot turns right away from feeler thats been touched
  else if (state_in == 200){ 
    for(count = 0; count < turn_amount; count ++ ){ //turn 90 degrees,.
      turning_r(); //Calls the rotate right movement
      delay (511-back_delay()); //delays the system for the servos to move into position
    }
    change_turn(); //changes the turn state
    state_in = 2;
  }
//Robot turns left away from feeler thats been touched
  else if (state_in == 211){ 
      for(count = 0; count < turn_amount; count ++ ){ //turn 90 degrees,.
      turning_l(); //Calls the rotate left movement
      delay (511-back_delay()); //delays the system for the servos to move into position
    }
    change_turn(); //changes the turn state
    state_in = 2;
  }
///////////////Left Filler touched
//Robot moves sideways right away from touched feeler
  else if (state_in == 40){
    //Robot turns away when feeler are touched too many times
    if (count_side > 2){
      for(count = 0; count < turn_amount; count ++ ){
      turning_r();  //Calls rotate right movement
      delay (511-back_delay()); //delays the system for the servos to move into position
      }
      count_side = 0;
      state_in = 2;
    }
    else{
      side_wayr(); //Calls sideways right movement
      delay(511-back_delay()); //delays the system for the servos to move into position
      state_in = 41;
    }
  }

  //Checks if opposite feeler is touched
  else if (state_in == 41){
      if(filler_countR == 1){
      filler_countR = 0;
        state_in = 400;
      }
       else{
        state_in = 42;  
       }
    }

  //Takes 8 steps sideways
  else if (state_in == 42){
    if (count < 8){
      count = count + 1;
      state_in = 40;
    }
    else {
      count = 0;
      filler_countL = 0;
      filler_countR = 0;
      state_in = 2;
    }
  }

  //When the opposite feelers is touched, the robot turns away from touched feeler
  else if (state_in == 400){
          for(count = 0; count < turn_amount; count ++ ){
      turning_l();//Calls rotate left movement
      delay (511-back_delay());//delays the system for the servos to move into position
    }
    state_in = 2;
  }
  ///////////////Right Filler touched
  //Robot moves sideways left away from touched feeler
   else if (state_in == 80){
    //Robot turns away when feeler are touched too many times
    if (count_side > 2){
      for(count = 0; count < turn_amount; count ++ ){
      turning_l();//Calls rotate left movement
      delay (511-back_delay());//delays the system for the servos to move into position
      }
      count_side = 0;
      state_in = 2;
    }
    else{
      side_wayl();//Calls sideways right movement
      delay(511-back_delay());//delays the system for the servos to move into position
      state_in = 81;
        
    }
  }

  //Checks if opposite feeler is touched
  else if (state_in == 81){
      if(filler_countL == 1){
      filler_countL = 0;
        state_in = 800;
      }
       else{
        state_in = 82;  
       }
    }
    
  //Takes 8 steps sideways
  else if (state_in == 82){
    if (count < 8){
      count = count + 1;
      state_in = 80;
    }
    else {
      count = 0;
      filler_countL = 0;
      filler_countR = 0;
      state_in = 2;
    }
  }
    //When the opposite feelers is touched, the robot turns away from touched feeler
  else if (state_in == 800){
          for(count = 0; count < turn_amount; count ++ ){
      turning_r();//Calls rotate right movement
      delay (511-back_delay());//delays the system for the servos to move into position
    }
    state_in = 2;
  } 

//////////When there is light on the left
//Robot turns right away from direction of light
  else if (state_in == 90){
          for(count = 0; count < 8; count ++ ){ //Turn 90 degrees
      turning_r();//Calls rotate right movment
      delay (511-back_delay()); //delays the system for the servos to move into position
    }
    state_in = 2;
  }
 /////////When there is light on the right
 //Robot turns left away from direction of light
    else if (state_in == 95){
          for(count = 0; count <8; count ++ ){ //Turn 90 degrees
      turning_l();//Calls rotate left movment
      delay (511-back_delay()); //delays the system for the servos to move into position
    }
    state_in = 2;
  }  
/////////////When there is light in front
//Robot moves backward away from light
  else if (state_in == 100){
    for(count = 0; count < 6; count ++ ){ 
      walkback(); // Call backwards movement
      delay (511-back_delay()); //delays the system for the servos to move into position
    }
    state_in = 101;
  }
  
  //Robot turns 180 to move directly away from light
  else if (state_in == 101){
    for(count = 0; count < 12; count ++ ){//Turns 180 degrees
      if(turn_state == 0){
        turning_l(); //Call rotate left movement
        delay (511-back_delay());//delays the system for the servos to move into position
      }
      else{
        turning_r(); //calls rotate right movement
        delay (511-back_delay());//delays the system for the servos to move into position
      }
    }
    state_in = 2;
  }
}

//Changes turn state for robot to decide where to move towards when sensor are activated
void change_turn(){
  if (turn_state == 1){
          turn_state = 0;
        }
        else{
          turn_state = 1;
        }
}

//This function is serviced when the left feeler is touched as D3 is an external interrupt
void filler_touchL()
{
 filler_countL = 1; //Signals main system left feeler is touched
  }
  
//This function is serviced when the right feeler is touched as D2 is an external interrupt
void filler_touchR()
{
 filler_countR = 1; //Signals main system right feeler is touched
  }

//Called at the beginning for a stable reset position
void Idle()
{
motion_set(100,100,135,999,999,45); //Lifts and set the front left and back right legs
delay(511);
motion_set(80,80,999,134,45,999); //Lifts and set the front right and back left legs
delay(511);
motion_set(90,90,135,135,45,45); //Set the legs in stable position
  }

//Called for forward movement
void Walk()
{
  if (state == 0){
  motion_set(75,75,45,135+21,135,45-21);//Lifts and moves front right and back left legs forward
  state = 1;
  }
  else if (state == 1){
  motion_set(105,105,135,45-21,45,135+21);//Lifts and moves front left and back right legs forward
  state = 0;
  }
}

//Called for backward movement
void walkback(){
    if (state == 0){
  motion_set(105,105,45,135+21,135,45-21);//Lift and moves front left and back right legs backward
  state = 1;
  }
  else if (state == 1){
  motion_set(75,75,135,45-21,45,135+21);//Lift and moves front right and back left legs backward
  state = 0;
  }
}

//Called for rotate right movement
void turning_r()
{
    if (state == 0){
  motion_set(75,75,45,45,135,135);//Lifts front right and back left
  state = 1;
  }
  else if (state == 1){
  motion_set(105,105,135,135,45,45); //Lifts front left and back right
  state = 0;
  }
}

//Called for rotate left movement
void turning_l()
{
    if (state == 0)
  {
  motion_set(105,105,45,45,135,135);//Lifts front left and back right
  state = 1;
  }
  else if (state == 1)
  {
  motion_set(75,75,135,135,45,45);//Lifts front right and back left
  state = 0;
  }
}

//Called for sideways left movement
void side_wayl()
{
 if (state == 0)
  {
  motion_set(75,75,180,180,60,60);//Lifts front right and back left
  state = 1;
  }
  else if (state == 1)
  {
    motion_set(105,105,120,120,0,0);//Lifts front left and back right
  state = 0 ;
  }
}

//Called for sideways right movement
void side_wayr()
{
  if (state == 0)
  {
  motion_set(105,105,180,180,60,60);//Lifts front left and back right
  state = 1;
  }
  else if (state == 1)
  {
    motion_set(75,75,120,120,0,0);//Lifts front right and back left
  state = 0 ;
  }
}

//Changes the speed of robot when light detected behind
float back_speed(){
  float speed_count = 0;
  if( count_gtgfast > 0){ // When sufficient light is detected behind
   speed_count = 15+(analogRead(light_b) * 0.01);//increases speed by 10 + 15 theoratically
  }
  else{
    speed_count = 0;
  }
  return(speed_count);
}

//Changes the delay of robot when light detected behind
float back_delay(){
    float delay_count = 0;
  if ( count_gtgfast > 0){// When sufficient light is detected behind
   delay_count =150+ (analogRead(light_b) * 0.0976);//decrease delay by 100 + 150 theoratically
  }
  else{
    delay_count = 0;
  }
    
  return(delay_count);
}

//Used to input and set the angle of legs with set rotation speed
void motion_set(float FMang, float BMang , float FLang, float FRang, float BLang,float BRang)
{ 
  float FMold,BMold,FLold,FRold,BLold,BRold;
  if (FMang != 999){      // ignores the input angle 999
  FMleg.slowmove(FMang,61 + back_speed() ); //inputs the angle for front middle leg with speed of 61
  }
  if (BMang != 999){      // ignores the input angle 999
  BMleg.slowmove(BMang+3,61 + back_speed());//inputs the angle for back middle leg with speed of 61
  }
  if (FLang != 999){      // ignores the input angle 999
  FLleg.slowmove(FLang-5,61 +back_speed());//inputs the angle for front left leg with speed of 61
  }
  if (FRang != 999){      // ignores the input angle 999
  FRleg.slowmove(190-FRang,61 + back_speed());//inputs the angle for front right leg with speed of 61
  }
  if (BLang != 999){      // ignores the input angle 999
  BLleg.slowmove(BLang+15,61 + back_speed());//inputs the angle for back left leg with speed of 61
  }
  if (BRang != 999){      // ignores the input angle 999
  BRleg.slowmove(180-BRang,61 + back_speed());//inputs the angle for back right leg with speed of 61
  }
  //31
}

//Ultrasonic sensor checks for return at rate of 30Hz 
void sense(){
  if(sonar.check_timer()){ //returns 1 on return and 0 is nothing returs.
    Msensor_f = 1; //Signals main system something is in front of robot
  }
}




