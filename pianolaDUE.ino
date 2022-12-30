int led[14] = {38, 36, 34, 32, 30,  28, 26, 40, 42, 44, 46, 48,  50, 52};
int piezo[14] = {A14, A13, A12, A11, A10, A9, A8, A7, A6, A5, A4, A3, A2, A1};
int nota[14] = {1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7};
int altezza;
long tempo[14];

int valore[14], maxVal[14], Max;

bool l[14], s[14];

int tem = 50;

int valMin = 300;   //valore minimo per considerare la nota

void setup() {
  Serial1.begin(9600);   //comunicazione tra arduini
  Serial.begin(9600);
  Serial1.setTimeout(30);

  for (int i = 0; i < 14; i++) {
    pinMode(led[i], OUTPUT);
    pinMode(piezo[i], INPUT);
  }
}

void loop() {
  for (int i = 0; i < 14; i++) {
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
        if (led[i] < 39) {
          altezza = 4;
        } else {
          altezza = 5;
        }
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

void playMIDINote (int note, int vel) {        //invia nota ad arduino
  Serial1.println("1");
  Serial1.println(note);
  Serial1.println(altezza);
  Serial1.println(vel);
}
