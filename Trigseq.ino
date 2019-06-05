
#define TRIG 2
#define PHASE 3
// pin 4-7 addr to switches and red leds
// pin 8-11 adddr to green leds
#define CH1 12
#define CH2 13
#define LENGTH_POT A6
#define FORWARDS A4
#define BACKWARDS A5

int redaddr, greenaddr, length, pulsedelay, halt, debounce, debouncedelay;

void trigger() {   // Interrupt routine for trigger input
  int s;
  
  if (halt)
      return;
      
  redaddr++;
  if (redaddr >= length)
      redaddr = 0;
  digitalWrite(4, redaddr & 1); 
  digitalWrite(5, (redaddr >> 1) & 1);
  digitalWrite(6, (redaddr >> 2) & 1);
  digitalWrite(7, (redaddr >> 3) & 1);
  
  s = digitalRead(A1);
  if (s == 0)
      digitalWrite(CH1, HIGH);
  else
      digitalWrite(CH1, LOW);
      
  greenaddr++;
  if (greenaddr >= length) 
      greenaddr = 0;
  digitalWrite(8, greenaddr & 1);
  digitalWrite(9, (greenaddr >> 1) & 1);
  digitalWrite(10, (greenaddr >> 2) & 1);
  digitalWrite(11, (greenaddr >> 3) & 1);  

  // Temporarily address switches to get switch value at green led position
  // If done quickly, the red led does not blink
  digitalWrite(4, greenaddr & 1); 
  digitalWrite(5, (greenaddr >> 1) & 1);
  digitalWrite(6, (greenaddr >> 2) & 1);
  digitalWrite(7, (greenaddr >> 3) & 1); 
  s = digitalRead(A1);
  if (s == 0)
      digitalWrite(CH2, HIGH);
  else
      digitalWrite(CH2, LOW);  
 
  // And switch the red diodes addressing back
  digitalWrite(4, redaddr & 1); 
  digitalWrite(5, (redaddr >> 1) & 1);
  digitalWrite(6, (redaddr >> 2) & 1);
  digitalWrite(7, (redaddr >> 3) & 1); 
  
  pulsedelay = 50;   // Number of loops in main loop until the output pulses 
                       // should be ended, this gives about 7 ms pulse
}

void phasing() {     // Interrupt routine for phasing input
  if (halt == 1)
      return;
      
  if (digitalRead(FORWARDS) == LOW) {
      greenaddr++;
      if (greenaddr >= length) 
        greenaddr = 0;      
  } else if (digitalRead(BACKWARDS) == LOW) {
    if (greenaddr == 0)
        greenaddr = length -1;
    else
        greenaddr--;
  }
  digitalWrite(8, greenaddr & 1);
  digitalWrite(9, (greenaddr >> 1) & 1);
  digitalWrite(10, (greenaddr >> 2) & 1);
  digitalWrite(11, (greenaddr >> 3) & 1);    
}

void setup() { 
  redaddr = 0;
  greenaddr = 0;
  halt = 0;
  debounce = 0;
  
  pinMode(TRIG, INPUT);  // Trig interrupt pin
  pinMode(PHASE, INPUT);  // Phase interrupt pin
  pinMode(4, OUTPUT); // Sw and red led addr
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT); // Green led addr
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(CH1, OUTPUT); // Ch1 output
  pinMode(CH2, OUTPUT); // Ch2 output
  
  pinMode(A0, INPUT_PULLUP);          // A0 = Phasing button (active low)
  pinMode(A1, INPUT_PULLUP);          // A1 = Switches
  pinMode(FORWARDS, INPUT_PULLUP);    // A4 = Forward (active low)
  pinMode(BACKWARDS, INPUT_PULLUP);   // A5 = Backwards (active low)  
  pinMode(LENGTH_POT, INPUT);         // A6 = Length pot
  
  digitalWrite(4, 0);  // Sw and red diodes addr = 0
  digitalWrite(5, 0);
  digitalWrite(6, 0);
  digitalWrite(7, 0);
  digitalWrite(8, 0);  // Green diodes addr = 0
  digitalWrite(9, 0);
  digitalWrite(10, 0);
  digitalWrite(11, 0);
  
  attachInterrupt(0, trigger, RISING);   // Enable trigger interrupt
  attachInterrupt(1, phasing, RISING);   // Enable phasing interrupt
 
}


void loop() { 
  int s;
  
  length = (analogRead(LENGTH_POT) >> 6) + 1;  // 1 - 16
  
  if (pulsedelay > 0) {
      pulsedelay--;
      if (pulsedelay == 0) {
         digitalWrite(CH1, LOW);
         digitalWrite(CH2, LOW);
      } 
     
  }
  
  // Check phasing button
  if ((digitalRead(A0) == LOW) && (debounce == 0)) {
      phasing();
      debounce = 1;
      debouncedelay = 1000;
  }
  
  if (debouncedelay > 0) {
     debouncedelay --;
     if (debouncedelay == 0)
        debounce = 0; 
  }
  
  // Check direction switch
  if ((digitalRead(FORWARDS) == HIGH) && (digitalRead(BACKWARDS) == HIGH))
      halt = 1;
  else
      halt = 0;  
      
}
