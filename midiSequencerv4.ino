//By Mark O'Donnell 2014///
/*

A simple 8 step midi sequencer with adjustable pitch, speed and gate, using the arduino platform

*/

//analog pins
const int noteLengthPin = 3;
const int tempoPin = 4;
const int pitchPin = 5;

//digital pins
const int LEDPin = 12;
const int switchPinThree = 8;
const int switchPinTwo = 9;
const int switchPinOne = 10;

//outside controlled variables
int tempo = 1; //this is in fact the PERIOD of a SINGLE NOTE and its rest
int noteLength = 512; //the fraction over 1023 that a note lasts for within one period
int pitchOne = 48;
int pitchTwo = 48;
int pitchThree = 48;
int pitchFour = 48;
int pitchFive = 48;
int pitchSix = 48;
int pitchSeven = 48;
int pitchEight = 48;

//internal variables
const int sensitivity = 8; //hardcoded sensitivity to pitch changes
const int lengthResolution = 512; //how many levels we will give tempo and note length
int currChange = 1; //indicates which pitch we are currently changing
int currPlay = 1; //indicates which note is currently playing
int noteDuration = 1;

int channelOne = 0x90;
int silent = 0x00;
int mid = 0x45;

int pitchPot_state = 0;

int pitchPot_next = 0;

//functions which need to use pass-by-reference
void setPitch(int &pitch, int &pot_state, int pot_next); 
void setTime(int &initial, int &pot_state, int pot_next); //this function deals with changing the value of note length or tempo

//start of code
void setup()
{
  Serial.begin(31250); //baud rate of midi

  pinMode(LEDPin, OUTPUT); //setting up pins!
  pinMode(switchPinOne, INPUT);
  pinMode(switchPinTwo, INPUT);
  pinMode(switchPinThree, INPUT);
  
  digitalWrite(LEDPin, LOW); //LEDS start off
  
  readPins(); //initial read of pots to set initial states, this ensures that pitch is not erratic on turn on
  pitchPot_state = pitchPot_next;
}

void loop()
{
  readPins(); //reads pins and sets next values of holders
  
  changePitch(); //sets the pitch of the note we are currently controlling the pitch of

  playNote(); //sends a midi message
}

void readPins()
{
  tempo = analogRead(tempoPin);
  noteLength = analogRead(noteLengthPin);
  if(!noteLength)
    noteLength = 1;
  pitchPot_next = analogRead(pitchPin);
  currChange = 1 + digitalRead(switchPinOne) + digitalRead(switchPinTwo)*2 + digitalRead(switchPinThree)*4;

}

void changePitch()
{
  switch(currChange)
  {
    case 1:
      setPitch(pitchOne, pitchPot_state, pitchPot_next);
      break;
    case 2:
      setPitch(pitchTwo, pitchPot_state, pitchPot_next);
      break;
    case 3:
      setPitch(pitchThree, pitchPot_state, pitchPot_next);
      break;
    case 4:
      setPitch(pitchFour, pitchPot_state, pitchPot_next);
      break;
    case 5:
      setPitch(pitchFive, pitchPot_state, pitchPot_next);
      break;
    case 6:
      setPitch(pitchSix, pitchPot_state, pitchPot_next);
      break;
    case 7:
      setPitch(pitchSeven, pitchPot_state, pitchPot_next);
      break;
    case 8:
      setPitch(pitchEight, pitchPot_state, pitchPot_next);
      break; 
    default:
      setPitch(pitchOne, pitchPot_state, pitchPot_next);
  }
}

void setPitch(int &pitch, int &pot_state, int pot_next)
{
  if((pitch + (pot_next - pot_state)) >= 127) //if the pitch would be higher than allowed by midi, it reverts to the highest value
  {
    pitch = 127;
    pot_state = pot_next;
    return;
  }
  
  if((pitch + (pot_next - pot_state)) <= 0) //if the pitch would be lower than allowed by midi, it reverts to the highest value
  {
    pitch = 0;
    pot_state = pot_next;
    return;
  }
  
  if(((pot_next - pot_state)/sensitivity) >= 1  || ((pot_next - pot_state)/sensitivity) <= -1)
  {
    pitch = (pitch + ((pot_next - pot_state)/sensitivity)); //else the pitch change by the change in resistance [MAY NEED CALIBRATION] 
    pot_state = pot_next;
    changeLED();
  }
}

void playNote()
{
  noteDuration = (tempo/1023)*noteLength;
  Serial.write(channelOne); //channel one
  switch(currPlay){
    case 1:
      Serial.write(pitchOne);
      break;
    case 2:
      Serial.write(pitchTwo);
      break;
    case 3:
      Serial.write(pitchThree);
      break;
    case 4:
      Serial.write(pitchFour);
      break;
    case 5:
      Serial.write(pitchFive);
      break;
    case 6:
      Serial.write(pitchSix);
      break;
    case 7:
      Serial.write(pitchSeven);
      break;
    case 8:
      Serial.write(pitchEight);
      break;
    default:
      Serial.write(pitchOne);
  }

  Serial.write(mid); //middle velocity
  delay(noteDuration);
  
  if(currPlay != 8)
    currPlay++;
  else currPlay = 1;

  Serial.write(channelOne); //channel one again, need to silence note
  switch(currPlay){
    case 2:
      Serial.write(pitchOne);
      break;
    case 3:
      Serial.write(pitchTwo);
      break;
    case 4:
      Serial.write(pitchThree);
      break;  
    case 1:
      Serial.write(pitchFour);
      break;
    case 5:
      Serial.write(pitchFive);
      break;
    case 6:
      Serial.write(pitchSix);
      break;
    case 7:
      Serial.write(pitchSeven);
      break;
    case 8:
      Serial.write(pitchEight);
      break;
    default:
      Serial.write(pitchOne);
  }
  Serial.write(silent);
  delay(tempo - noteDuration);
} 
  
void changeLED()
{
  digitalWrite(LEDPin, !digitalRead(LEDPin));
}
