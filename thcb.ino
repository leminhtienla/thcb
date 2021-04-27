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
  {120,115,110,105,100,95,90,85,80,75,70,65,60,55,50,45,40,35,30,25,20,15,10,5,0,-5,-10,-15,-20,-25,-30},  //nhiệt độ (độ C)
  {324,381,448,524,612,713,826,954,1097,1254,1425,1610,1807,2013,2227,2445,2663,2879,3089,3291,3482,3661,3825,3975,4110,4231,4338,4433,4515,4586,4648}  //điện áp (mmV)
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
    bg = map(bgA0,0,1023,0,100);
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
    Serial.print(bg);
    Serial.print("%\tetcT:");
    Serial.print(etcT,2);
    Serial.print("\tspd_ms:");
    Serial.print(pulse_ms,3);
    Serial.print("\trps:");
    Serial.print(rps,3);
    Serial.print("\trpm:");
    Serial.print(rpm,3);
    Serial.println();
}
