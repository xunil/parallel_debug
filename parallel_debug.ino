#include "DigitLedDisplay.h"

/* Arduino Pin to Display Pin
  A0 to DIN,
  A1 to CS,
  A2 to CLK */
#define DIN A0
#define CS  A1
#define CLK A2   

DigitLedDisplay ld = DigitLedDisplay(DIN, CS, CLK);

volatile byte latchByte = 0x00;
volatile bool strobe = false;
long loopCount = 0;

void respondToStrobe() {
  latchByte = PINK;
  strobe = true;
}

void setup() {

  /* Set the brightness min:1, max:15 */
  ld.setBright(6);

  /* Set the digit count */
  ld.setDigitLimit(8);

  /* DB25  Arduino
   * Pin#  Pin#     Signal name   Wire color
   * ----  -------  -----------   ------------
   *  1    A7       /STROBE       Blue
   *  2    A8       D0            Gray
   *  3    A9       D1            Green
   *  4    A10      D2            Yellow
   *  5    A11      D3            White/Brown
   *  6    A12      D4            Orange
   *  7    A13      D5            Red
   *  8    A14      D6            Brown
   *  9    A15      D7            White
   * 10    53       ACK           Purple
   * 11    51       /BUSY         White/Black
   * 25    GND      GND           Black
   * 
   */

  pinMode(A7,  INPUT_PULLUP);   // /STROBE signal
  DDRK = 0x00;            // all input on pins A8-A15
  pinMode(53, OUTPUT);     // ACK signal
  pinMode(51, OUTPUT);     // /BUSY signal

  digitalWrite(53, LOW);
  digitalWrite(51, HIGH);  // not busy

  attachInterrupt(digitalPinToInterrupt(19), respondToStrobe, FALLING);
  interrupts();
}

void loop() {
  // turn on decimal point for digit 5
  //     ld.write(5, B10000000);
  // write a number starting at rightmost digit, without regard for length
  //    ld.printDigit(i);

  if (strobe) {
    // strobe line asserted, new byte to show
    noInterrupts();
    strobe = false;
    interrupts();
    ld.clear();
    ld.printDigit(latchByte);   
  }
  delay(5);
  loopCount++;
  if (loopCount > 0 && loopCount % 200 == 0) {
    loopCount = 0;
    digitalWrite(51, LOW);
    delay(2);
    digitalWrite(51, HIGH);
  }
}
