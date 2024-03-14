/* secret_door
   March 3, 2024
   Josh Falter 
*/

#include <Bounce2.h>
#include<MD_YX5300.h>
#include <SoftwareSerial.h>

const int PIN_BOOK = A0;
const int PIN_LOCK = A1;
const uint8_t ARDUINO_RX = 0;
const uint8_t ARDUINO_TX = 1;

Bounce bookSwitch = Bounce();
SoftwareSerial  MP3Stream(ARDUINO_RX, ARDUINO_TX);
MD_YX5300 mp3(MP3Stream);

enum Mode {Initializing, Locked, Unlocked};
Mode doorState = Mode::Initializing;

void setup() {

  //Setup the output signal to go to the relay 
  pinMode(PIN_LOCK, OUTPUT);
  //Setup the input signal for the switch in the book
  bookSwitch.attach(PIN_BOOK, INPUT_PULLUP);

  //Startup the MP3Stream 
  MP3Stream.begin(MD_YX5300::SERIAL_BPS);
  mp3.begin();
  //Play the sound to indicate that the door is turned on
  mp3.playNext();

  resetDoor();
}

void loop() {
  //Get the current state of the bookSwitch.  
  //LOW  - indicates book has been pulled
  //HIGH - indicates book is down
  bookSwitch.update();
  int currentState = bookSwitch.read();

  //If the door is locked and the book has been pulled
  if(doorState == Mode::Locked && currentState == LOW)
      openDoor();
  //If the door state is unlocked and the book is put back down 
  else if(doorState == Mode::Unlocked && currentState == HIGH)
      resetDoor();
}

void openDoor() {
  //Set the lock signal to low, which will turn off the signal to the relay
  digitalWrite(PIN_LOCK, LOW);
  //Play the song on the SD card
  mp3.playNext();
  //Set mode to unlocked so we know to reset door next time the book is pushed down
  doorState = Mode::Unlocked;
}
void resetDoor() {
  doorState = Mode::Locked;
  //Set the lock signal to high, which will turn on the signal to the relay 
  //which will trigger the NO and Common on the relay to feed 12V to the magnet
  digitalWrite(PIN_LOCK, HIGH);
}
