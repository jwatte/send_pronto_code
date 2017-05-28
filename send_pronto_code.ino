
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#define D9 9
#define D13 13

void setup() {
  pinMode(D9, OUTPUT);
  digitalWrite(D9, LOW);
  pinMode(D13, OUTPUT);
  digitalWrite(D13, HIGH);
  /* wait for things to stabilize */
  delay(500);
  /* OK,0x going now */
  digitalWrite(D13, LOW);
}

/* put codes in program memory to save on RAM */
static uint16_t const vizio_on[] PROGMEM = {
  0x0000, 0x006D, 0x0022, 0x0002,
  0x0157, 0x00AC, 0x0015, 0x0016, 0x0015, 0x0016, 0x0015, 0x0041, 0x0015, 0x0016, 0x0015, 0x0016, 0x0015, 0x0016, 0x0015, 0x0016,
  0x0015, 0x0016, 0x0015, 0x0041, 0x0015, 0x0041, 0x0015, 0x0016, 0x0015, 0x0041, 0x0015, 0x0041, 0x0015, 0x0041, 0x0015, 0x0041,
  0x0015, 0x0041, 0x0015, 0x0016, 0x0015, 0x0041, 0x0015, 0x0016, 0x0015, 0x0041, 0x0015, 0x0016, 0x0015, 0x0041, 0x0015, 0x0016,
  0x0015, 0x0016, 0x0015, 0x0041, 0x0015, 0x0016, 0x0015, 0x0041, 0x0015, 0x0016, 0x0015, 0x0041, 0x0015, 0x0016, 0x0015, 0x0041,
  0x0015, 0x0041, 0x0015, 0x0689, 0x0157, 0x0056, 0x0015, 0x0E94
};

/* make sure no code is longer than 128 words! */
uint16_t send_buf[128];

void sendCode(uint16_t const *code) {
  memcpy_P(send_buf, code, 2 * 4);
  memcpy_P(&send_buf[4], code + 4, code[2] * 2 * 2);
  memcpy_P(&send_buf[4 + code[2] * 2], code + 4 + code[2] * 2, code[3] * 2 * 2);

  uint16_t usHalf = 500000 * code[1] / 4145146;

  cli();
  for (int w = 0, n = send_buf[2] * 2; w != n; w += 2) {
    for (int p = 0, q = send_buf[4 + w]; p != q; ++p) {
      PORTB |= 0x2;
      delayMicroseconds(usHalf);
      PORTB &= ~0x2;
      delayMicroseconds(usHalf);
    }
    PORTB &= ~0x2;
    for (int p = 0, q = send_buf[4 + w + 1]; p != q; ++p) {
      delayMicroseconds(usHalf);
      delayMicroseconds(usHalf);
    }
  }
  for (int w = 0, n = send_buf[3] * 2; w != n; w += 2) {
    for (int p = 0, q = send_buf[4 + send_buf[2] * 2 + w]; p != q; ++p) {
      PORTB |= 0x2;
      delayMicroseconds(usHalf);
      PORTB &= ~0x2;
      delayMicroseconds(usHalf);
    }
    PORTB &= ~0x2;
    for (int p = 0, q = send_buf[4 + send_buf[2] * 2 + w + 1]; p != q; ++p) {
      delayMicroseconds(usHalf);
      delayMicroseconds(usHalf);
    }
  }
  sei();
}

bool sent = false;

void loop() {
  if (!sent) {
    sendCode(vizio_on);
    sent = true;
  }
  //  SLEEP FOREVAR!
  cli();
  sleep_enable();
  sleep_cpu();
  /* did I wake up from sleep? */
  digitalWrite(D13, HIGH);
}


