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

    void updateChannels() {
      if (channel_counter < PULSE_COUNT) return;
      if (channel_counter > PULSE_COUNT) return;

      for(byte i = 0; i < CHANNEL_COUNT; i++) {
        channels[i] = temp_channels_buffer[i+1];
      }

      channel_counter = 0;
    }

    uint16_t getChannel(uint8_t index) {
      if (index >= CHANNEL_COUNT) {
        return 0; // or some error indication
      }
      return channels[index];
    }

    void loop(){
      for(int i = 0; i < CHANNEL_COUNT; i++){
        Serial.print(channels[i]);
        Serial.print(' ');
      }
      Serial.print('\n');
    }
};

uint16_t PPMCapture::temp_channels_buffer[PULSE_COUNT] = {0}; 
uint16_t PPMCapture::channels[CHANNEL_COUNT] = {0};
uint8_t PPMCapture::channel_counter = 0;

PPMCapture ppm;

void armPPMCapture() {
  ppm.begin();
}

/* Pulse Detected */
ISR (TIMER1_CAPT_vect) {
  if(ppm.channel_counter < ppm.PULSE_COUNT) {
    ppm.temp_channels_buffer[ppm.channel_counter] = ICR1;
    ppm.channel_counter++;
  }
  else if (ppm.channel_counter < ppm.PULSE_COUNT + 1) ppm.channel_counter++;
}

/* Synchronization Detected */
ISR (TIMER1_COMPA_vect) {
  ppm.updateChannels();
}

void setup(){
  ppm.begin();
}

void loop(){
  ppm.loop();
}
