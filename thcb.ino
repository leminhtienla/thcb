#define pinSPD 2  //khai báo chân cảm biến tốc độ
#define pinBG A0  //khai báo chân cảm biến bướm ga
#define pinETC A1 //khai báo chân cảm biến nhiệt độ động cơ
#define teeth 1   //khai báo số răng cảm biến tốc độ
#define TBL_X 31
#define TBL_Y 2

unsigned long loops_count = 0;  //biến đếm số vòng lặp loops/s
unsigned long next_ms = 0, cur_ms = 0; //
unsigned int bg = 0, etc_mV = 0;
float pulse_ms = 0, rps = 0, rpm = 0, etcT = 0;
volatile unsigned long isr_pre_ms = 0, isr_cur_ms = 0;

int ETCV_table[TBL_Y][TBL_X] = {
  {1200,1150,1100,1050,1000,950,900,850,800,750,700,650,600,550,500,450,400,350,300,250,200,150,100,50,0,-50,-100,-150,-200,-250,-300},  //nhiệt độ (độ C)
  {298,351,412,482,563,656,760,878,1009,1154,1311,1481,1662,1852,2049,2249,2450,2649,2842,3028,3204,3368,3519,3657,3782,3893,3991,4078,4154,4219,4276}  //điện áp (mmV)
  };

void setup() {
  // put your setup code here, to run once:
  pinMode(pinSPD,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSPD), pulse_handler, FALLING);
  next_ms = 1000;
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
  loops_count++;
  if (cur_ms >= next_ms) {
    cal();
    printSerial();
    next_ms += 1000;
    loops_count = 0;
  } else {}
}

void cal() {
int bgA0 = analogRead(pinBG);
    bg = map(bgA0,0,1023,0,1000);
int etcA1 = analogRead(pinETC);
    etc_mV = map(etcA1,0,1023,0,5000);
    if ((etc_mV >= ETCV_table[1][0]) && (etc_mV <= ETCV_table[1][TBL_X-1])) {
      int x = 0;
      for (x = 0; x < TBL_X; x++) {
        if (ETCV_table[1][x] >= etc_mV) {
          break;
        } else {}
      }
      etcT = map(etc_mV,ETCV_table[1][x-1],ETCV_table[1][x],ETCV_table[0][x-1],ETCV_table[0][x]);
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
    Serial.print(cur_ms/1000);
    Serial.print("\tloops:");
    Serial.print(loops_count);
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
