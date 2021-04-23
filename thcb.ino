#define pinSPD 2
#define pinBG A0
#define pinETC A1
#define teeth 1

unsigned long loops_count = 0;
unsigned long pre_ms = 0, cur_ms = 0;
unsigned long bg = 0, etcV = 0;
float pulse_ms = 0, rps = 0, rpm = 0, etcT = 0;
volatile unsigned long isr_pre_ms = 0, isr_cur_ms = 0;

int ETCV_table[2][10] = {
  {120,110,100,90,  80,  70,  60,  50,  40,  30},
  {300,400,500,1000,1500,2000,2500,3000,3500,4000}
  };

void setup() {
  // put your setup code here, to run once:
  pinMode(pinSPD,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pinSPD), pulse_handler, RISING);
  pre_ms = 1000;
  Serial.begin(9600);
  Serial.println("UNO Ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  cur_ms = millis();
  loops_count++;
  if (cur_ms >= pre_ms) {
    cal();
    printSerial();
    pre_ms += 1000;
    loops_count = 0;
  } else {}
}

void pulse_handler() {
  isr_pre_ms = isr_cur_ms;
  isr_cur_ms = micros();
}

void cal() {
int bgA0 = analogRead(pinBG);
    bg = map(bgA0,0,1023,0,100);
int etc = analogRead(pinETC);
    etcV = map(etc,0,1023,0,5000);
byte x = 1;
    etcT = map(etcV,ETCV_table[x-1][1],ETCV_table[x][1],ETCV_table[x-1][0],ETCV_table[x][0]);
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
    Serial.print("%\tspd_ms:");
    Serial.print(pulse_ms,3);
    Serial.print("\trps:");
    Serial.print(rps,3);
    Serial.print("\trpm:");
    Serial.print(rpm,3);
    Serial.println();
}
