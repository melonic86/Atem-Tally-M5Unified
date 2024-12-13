#include <M5Unified.h>
#include <WiFi.h>
#include <SkaarhojPgmspace.h>
#include <ATEMbase.h>
#include <ATEMstd.h>

// Définir l'adresse IP de l'ATEM switcher
IPAddress switcherIp(192, 168, 0, 101);

// Votre SSID et mot de passe WiFi
const char* ssid = "ORCAHub";
const char* password = "Production21";

// Couleurs personnalisées (en RGB)
#define RED     0xF800  // Rouge pur
#define GREEN   0x07E0  // Vert pur
#define BLUE    0x001F  // Bleu pur
#define BLACK   0x0000  // Noir
#define WHITE   0xFFFF  // Blanc
#define YELLOW  0xFFE0  // Jaune
#define CYAN    0x07FF  // Cyan
#define MAGENTA 0xF81F  // Magenta

ATEMstd AtemSwitcher;

int cameraNumber = 1;

int previewTallyPrevious = 1;
int programTallyPrevious = 1;
int cameraNumberPrevious = cameraNumber;
int screenRotation = 1; // Rotation initiale

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  // Initialisation
  M5.begin();
  M5.Lcd.setRotation(screenRotation);

  AtemSwitcher.begin(switcherIp);
  AtemSwitcher.serialOutput(0x80);
  AtemSwitcher.connect();
}

void updateScreenRotation() {
  // Lire les données de l'accéléromètre
  float accX, accY, accZ;
  M5.Imu.getAccel(&accX, &accY, &accZ);

  // Déterminer la nouvelle rotation
  int newRotation = screenRotation;
  if (abs(accX) > abs(accY)) { // Paysage
    newRotation = (accX > 0) ? 1 : 3;
  } else { // Portrait
    newRotation = (accY > 0) ? 0 : 2;
  }

  // Si la rotation a changé, mettre à jour l'écran
  if (newRotation != screenRotation) {
    screenRotation = newRotation;
    M5.Lcd.setRotation(screenRotation);
  }
}

void redrawScreen(int camera, int programTally, int previewTally) {
  if (programTally && !previewTally) { // seulement program
    M5.Lcd.fillScreen(RED);  // Fond rouge
  } else if (programTally && previewTally) { // program ET preview
    M5.Lcd.fillScreen(RED);  // Fond rouge
  } else if (previewTally && !programTally) { // seulement preview
    M5.Lcd.fillScreen(GREEN);  // Fond vert
  } else { // aucun
    M5.Lcd.fillScreen(BLACK);  // Fond noir
  }
  M5.Lcd.setTextColor(WHITE);  // Texte blanc
  M5.Lcd.setTextSize(8);
  M5.Lcd.setCursor(M5.Lcd.width() / 2 - 20, M5.Lcd.height() / 2 - 25); // Centrer le texte
  M5.Lcd.print(camera);
}

void loop() {
  // Mise à jour des boutons
  M5.update();

  // Bouton A : changement de camera
  if (M5.BtnA.wasPressed()) {  // Pression courte
    Serial.println("Changing camera number");
    cameraNumber = (cameraNumber % 4) + 1;
    Serial.printf("New camera number: %d\n", cameraNumber);
  }


  // Bouton B : changer le program
  if (M5.BtnB.pressedFor(1000)) {
    Serial.println("Switching program input");
    AtemSwitcher.changeProgramInput(cameraNumber);
    redrawScreen(cameraNumber, programTallyPrevious, previewTallyPrevious);
  }

  // Bouton B : changer la rotation
  if (M5.BtnB.wasPressed()) {  // Pression courte
   updateScreenRotation(); // Mettre à jour la rotation uniquement si BtnA est pressé
    redrawScreen(cameraNumber, programTallyPrevious, previewTallyPrevious);
    
  }

  // Vérifier les paquets et maintenir la connexion
  AtemSwitcher.runLoop();

  int programTally = AtemSwitcher.getProgramTally(cameraNumber);
  int previewTally = AtemSwitcher.getPreviewTally(cameraNumber);

  // Mise à jour de l'écran uniquement si nécessaire
  if ((cameraNumber != cameraNumberPrevious) || 
      (programTallyPrevious != programTally) || 
      (previewTallyPrevious != previewTally)) {
    redrawScreen(cameraNumber, programTally, previewTally);
  }

  programTallyPrevious = programTally;
  previewTallyPrevious = previewTally;
  cameraNumberPrevious = cameraNumber;
}
