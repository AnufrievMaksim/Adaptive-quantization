#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#define PI 3.141592653589793
int okrugli(double x) {
if (x >= 0) {
return (int)(x + 0.5);
}
else {
return (int)(x - 0.5);
}
}
int otbros(double x) {
return (int)x;
}
int znak(double x) {
if (x > 0) return 1;
if (x < 0) return -1;
return 0;
}
double pervaya_garmonika_int(int n, int* signal, double k) {
double sum_sin = 0.0;
double sum_cos = 0.0;
int i;
double ugol;
for (i = 0; i <= n; i++) {
ugol = (2.0 * i + 1.0) * PI / n;
sum_sin = sum_sin + (double)signal[i] * sin(ugol);
sum_cos = sum_cos + (double)signal[i] * cos(ugol);
}
return sqrt(sum_sin * sum_sin + sum_cos * sum_cos) * k;
}
double pervaya_garmonika_double(int n, double* signal, double k) {
double sum_sin = 0.0;
double sum_cos = 0.0;
int i;
double ugol;
for (i = 0; i <= n; i++) {
ugol = (2.0 * i + 1.0) * PI / n;
sum_sin = sum_sin + signal[i] * sin(ugol);

62
sum_cos = sum_cos + signal[i] * cos(ugol);
}
return sqrt(sum_sin * sum_sin + sum_cos * sum_cos) * k;
}
double oshibka_skz(int n, double* ideal, int* quant) {
double sum_kv_ideal = 0.0;
double sum_kv_quant = 0.0;
double rms_ideal, rms_quant;
int i;
for (i = 0; i <= n; i++) {
sum_kv_ideal = sum_kv_ideal + (ideal[i] * ideal[i]);
sum_kv_quant = sum_kv_quant + ((double)quant[i] * (double)quant[i]);
}
rms_ideal = sqrt(sum_kv_ideal / (n + 1));
rms_quant = sqrt(sum_kv_quant / (n + 1));
return (fabs(rms_ideal - rms_quant) / rms_ideal) * 100.0;
}
int main() {
int n_values[] = { 2, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
int n_count = 11;
double a_values[] = { 200.0, 500.0, 1000.0, 1500.0, 2000.0 };
int a_count = 5;
double ideal[101];
int vniz[101];
int vverh[101];
int bliz[101];
int adapt[101];
int n, i, j;
int idx_n, idx_a;
double ugol;
double k;
double garm_ideal;
double sum_sin_real, sum_cos_real;
double base_sin, base_cos;
double u, sin_u, cos_u;
double garm_tek, garm_verh, garm_niz;
double oshibka_tek, oshibka_verh, oshibka_niz;
int old_val, x_bliz, x_vverh, x_vniz, best_val;
double garm_vniz, garm_vverh, garm_bliz, garm_adapt;
double osh_garm_vniz, osh_garm_vverh, osh_garm_bliz, osh_garm_adapt;

63

double osh_skz_vniz, osh_skz_vverh, osh_skz_bliz, osh_skz_adapt;
double snizhenie;
FILE* f = fopen("rezultat.txt", "w");
if (f == NULL) {
printf("Oshibka: ne mogu sozdat fail rezultat.txt\n");
return 1;
}
fprintf(f, "N\tA\tMetod\tPogr_1_garm(%%)\tPogr_SKZ(%%)\tSnizhenie(raz)\n");
for (idx_n = 0; idx_n < n_count; idx_n++) {
n = n_values[idx_n];
for (idx_a = 0; idx_a < a_count; idx_a++) {
double A = a_values[idx_a];
printf("Schitaju dlja N = %d, A = %.0f ...\n", n, A);
for (i = 0; i <= n; i++) {
ugol = 2.0 * PI * i / (1.001 * n) - PI / n;
ideal[i] = A * sin(ugol);
}
for (i = 0; i <= n; i++) {
vniz[i] = otbros(ideal[i]);
vverh[i] = otbros(ideal[i]) + znak(ideal[i]);
bliz[i] = okrugli(ideal[i]);
adapt[i] = bliz[i];
}
k = (2.0 / n) * sin(PI / n) / (PI / n);
garm_ideal = pervaya_garmonika_double(n, ideal, k);
sum_sin_real = 0.0;
sum_cos_real = 0.0;
for (j = 0; j <= n; j++) {
ugol = (2.0 * j + 1.0) * PI / n;
sum_sin_real = sum_sin_real + (double)adapt[j] * sin(ugol);
sum_cos_real = sum_cos_real + (double)adapt[j] * cos(ugol);
}
for (i = 0; i <= n; i++) {
old_val = adapt[i];
u = (2.0 * i + 1.0) * PI / n;
sin_u = sin(u);
cos_u = cos(u);
base_sin = sum_sin_real - (double)old_val * sin_u;
base_cos = sum_cos_real - (double)old_val * cos_u;

64

x_bliz = okrugli(ideal[i]);
x_vniz = otbros(ideal[i]);
x_vverh = otbros(ideal[i]) + znak(ideal[i]);
garm_tek = sqrt((base_sin + (double)x_bliz * sin_u) * (base_sin + (double)x_bliz * sin_u)
+
(base_cos + (double)x_bliz * cos_u) * (base_cos + (double)x_bliz * cos_u)) * k;
oshibka_tek = fabs(garm_tek - garm_ideal);
garm_verh = sqrt((base_sin + (double)x_vverh * sin_u) * (base_sin + (double)x_vverh *
sin_u) +
(base_cos + (double)x_vverh * cos_u) * (base_cos + (double)x_vverh * cos_u)) * k;
oshibka_verh = fabs(garm_verh - garm_ideal);
garm_niz = sqrt((base_sin + (double)x_vniz * sin_u) * (base_sin + (double)x_vniz *
sin_u) +
(base_cos + (double)x_vniz * cos_u) * (base_cos + (double)x_vniz * cos_u)) * k;
oshibka_niz = fabs(garm_niz - garm_ideal);
best_val = x_bliz;
if (oshibka_verh < oshibka_tek && oshibka_verh < oshibka_niz) {
best_val = x_vverh;
}
if (oshibka_niz < oshibka_tek && oshibka_niz < oshibka_verh) {
best_val = x_vniz;
}
if (best_val != old_val) {
adapt[i] = best_val;
sum_sin_real = base_sin + (double)best_val * sin_u;
sum_cos_real = base_cos + (double)best_val * cos_u;
}
}
garm_vniz = pervaya_garmonika_int(n, vniz, k);
garm_vverh = pervaya_garmonika_int(n, vverh, k);
garm_bliz = pervaya_garmonika_int(n, bliz, k);
garm_adapt = pervaya_garmonika_int(n, adapt, k);
osh_garm_vniz = fabs(garm_vniz - garm_ideal) / garm_ideal * 100.0;
osh_garm_vverh = fabs(garm_vverh - garm_ideal) / garm_ideal * 100.0;
osh_garm_bliz = fabs(garm_bliz - garm_ideal) / garm_ideal * 100.0;
osh_garm_adapt = fabs(garm_adapt - garm_ideal) / garm_ideal * 100.0;
osh_skz_vniz = oshibka_skz(n, ideal, vniz);
osh_skz_vverh = oshibka_skz(n, ideal, vverh);
osh_skz_bliz = oshibka_skz(n, ideal, bliz);
osh_skz_adapt = oshibka_skz(n, ideal, adapt);
fprintf(f, "%d\t%.0f\tvniz\t%.6f\t%.6f\t---\n",
n, A, osh_garm_vniz, osh_skz_vniz);
fprintf(f, "%d\t%.0f\tvverh\t%.6f\t%.6f\t---\n",

65
n, A, osh_garm_vverh, osh_skz_vverh);
fprintf(f, "%d\t%.0f\tbliz\t%.6f\t%.6f\t---\n",
n, A, osh_garm_bliz, osh_skz_bliz);
if (osh_garm_adapt < 0.000001) {
fprintf(f, "%d\t%.0f\tadapt\t%.6f\t%.6f\t>1000\n",
n, A, osh_garm_adapt, osh_skz_adapt);
}
else {
snizhenie = osh_garm_bliz / osh_garm_adapt;
fprintf(f, "%d\t%.0f\tadapt\t%.6f\t%.6f\t%.1f\n",
n, A, osh_garm_adapt, osh_skz_adapt, snizhenie);
}
}
}
fclose(f);
return 0;
}