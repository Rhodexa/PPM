class PPMCapture {
  public:
    static const unsigned int SYNC_PULSE_LENGTH = 6000;              // ~ 3ms (normally, anything more than 2ms should be considered an end of frame)
    static const unsigned int CHANNEL_COUNT = 7;                     // 7 is typical... Walkera/Phantom 2 Trainer Port

    static const unsigned int PULSE_COUNT = CHANNEL_COUNT + 1;       // Don't touch this... lol

    static uint16_t temp_channels_buffer[PULSE_COUNT]; 
    static uint16_t channels[CHANNEL_COUNT];
    static uint8_t channel_counter;

    PPMCapture() {}

    void begin() {
      // CTC Mode, ICR1, at clk/8
      TCCR1A = B00000000;
      TCCR1B = B01011010;
      TIMSK1 |= (1 << ICIE1) | (1 << OCIE1A);
      OCR1A = SYNC_PULSE_LENGTH;
    }

    void pulse(){
      if(channel_counter < PULSE_COUNT) {
        temp_channels_buffer[channel_counter] = ICR1;
        channel_counter++;
      }
      else if (channel_counter < PULSE_COUNT + 1) channel_counter++;
    }

    void sync() {
      if (channel_counter < PULSE_COUNT) return;
      if (channel_counter > PULSE_COUNT) return;

      for(byte i = 0; i < CHANNEL_COUNT; i++) {
        channels[i] = temp_channels_buffer[i+1];
      }

      channel_counter = 0;
    }
};

uint16_t PPMCapture::temp_channels_buffer[PULSE_COUNT]; 
uint16_t PPMCapture::channels[CHANNEL_COUNT];
uint8_t PPMCapture::channel_counter = 0;

PPMCapture ppm;

ISR (TIMER1_CAPT_vect) {
  ppm.pulse();
}

ISR (TIMER1_COMPA_vect) {
  ppm.sync();
}

void setup(){
  ppm.begin();
}

void loop(){
  for(int i = 0; i < ppm.CHANNEL_COUNT; i++){
    Serial.print(ppm.channels[i]);
    Serial.print(' ');
  }
  Serial.print('\n');
}
