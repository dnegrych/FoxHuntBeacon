/*

Radio Fox Tranmitter
by Nathanael Wilson
modified by Dave Negrych VE4NEG - April 2016

This transmits a long tone, followed by a call in morse code.
It then waits an ammount of time then transmits it again.
This transmits by connecting to a radio's ptt and mic input.
Audio output on pin 8

*/

#define tonehz 735 //the aproxamite frequency of the tones in hz, in reality it will be a tad lower, and includes lots of harmonics.
#define dit 64 //the length of the dit in milliseconds. The dah and pauses are derived from this.
//#define rest 60000 //the ammount of time between transmits in milliseconds
#define longlength 6000 //length of long tone in milliseconds
#define tx 11 //the pin of the board then keys the radio
#define audio 8 //the pin of the board then outputs the audio

unsigned long rest = 30000;  // <<<<<<<<<<<<<<<<<< set this to change rest period
  
//Do not change this array, or your morse code may not be converted properly.
String morseTable[] = {"0","12","2111","2121","211","1","1121","221","1111","11","1222","212","1211","22","21","222","1221","2212","121","111","2","112","1112","122","2112","2122","2211","22222","12222","11222","11122","11112","11111","21111","22111","22211","22221"};


String Text = "VE4NEG Fox Hunt"; //This is what will be transmitted
//String easterEgg = "VE4NEG Fox found go to www dot hackaday dot com"; //This will be transmitted instead if pin 10 is held high at reset. 

bool egg = 0;

//int TextChars = 15;
//int CodeChars;

//String EEgg = formMorse(easterEgg);
String code = formMorse(Text);
char inChar;
String inputString = "";
int peekInt;
boolean stringComplete = true;
//boolean stringComplete = false;
boolean sending = false;
//boolean debug = true;
boolean debug = false;
boolean led = HIGH;

int duration;
int note = 1000000 / tonehz; //converts the frequency into period
unsigned long waitTimer = 10000;

void setup(){ //set the pins to output mode

  pinMode(tx, OUTPUT);
  pinMode(10, INPUT);
  pinMode(audio, OUTPUT);
  pinMode(13, OUTPUT); //this is to see how the code looks with an led
  
  egg = digitalRead(10);
  
  Serial.begin(9600);
  //Serial.println(egg);
  //Serial.println(Text);
  //Serial.println(code);
  //Serial.println(easterEgg);
  //Serial.println(EEgg);
  annouce();
 }

//--------------------------------------------------------------;
void loop(){
   
while (stringComplete) {  
  digitalWrite(tx, HIGH); //starts the radio transmitting
  Serial.println("Sending long tone.");
  playtone(longlength);
  delay(250);
  
  //if (egg){
    //Serial.println(EEgg);
    //playcode(EEgg);
  //}
  //else{
    //Serial.println(code);
    Serial.println("Sending CW code.");
    playcode(code);
  //}

  delay(250);
  Serial.println("Sending long tone.");
  playtone(longlength);
  digitalWrite(tx, LOW); //Stops the radio's transmission
  restPeriod(rest);
}
}
//------------------------------------------------------------------------------;
void serialEvent() {

/*  if (sending) {
    stringComplete = !stringComplete;
    sending = !sending;
    // if ( debug) {      Serial.println("Serial debug 1");     } 
  }
*/

  while (!stringComplete and Serial.available()) {
    getinput();
    
    // Serial.print(" #1 Char input = ");
    //Serial.print(inChar);  
    if (inChar == char(10) || inChar == char(13)) { //buffering till enter or CR/LF pressed
      stringComplete = true;
      sending = true;
      code = "";
      if (inputString=="") {
        code = formMorse(Text);
        Serial.println("Sending default text.");
        Serial.println(Text);
        }
      else {
        code = formMorse(inputString);
        Serial.println("Sending entered text.");
        Serial.println(inputString);
         }
      break; 
      } 
     inputString += inChar;  // add input character to buffer
    }   // While ends here
    }  
//---------- Serial Event ends here ----------------------------------------------//
//------------------------------------------------------------------------------;
void playtone(int note_duration){

  long elapsed_time = 0;
  long startTime=millis();
  if (note_duration > 0){
    digitalWrite(13, HIGH); //See when it is making a tone on the led
    while (elapsed_time < note_duration){
      digitalWrite(audio, HIGH);
      delayMicroseconds(note / 2);
      digitalWrite(audio, LOW);
      delayMicroseconds(note / 2);
      elapsed_time = millis()-startTime;
    }
    digitalWrite(13, LOW);
  }
  else{ //if it's a pause this will run
    delay(dit * 2);
  }
}

void playcode(String input){
  
  for (int i=0; i < input.length(); i++){
    //Serial.print(input[i]);
    if (input[i] == '0'){ //See if it's a pause
      duration = 0;
    }
    else if (input[i] == '1'){ //See if it's a dit
      duration = dit;
    }
    else if (input[i] == '2'){ //See if it's a dah
      duration = dit * 3;
    }
    playtone(duration);
    delay(dit); //makes a pause between sounds, otherwise each letter would be continuous.
  }
}
String formMorse(String input){

    input.toUpperCase();
    String output = "";
    for(int i=0; i < input.length() ;i++){
        if (input[i] >= 65 && input[i] <= 90)
            output = output + morseTable[input[i]-64] + '0';
        else if (input[i] >= 48 && input[i] <= 57)
            output = output + morseTable[input[i]-21] + '0';
        else if (input[i] == 32)
            output = output + morseTable[0];
    }
    return output;

}

//---------------------------------------------------------------------;
void restPeriod(unsigned long rest) {
   Serial.print("Resting for "); 
   Serial.print(rest/1000);
   Serial.println(" seconds");
   wait(rest/1000);
   //delay(rest); 
   //Serial.println("Press enter now to change message.");
   //delay(2000);
   //checkforbreak();

}
//---------------------------------------------------------------------;

boolean checkforbreak(){
  
 if ( Serial.available()) {
         getinput();
          if (inChar > 0) {
          // get input from console
          stringComplete = false;
          //inputString[0] = char(peekInt);
          inputString = "";
          annouce();
          Serial.println("Waiting for input");
          Serial.flush();
          return 1;

          }         
 }
 return 0;
}
//---------------------------------------------------------------------;

void getinput() {
    inChar = toUpperCase((char)Serial.read());
    if (debug) {    Serial.print(" Char input = "); Serial.println(inChar, DEC); }
  
}
//---------------------------------------------------------------------;
void annouce() {
  Serial.println("Press enter to change CW message.");
  // Serial.print("Or wait for default CW message to be sent.");
  Serial.print("Message text = '"); Serial.print(Text); Serial.println("'");
  //restPeriod(5000);
  // checkforbreak();
}
//-------------------------------------------------------------------------

void wait(int seconds) {
// wait for given seconds with check for input each second
  // Serial.print("Waiting ");
  // Serial.println(seconds);
   for(int i=0; i < seconds;i++){
         Serial.print(".");
         if (led == HIGH) {
           // playtone(dit * 3);  // VOX does not respond fast enough to send single dit or dah!
           led = LOW; 
          }
         else {
            led = HIGH;
         } 
        digitalWrite(13, led); 
        delay(1000);
        if (checkforbreak() == true) {
          //Serial.println(" "); 
          i = seconds;  
        }
        
    }
    Serial.println(); 
}

