// Led valojen pin numerot
const int ledRed = 7;
const int ledGreen = 6;
const int ledBlue = 5;

// Nappuloiden pin numerot 
const int buttonRed = 12;
const int buttonGreen = 11;
const int buttonBlue = 8;

// Painetun nappulan pin numero. Käsitellään keskeytyksessä.
// Volatile tarkoittaa sitä, että arvo voi muuttua ennalta odottamattomalla tavalla. Estää kääntäjää optimoimasta sitä esim vakioksi.
volatile int buttonPressedPin = -1;

// Ennalta määrätty PCI metodi portille B (pins D8:D13)
// Rekisteröi mitä näppäintä painettiin.
ISR (PCINT0_vect) {
  if ( digitalRead(buttonRed)  == HIGH ) { // == LOW jos käytetään pull-up vastuksia kytkimissä.
    buttonPressedPin = buttonRed;
  }
  else if ( digitalRead(buttonGreen)  == HIGH ) {
    buttonPressedPin = buttonGreen;
  }
  else if ( digitalRead(buttonBlue)  == HIGH ) {
    buttonPressedPin = buttonBlue;
  }
}

// Määritetään pinien ominaisuudet ja aloitus.
void setup() {
  // Enable PCIE0 Bit0 = 1 (Port B)
  PCICR |= B00000001;
  // Valitaan PCINT3 Bit4, 3, 0 = 1 (Port D12, D11, D8)
  PCMSK0 |= B00011001;
  // Ottaa käyttöön siemenen random numeroille väliltä 0-1023 riippuen analog pin A0 inputista. Saadaan ympäröivän magneettikentän muutoksesta jokin random jännite.
  randomSeed(analogRead(A0));

  // Asetetaan ledejen pin modet.
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  // Asetetaan nappuloiden pin modet.
  pinMode(buttonRed, INPUT);
  pinMode(buttonGreen, INPUT);
  pinMode(buttonBlue, INPUT);
  // Välkytetään valoja ohjelman käynnistymisen merkiksi.
  for (int i = 0; i < 3; i++){
    allLedsOn(true);
    delay(500);
    allLedsOn(false);
    delay(500);
  }
}

void loop() {

  // static lokaali muuttuja säilyttää arvonsa funktiokutsujen välillä.
  static int blinkDelayMs = 50;
  static int multiplier = 50;
  static int level = 0;
  
  long ledCurrentlyOn = -1;
  int buttonToBePressedPin = -1;
  bool continueToNextStep = false;
  
  // Nollataan painettu nappi kierroksen alussa.
  buttonPressedPin = -1;
  // Määritetään mikä led on päällä.
  ledCurrentlyOn = switchLedOn(random(5,8));

  // Määritetään mitä näppäintä tulee painaa jotta peli jatkuu.
  switch(ledCurrentlyOn){
    case ledRed:
      buttonToBePressedPin = buttonRed;
      break;
    case ledGreen:
      buttonToBePressedPin = buttonGreen;
      break;
    case ledBlue:
      buttonToBePressedPin = buttonBlue;
      break;
    default :
      break;
  }
  
  // Viive jonka aikana nappia täytyy ehtiä painamaan.
  delayLoop(multiplier, blinkDelayMs);
  // Tarkistetaan onko painettu oikeaa nappia.
  continueToNextStep = wasPressCorrect(buttonToBePressedPin);
  
  if ( continueToNextStep ) {
    digitalWrite(ledCurrentlyOn, LOW);
    level++;
    multiplier = changeDelay(multiplier, blinkDelayMs);
  }
  else {
    buttonToBePressedPin = -1;
    allLedsOn(true);
    while(true){
      //valot jää päälle.
    }
  }
}

// Tarkistetaan onko painettu oikeaa nappia.
bool wasPressCorrect(int buttonToBePressedPin) {
  if ( buttonPressedPin == buttonToBePressedPin ) {
    return true;
  }
  return false;
}

// Sammuttaa tai sytyttää kaikki ledit.
void allLedsOn(bool on_off) {
  digitalWrite(ledRed, on_off);
  digitalWrite(ledGreen, on_off);
  digitalWrite(ledBlue, on_off);
}

// Pienentää viivettä mikä on aikaa painaa nappia. Palauttaa uuden viiveen kertoimen(silmukan kierrosten lkm).
int changeDelay(int multiplier, int blinkDelayMs) {
  if (multiplier > 1000) multiplier -= 5;
  else multiplier -= 2;
    
  delay(multiplier * blinkDelayMs);
  return multiplier;
}

// Viivesilmukka joka tarkistaa väliajoin onko nappia painettu.
void delayLoop(int multiplier, int blinkDelayMs) {
  for (int i = 0; i < multiplier; i++){
    delay(blinkDelayMs);
    // button press breaks the loop
    if ( buttonPressedPin != -1 ) { 
      break; 
    }
  }
}

// Sytyttää random ledin. Palauttaa sytytetyn ledin pinin numeron.
long switchLedOn(long pinForLed) {
    digitalWrite(pinForLed, HIGH);
    return pinForLed;
}
