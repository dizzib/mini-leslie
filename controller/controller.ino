const int PIN_ENA    = 9;
const int PIN_BTN_L  = 11;
const int PIN_BTN_R  = 12;
const int RAMP_DELAY = 30; // ms

// assumes 13.5V input
enum STATE { STOP = 0, SLOW = 146, FAST = 235 };

void setup() {
  Serial.begin(9600);

  pinMode(PIN_ENA,  OUTPUT);
  pinMode(PIN_BTN_L, INPUT);
  pinMode(PIN_BTN_R, INPUT);

  // increase pwm width from 490Hz to 30Khz to stop motor buzz
  // http://forum.arduino.cc/index.php/topic,16612.0.html#4
  // MAY HAVE SIDE EFFECTS!
  TCCR1B = TCCR1B & 0b11111000 | 0x01;
}

void loop() {
  static int state = STOP;
  static int speed = STOP;

  setState(&state);
  setSpeed(&state, &speed);
}

void setState(int *state) {
  bool btn_l = digitalRead(PIN_BTN_L);
  bool btn_r = digitalRead(PIN_BTN_R);
  *state = btn_l ? (btn_r ? FAST : SLOW) : STOP;
}

void setSpeed(int *state, int *speed) {
  const int KICKSTART = 5;
  if (*speed > SLOW) switch (*state) {
    case FAST:
      if (*speed < FAST) (*speed)++;
      break;
    default:
      if (*speed > SLOW) (*speed)--;
      break;
  }
  else switch (*state) {
    case FAST:
      *speed = SLOW + 1;
      break;
    case SLOW:
      *speed = SLOW + ((*speed == STOP) ? KICKSTART : 0);
      break;
    case STOP:
      *speed = STOP;
      break;
  }
  Serial.println(*speed);
  analogWrite(PIN_ENA, *speed);
  delay(RAMP_DELAY);
}
