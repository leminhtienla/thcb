#define pinSPD 2  //khai báo chân cảm biến tốc độ
#define pinBG A0  //khai báo chân cảm biến bướm ga
#define pinETC A1 //khai báo chân cảm biến nhiệt độ động cơ
#define teeth 1   //khai báo số răng cảm biến tốc độ
#define TBL_X 31
#define TBL_Y 2
#define etcR 1050.0 //Ohm
#define serial_ms 500

//unsigned long loops_count = 0;  //biến đếm số vòng lặp loops/s
unsigned long next_ms = 0, cur_ms = 0; //
unsigned int bg = 0;
float pulse_ms = 0, rps = 0, rpm = 0, etcT = 0;
volatile unsigned long isr_pre_ms = 0, isr_cur_ms = 0;
uint16_t Vcc = 0;

float ETCV_table[TBL_Y][TBL_X] = {
  {1200,1150,1100,1050,1000,950,900,850,800,750,700,650,600,550,500,450,400,350,300,250,200,150,100,50,0,-50,-100,-150,-200,-250,-300},  //nhiệt độ (độ C)
  //{84.2,99.4,117.2,138.2,163.0,192.2,226.7,267.4,315.4,371.9,438.7,517.3,610.2,719.6,848.7,1001.0,1180.5,1392.3,1642.1,1936.6,2284.1,2693.8,3177.1,3747.0,4419.2,5212.0,6147.0,7249.7,8550.2,10084.1,11893.1}
  {69,83,98,117,140,166,198,236,281,335,399,475,566,674,803,957,1140,1357,1617,1926,2295,2734,3256,3879,4621,5505,6558,7812,9306,11085,13205}  //điện trở (Ohm)
  };

void setup() {
  // put your setup code here, to run once:
  pinMode(pinSPD,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSPD), pulse_handler, FALLING);
  next_ms = serial_ms;
  Serial.begin(9600);
  Serial.println("UNO Ready");
}

void pulse_handler() {
  isr_pre_ms = isr_cur_ms;
  isr_cur_ms = micros();
}

void loop() {
  // put your main code here, to run repeatedly:
  cur_ms = millis();
  //loops_count++;
  if (cur_ms >= next_ms) {
    cal();
    printSerial();
    next_ms += serial_ms;
    //loops_count = 0;
  } else {}
}

void cal() {
    Vcc = readVcc();
int bgA0 = analogRead(pinBG);
    bg = map(bgA0,0,1023,0,1000);
float etcA1 = analogRead(pinETC);
int etc_mV = map(etcA1,0,1023,0,Vcc);
float etc_Ohm = etcR*etcA1/(1023.0-etcA1);
//float etc_Ohm = etcR*etc_mV/(Vcc-etc_mV);
    //Serial.println(etc_Ohm);
    if ((etc_Ohm >= ETCV_table[1][0]) && (etc_Ohm <= ETCV_table[1][TBL_X-1])) {
      int x = 0;
      for (x = 0; x < TBL_X; x++) {
        if (ETCV_table[1][x] >= etc_Ohm) {
          break;
        } else {}
      }
      etcT = map(etc_Ohm,ETCV_table[1][x-1],ETCV_table[1][x],ETCV_table[0][x-1],ETCV_table[0][x]);
    } else {
      etcT = 0;
    }  
    if ((micros()-isr_cur_ms) >= 1e6) {
      pulse_ms = 0;
    } else {
      pulse_ms = (isr_cur_ms - isr_pre_ms)*0.001;
    }
    rps = 1000/pulse_ms/teeth;
    rpm = 60*rps;  
}

void printSerial() {
    Serial.print("ms:");
    Serial.print(cur_ms/1000.0,1);
    //Serial.print("\tloops:");
    //Serial.print(loops_count);
    Serial.print("\tVcc:");
    Serial.print(Vcc);
    Serial.print("\tbg:");
    Serial.print(bg/10.0,1);
    Serial.print("%\tetcT:");
    Serial.print(etcT/10.0,1);
    Serial.print("\tspd_ms:");
    Serial.print(pulse_ms,3);
    Serial.print("\trps:");
    Serial.print(rps,3);
    Serial.print("\trpm:");
    Serial.print(rpm,3);
    Serial.println();
}

uint16_t readVcc() {
  uint16_t result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}
