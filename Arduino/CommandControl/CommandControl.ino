#include <Servo.h> 

#define SERVO_COUNT   4
#define LED_COUNT     1
#define PIN_COUNT     4

#define CMD_BLINK           0  
#define CMD_SERVO           1  
#define CMD_REGISTER_SWITCH 2  
#define CMD_READ_SWITCH     3  
#define CMD_PIN             4  


Servo servo[SERVO_COUNT];
int servo_pin[SERVO_COUNT];

int led[LED_COUNT];          
int led_counter[LED_COUNT];  
int led_pin[LED_COUNT];      
long led_millis = 0;         

int minPulse = 900;   
int maxPulse = 2100;  

int startbyte;       
int cmd;             
int parm1;           
int parm2;           
int i;               

int relay_pin = 4;
int relay_state = LOW;


void setup()
{
  Serial.begin(57600);                    

  for (i=0; i < SERVO_COUNT; i++) {      
    servo_pin[i] = -1;
  }

  for (i=0; i < LED_COUNT; i++) {        
    led_pin[i] = -1;
    led_counter[i] = 0;
  }

  pinMode(relay_pin, OUTPUT);
  attachInterrupt(0, toggle_relay2, CHANGE);
} 


void set_led(int pin, int blinks)
{
  int i;

  for (i=0; i < LED_COUNT; i++) {      
    if (led_pin[i] == pin) {
      led_counter[i] = blinks;
      break;
    }
  }

  if (i == LED_COUNT) {                
    for (i=0; i < LED_COUNT; i++) {
      if (led_pin[i] == -1) {
        led_pin[i] = pin;
        led_counter[i] = blinks;
        pinMode(pin, OUTPUT);
        break;
      }
    }
  }
}


void blink_leds()
{
  int i;
  unsigned long now = millis();

  if(now - led_millis > 1000) {
    led_millis = now;

    for (i=0; i < LED_COUNT; i++) {
      if (led_counter[i] > 0) {
        if (led[i] == LOW) {
          led[i] = HIGH;
        } else {
          led[i] = LOW;
          led_counter[i] = led_counter[i] - 1;
        }

        digitalWrite(led_pin[i], led[i]);
      }
    }
  }
}


void move_servo(int pin, int angle)
{
  int i;

  for (i=0; i < SERVO_COUNT; i++) {      
    if (servo_pin[i] == pin) {
      servo[i].write(angle);
      break;
    }
  }

  if (i == SERVO_COUNT) {                
    for (i=0; i < SERVO_COUNT; i++) {
      if (servo_pin[i] == -1) {
        servo_pin[i] = pin;
        servo[i].attach(pin, minPulse, maxPulse);
        servo[i].write(angle);
        break;
      }
    }
  }
}


void toggle_relay()
{
  relay_state = !relay_state;
}


void toggle_relay2()
{
  int val = digitalRead(2);

  if (val == LOW) {
    relay_state = LOW;
  } else {
    relay_state = HIGH;
  }
}

void relay_on()
{
  relay_state = HIGH;
}


void relay_off()
{
  relay_state = LOW;
}


void loop() 
{
  if (Serial.available() > 3) { 
    startbyte = Serial.read();
    if (startbyte == 255) {     
      cmd = Serial.read();      
      if (cmd == 255) {         
        return;
      }

      parm1 = Serial.read();    
      if (parm1 == 255) {       
        return;
      }

      parm2 = Serial.read();    
      if (parm2 == 255) {       
        return;
      }

      switch (cmd) {
          case CMD_BLINK:
            set_led(parm1, parm2);
            break;

          case CMD_SERVO:
            move_servo(parm1, parm2);
            break;

          case CMD_PIN:
            pinMode(parm1, OUTPUT);
            if (parm2 == 0) {
                digitalWrite(parm1, LOW);
            } else {
                digitalWrite(parm1, HIGH);
            }
            break;

          case CMD_REGISTER_SWITCH:
          case CMD_READ_SWITCH:
            break;
      }
    }
  }
  blink_leds();
  digitalWrite(relay_pin, relay_state);
}
