#include <Wire.h>
#include <Adafruit_MCP4725.h>
Adafruit_MCP4725 dac;
const int N_MAX = 101;
const int OFFSET = 2048;
const int LED_PIN = 13;
double X[N_MAX];
int XD2[N_MAX];
int XD_standart[N_MAX];
int current_N = 50;
double current_A = 2000.0;
bool need_recalc = true;
int current_point = 0;
char active_cmd = '\0';
int incoming_value = 0;
int Okrugli(double x) {
int res_okr = 0;
if (x >= 0) {
double temp1 = x + 0.5;
res_okr = (int)temp1;
} else {
double temp2 = x - 0.5;
res_okr = (int)temp2;
}
return res_okr;
}
int Otbros(double x) {
int celoe = (int)x;
return celoe;
}
int Znak(double x) {
if (x > 0) return 1;
if (x < 0) return -1;
return 0;
}
void process_signal(int N, double A) {
digitalWrite(LED_PIN, LOW);
for (int i = 0; i < N_MAX; i++) {
X[i] = 0.0;
XD2[i] = 0;

50

XD_standart[i] = 0;
}
double h_step = 1.001 * N;
for (int i = 0; i <= N; i += 1) {
double ugol = 2 * PI * i / h_step - PI / N;
X[i] = A * sin(ugol);
XD2[i] = Okrugli(X[i]);
XD_standart[i] = XD2[i];
}
double K = (2.0 / N) * sin(PI / N) / (PI / N);
double SumSinId = 0.0;
double SumCosId = 0.0;
for (int j = 0; j <= N; j += 1) {
double ugol = (2 * j + 1) * PI / N;
SumSinId += X[j] * sin(ugol);
SumCosId += X[j] * cos(ugol);
}
double GarmIdeal = sqrt(SumSinId * SumSinId + SumCosId * SumCosId) * K;
Serial.println("==================================================");
Serial.print("ÏÀÐÀÌÅÒÐÛ ÑÈÍÒÅÇÀ: N = "); Serial.print(N);
Serial.print(" | A = "); Serial.println((int)A);
Serial.println("--------------------------------------------------");
calculate_and_print_error(N, GarmIdeal, K, XD_standart, "Standartnoe okruglenie");
double SumSinReal = 0.0;
double SumCosReal = 0.0;
for (int j = 0; j <= N; j += 1) {
double ugol = (2 * j + 1) * PI / N;
SumSinReal += XD_standart[j] * sin(ugol);
SumCosReal += XD_standart[j] * cos(ugol);
}
for (int i = 0; i <= N; i += 1) {
int old_val = XD2[i];
double u = (2 * i + 1) * PI / N;
double sin_u = sin(u);
double cos_u = cos(u);
double baseSin = SumSinReal - (old_val * sin_u);
double baseCos = SumCosReal - (old_val * cos_u);
int x_do = Okrugli(X[i]);
int x_db = Otbros(X[i]) + Znak(X[i]);
int x_dn = Otbros(X[i]);
double termVerhSin = baseSin + x_db * sin_u;
double termVerhCos = baseCos + x_db * cos_u;
double GarmVerh = sqrt(termVerhSin * termVerhSin + termVerhCos * termVerhCos) * K;

51
double ErrVerh = fabs(GarmVerh - GarmIdeal);
double termNizSin = baseSin + x_dn * sin_u;
double termNizCos = baseCos + x_dn * cos_u;
double GarmNiz = sqrt(termNizSin * termNizSin + termNizCos * termNizCos) * K;
double ErrNiz = fabs(GarmNiz - GarmIdeal);
double termTekSin = baseSin + x_do * sin_u;
double termTekCos = baseCos + x_do * cos_u;
double GarmTek = sqrt(termTekSin * termTekSin + termTekCos * termTekCos) * K;
double ErrTek = fabs(GarmTek - GarmIdeal);
int best_val = x_do;
double min_err = ErrTek;
if (ErrVerh < min_err) {
min_err = ErrVerh;
best_val = x_db;
}
if (ErrNiz < min_err) {
min_err = ErrNiz;
best_val = x_dn;
}
if (best_val != old_val) {
XD2[i] = best_val;
SumSinReal = baseSin + best_val * sin_u;
SumCosReal = baseCos + best_val * cos_u;
}
}
calculate_and_print_error(N, GarmIdeal, K, XD2, "Adaptivnoe kvantovanie");
Serial.print("Raschet okonchen dlya N = ");
Serial.println(N);
Serial.print("Idealnaya garmonika: ");
Serial.println(GarmIdeal, 4);
Serial.println("==================================================");
Serial.println();
digitalWrite(LED_PIN, HIGH);
}
void setup() {
pinMode(LED_PIN, OUTPUT);
digitalWrite(LED_PIN, LOW);
Serial.begin(9600);
dac.begin(0x62);
Wire.setClock(400000);

52

Serial.println("--- ÑÈÑÒÅÌÀ ÃÎÒÎÂÀ Ê ÐÓ×ÍÎÌÓ ÂÂÎÄÓ ÊÎÌÀÍÄ ---");
}
void loop() {
if (Serial.available() > 0) {
char ch = Serial.read();
if (ch == 'n' || ch == 'N' || ch == 'a' || ch == 'A') {
active_cmd = ch;
incoming_value = 0;
}
else if (ch >= '0' && ch <= '9') {
int digit = ch - '0';
incoming_value = (incoming_value * 10) + digit;
}
else if (ch == '\n' || ch == '\r' || ch == ' ' || ch == ';') {
if (active_cmd == 'n' || active_cmd == 'N') {
if (incoming_value >= 8 && incoming_value <= 100) {
current_N = incoming_value;
need_recalc = true;
}
}
else if (active_cmd == 'a' || active_cmd == 'A') {
if (incoming_value >= 100 && incoming_value <= 2047) {
current_A = (double)incoming_value;
need_recalc = true;
}
}
active_cmd = '\0';
}
}
if (need_recalc) {
process_signal(current_N, current_A);
current_point = 0;
need_recalc = false;
}
int out_val = XD2[current_point] + OFFSET;
if (out_val > 4095) out_val = 4095;
if (out_val < 0) out_val = 0;
dac.setVoltage(out_val, false);
current_point += 1;
if (current_point > current_N) {
current_point = 0;
}
delayMicroseconds(1500);
}

53

void calculate_and_print_error(int N, double GarmIdeal, double K, int* signal_array, const char*
mode_name) {
double SumSinReal = 0.0;
double SumCosReal = 0.0;
for (int j = 0; j <= N; j += 1) {
double ugol = (2 * j + 1) * M_PI / (double)N;
SumSinReal += (double)signal_array[j] * sin(ugol);
SumCosReal += (double)signal_array[j] * cos(ugol);
}
double GarmReal = sqrt(SumSinReal * SumSinReal + SumCosReal * SumCosReal) * K;
double relative_error = (fabs(GarmIdeal - GarmReal) / GarmIdeal) * 100.0;
Serial.print("Rezhim: ");
Serial.print(mode_name);
Serial.print(" | Garmonika 1: ");
Serial.print(GarmReal, 4);
Serial.print(" | Otnositel'naya Pogreshnost': ");
Serial.print(relative_error, 5);
Serial.println("%");
}