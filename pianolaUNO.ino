/*
   Comando da mandare:

   Evento                                   Status byte + channel         byte2                   byte3
   Note OFF                                 0x8           0 - F           Note Number             Velocity
   Note ON                                  0x9           0 - F           Note Number             Velocity
   Polyphonic Key Pressure (aftertouch)     0xA           0 - F           Note Number             Pressure
   Control Change                           0xB           0 - F           Controller Number       Value
   Programa Change                          0xC           0 - F           Program Number
   Channel Pressure (aftertouch)            0xD           0 - F           Pressure
   Pitch Wheel                              0xE           0 - F           LSB                     MSB
*/

//0 = C-1     127 = G9
int C[11] = {0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120};
int D[11] = {2, 14, 26, 38, 50, 62, 74, 86, 98, 110, 122};
int E[11] = {4, 16, 28, 40, 52, 64, 76, 88, 100, 112, 124};
int F[11] = {5, 17, 29, 41, 53, 65, 77, 89, 101, 113, 125};
int G[11] = {7, 19, 31, 43, 55, 67, 79, 91, 103, 115, 127};
int A[10] = {9, 21, 33, 45, 57, 69, 81, 93, 105, 117};
int B[10] = {11, 23, 35, 47, 59, 71, 83, 95, 107, 119};

int led[7] = {35, 33, 31, 29, 27, 25, 23};  //pin dei led
int piezo[7] = {A9, A10, A11, A12, A13, A14, A15};  //pin dei tasti
int nota[7] = {7, 6, 5, 4, 3, 2, 1};  //note (1 = do, 7 = si)
int alt = 3, altezza;  //altezza della tastiera (3 --> do3 fino a si3)
long tempo[7];  //tempo passato da quando suonata la nota

int valore[7], maxVal[7], Max;

bool l[7], s[7];

int tem = 50;

int valMin = 300;   //valore minimo per considerare la nota

int notaA, altA, velA;

void setup() {
  Serial.begin(31250);    //velocita della comunicazione midi 31250
  Serial1.begin(9600);   //comunicazione tra arduini
  Serial1.setTimeout(25);

  for (int i = 0; i < 7; i++) {
    pinMode(led[i], OUTPUT);
    pinMode(piezo[i], INPUT);
  }

  //interruttori per modifica nota (# o b)
  pinMode(38, INPUT);
  pinMode(40, INPUT);
  pinMode(42, INPUT);
  pinMode(44, INPUT);
  pinMode(46, INPUT);
}

void loop() {
  if (Serial1.available() > 0 and Serial1.parseInt() == 1) {
    notaA = Serial1.parseInt();
    altA = Serial1.parseInt();
    velA = Serial1.parseInt();
    altezza = altA;
    playMIDINote(notaA, velA);
  }

  altezza = alt;

  for (int i = 0; i < 7; i++) {
    valore[i] = analogRead(piezo[i]);  //leggi valori piezo

    if (valore[i] > Max) {
      Max = valore[i];
    }

    if (millis() - tempo[i] > tem) {
      if (valore[i] > maxVal[i]) {  //leggere valore max
        maxVal[i] = valore[i];
      } else {
        s[i] = true;
      }

      //ha raggiunto il suo maxVal & circa uguale di valore alla nota più forte di tutte
      if (s[i] and maxVal[i] > valMin and maxVal[i] > Max - 15) {
        playMIDINote(nota[i], maxVal[i]);       //suona nota
        digitalWrite(led[i], HIGH);
        tempo[i] = millis();
        l[i] = true;                            //stato nota (true = suonato)
        s[i] = false;
        maxVal[i] = 0;

      }
    }

    if (millis() - tempo[i] > tem and l[i]) {    //dopo che è stata suonata la nota, spegnila
      digitalWrite(led[i], LOW);
      l[i] = false;
    }
  }
  Max = valMin;
}

void playMIDINote (int note, int vel) {        //invia nota via midi
  if (vel != 0) {   //adegua velocità nota a standard midi
    vel = map(vel, valMin, 1023, 0, 127);
  }
  Serial.write(0x90);
  Serial.write(modificaNota(note));   //scegli nota in base a tasto e interruttori
  Serial.write(vel);
}

int modificaNota(int no) { //modifica nota (# o b)
  int valoreNota;

  switch (no) { //che nota bisogna modificare?
    case 1:
      if (!digitalRead(46)) {                 //do#
        valoreNota = C[altezza + 1] + 1;
      } else {
        valoreNota = C[altezza + 1];
      }
      break;
    case 2:
      valoreNota = D[altezza + 1];
      break;
    case 3:
      if (!digitalRead(44)) {                 //mib
        valoreNota = E[altezza + 1] - 1;
      } else {
        valoreNota = E[altezza + 1];
      }
      break;
    case 4:
      if (!digitalRead(42)) {                 //fa#
        valoreNota = F[altezza + 1] + 1;
      } else {
        valoreNota = F[altezza + 1];
      }
      break;
    case 5:
      valoreNota = G[altezza + 1];
      break;
    case 6:
      if (!digitalRead(40)) {                 //lab
        valoreNota = A[altezza + 1] - 1;
      } else {
        valoreNota = A[altezza + 1];
      }
      break;
    case 7:
      if (!digitalRead(38)) {                 //sib
        valoreNota = B[altezza + 1] - 1;
      } else {
        valoreNota = B[altezza + 1];
      }
      break;
  }

  return valoreNota;
}
