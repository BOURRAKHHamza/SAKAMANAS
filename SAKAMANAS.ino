#define BLYNK_PRINT Serial

// ------------------- BLYNK -------------------
#define BLYNK_TEMPLATE_ID "TMPL2XgrKAE90"
#define BLYNK_TEMPLATE_NAME "Température"
#define BLYNK_AUTH_TOKEN "kyrNbBQf20JLtSy6vV0vyhG1OIi8CR24"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// ------------------- DS18B20 -------------------
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Adresses des 3 capteurs (détectées automatiquement au démarrage)
DeviceAddress sensor1, sensor2, sensor3;

// ------------------- LCD -------------------
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------------------- Timer -------------------
BlynkTimer timer;

// ------------------- WIFI MULTI-RÉSEAUX -------------------
// Liste des réseaux WiFi avec ordre de priorité
struct WiFiNetwork {
  const char* ssid;
  const char* password;
};

WiFiNetwork wifiNetworks[] = {
  {"Dept_Biologie3", "Dept@Biologie2020"},  // Réseau prioritaire 1
  {"La_Fibre_dOrange_247C", "AA7K42KQ4HEGSKN7ZC"},                       // Réseau prioritaire 2
  {"inwi Home 4G 58Z2G8", "1922833370"}                        // Réseau prioritaire 3
};

const int numNetworks = 3;  // Nombre de réseaux configurés
int connectedNetworkIndex = -1;  // Index du réseau connecté (-1 = aucun)

// ------------------- ALARME -------------------
#define BUZZER_PIN 18
#define BUTTON_PIN 19
bool alarmeActive = true; // état du buzzer (contrôlé par bouton physique OU Blynk)

// ------------------- AFFICHAGE LCD -------------------
int affichageActuel = 0; // 0=Wild, 1=Mongolian, 2=Fry

// Variables globales pour les températures
float temp1 = 0.0;
float temp2 = 0.0;
float temp3 = 0.0;
bool alerteDetectee = false;

// ------------------- FONCTION BLYNK : CONTRÔLE BUZZER À DISTANCE -------------------
// Cette fonction est appelée quand vous appuyez sur le bouton V4 dans Blynk
BLYNK_WRITE(V4) {
  int boutonBlynk = param.asInt();  // 0 = OFF, 1 = ON
  
  if(boutonBlynk == 1) {
    // Bouton Blynk activé → Alarme ACTIVE
    alarmeActive = true;
    Serial.println("📱 Blynk: Alarme ACTIVÉE à distance");
    
    // Mettre à jour l'affichage LCD
    lcd.setCursor(0, 1);
    lcd.print("Alarm: Blynk ON ");
    delay(1000);
    
  } else {
    // Bouton Blynk désactivé → Alarme DÉSACTIVÉE
    alarmeActive = false;
    digitalWrite(BUZZER_PIN, LOW);  // Arrêt immédiat du buzzer
    Serial.println("📱 Blynk: Alarme DÉSACTIVÉE à distance");
    
    // Mettre à jour l'affichage LCD
    lcd.setCursor(0, 1);
    lcd.print("Alarm: Blynk OFF");
    delay(1000);
  }
}

// ------------------- FONCTION D'ENVOI TEMPERATURE -------------------
void sendTemperature() {
  sensors.requestTemperatures();
  
  // Lecture des 3 températures
  temp1 = sensors.getTempC(sensor1);  // Wild strain
  temp2 = sensors.getTempC(sensor2);  // Mongolian strain
  temp3 = sensors.getTempC(sensor3);  // Fry
  
  // Vérification erreurs
  if(temp1 == DEVICE_DISCONNECTED_C) temp1 = 0.0;
  if(temp2 == DEVICE_DISCONNECTED_C) temp2 = 0.0;
  if(temp3 == DEVICE_DISCONNECTED_C) temp3 = 0.0;
  
  // Affichage Serial
  Serial.print("🌡 Wild: ");
  Serial.print(temp1, 1);
  Serial.print("°C | Mongolian: ");
  Serial.print(temp2, 1);
  Serial.print("°C | Fry: ");
  Serial.print(temp3, 1);
  Serial.print("°C | Alarme: ");
  Serial.println(alarmeActive ? "ACTIVE" : "DÉSACTIVÉE");
  
  // ----- Blynk -----
  Blynk.virtualWrite(V1, temp1);  // Wild → V1
  Blynk.virtualWrite(V2, temp2);  // Mongolian → V2
  Blynk.virtualWrite(V3, temp3);  // Fry → V3
  
  // Envoyer l'état de l'alarme à Blynk (synchronisation)
  Blynk.virtualWrite(V4, alarmeActive ? 1 : 0);
  
  // ----- LCD - LIGNE 1 : Affichage alterné des températures -----
  lcd.clear();
  lcd.setCursor(0, 0);
  
  switch(affichageActuel) {
    case 0:  // Wild strain
      lcd.print("Wild: ");
      lcd.print(temp1, 1);
      lcd.print((char)223);  // symbole degré
      lcd.print("C");
      break;
    case 1:  // Mongolian strain
      lcd.print("Mongolian: ");
      lcd.print(temp2, 1);
      lcd.print((char)223);
      lcd.print("C");
      break;
    case 2:  // Fry
      lcd.print("Fry: ");
      lcd.print(temp3, 1);
      lcd.print((char)223);
      lcd.print("C");
      break;
  }
  
  // Alterner l'affichage : 0 → 1 → 2 → 0 ...
  affichageActuel = (affichageActuel + 1) % 3;
  
  // ----- LCD - LIGNE 2 : Alerte ou OK -----
  lcd.setCursor(0, 1);
  
  alerteDetectee = false;
  
  // Vérifier chaque aquarium
  if(temp1 > 0 && (temp1 < 20 || temp1 > 28)) {
    lcd.print("ALERTE Wild !");
    alerteDetectee = true;
  } 
  else if(temp2 > 0 && (temp2 < 20 || temp2 > 28)) {
    lcd.print("ALERTE Mongol!");
    alerteDetectee = true;
  } 
  else if(temp3 > 0 && (temp3 < 20 || temp3 > 28)) {
    lcd.print("ALERTE Fry !");
    alerteDetectee = true;
  }
  
  // Si pas d'alerte, afficher l'état de l'alarme et le WiFi
  if(!alerteDetectee) {
    lcd.print("Alarm:");
    lcd.print(alarmeActive ? "ON" : "OFF");
    
    // Afficher le numéro du réseau WiFi si connecté
    if(connectedNetworkIndex >= 0) {
      lcd.print(" W");
      lcd.print(connectedNetworkIndex + 1);
    }
  }
}

// ------------------- FONCTION CONNEXION WIFI MULTI-RÉSEAUX -------------------
void connectToWiFi() {
  Serial.println("\n═══════════════════════════════════");
  Serial.println("Recherche réseau WiFi disponible...");
  Serial.println("═══════════════════════════════════");
  
  for (int i = 0; i < numNetworks; i++) {
    Serial.print("\n🔍 Tentative ");
    Serial.print(i + 1);
    Serial.print("/");
    Serial.print(numNetworks);
    Serial.print(": ");
    Serial.println(wifiNetworks[i].ssid);
    
    // Affichage LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Reseau ");
    lcd.print(i + 1);
    lcd.setCursor(0, 1);
    lcd.print("Connexion...");
    
    // Tentative de connexion
    WiFi.begin(wifiNetworks[i].ssid, wifiNetworks[i].password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    // Vérifier si connecté
    if (WiFi.status() == WL_CONNECTED) {
      connectedNetworkIndex = i;
      
      Serial.println("\n✅ CONNEXION RÉUSSIE!");
      Serial.print("📶 Réseau: ");
      Serial.println(wifiNetworks[i].ssid);
      Serial.print("📡 IP: ");
      Serial.println(WiFi.localIP());
      Serial.print("📊 Signal: ");
      Serial.print(WiFi.RSSI());
      Serial.println(" dBm");
      
      // Affichage LCD succès
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi Reseau ");
      lcd.print(i + 1);
      lcd.setCursor(0, 1);
      lcd.print("Connecte!");
      delay(2000);
      
      return;  // Sortir de la fonction, connexion réussie
    }
    
    // Échec de connexion à ce réseau
    Serial.println(" ❌ Échec");
    WiFi.disconnect();
    delay(1000);
  }
  
  // Aucun réseau n'a fonctionné
  Serial.println("\n❌ ERREUR: Aucun réseau WiFi disponible");
  Serial.println("Le système continuera en mode LOCAL (sans Blynk)");
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi: AUCUN");
  lcd.setCursor(0, 1);
  lcd.print("Mode local");
  delay(2000);
}

// ------------------- SETUP -------------------
void setup() {
  Serial.begin(115200);
  
  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SAKAMANAS V3.0");
  lcd.setCursor(0, 1);
  lcd.print("Blynk Control");
  delay(2000);
  
  // DS18B20
  sensors.begin();
  
  // Détection et affichage du nombre de capteurs
  int nbCapteurs = sensors.getDeviceCount();
  Serial.print("Capteurs détectés: ");
  Serial.println(nbCapteurs);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Capteurs: ");
  lcd.print(nbCapteurs);
  delay(2000);
  
  if(nbCapteurs < 3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ERREUR!");
    lcd.setCursor(0, 1);
    lcd.print("Besoin 3 sondes");
    Serial.println("❌ ERREUR: Moins de 3 capteurs détectés!");
    while(1) { delay(1000); }  // Arrêt du système
  }
  
  // Récupération des adresses des 3 capteurs
  sensors.getAddress(sensor1, 0);
  sensors.getAddress(sensor2, 1);
  sensors.getAddress(sensor3, 2);
  
  // Configuration résolution 12 bits (0.0625°C de précision)
  sensors.setResolution(sensor1, 12);
  sensors.setResolution(sensor2, 12);
  sensors.setResolution(sensor3, 12);
  
  Serial.println("✅ 3 capteurs configurés avec succès!");
  
  // Affichage des adresses (optionnel, pour debug)
  Serial.print("Wild (sensor1): ");
  for(int i = 0; i < 8; i++) {
    Serial.print(sensor1[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  Serial.print("Mongolian (sensor2): ");
  for(int i = 0; i < 8; i++) {
    Serial.print(sensor2[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  Serial.print("Fry (sensor3): ");
  for(int i = 0; i < 8; i++) {
    Serial.print(sensor3[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // GPIO
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // bouton avec pull-up interne
  digitalWrite(BUZZER_PIN, LOW);
  
  // Test bouton
  Serial.print("Test bouton GPIO19: ");
  Serial.println(digitalRead(BUTTON_PIN) == HIGH ? "OK (relâché)" : "Appuyé");
  
  // Connexion WiFi multi-réseaux
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connexion WiFi");
  
  connectToWiFi();
  
  // Connexion Blynk
  if(connectedNetworkIndex >= 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connexion Blynk");
    
    Blynk.config(BLYNK_AUTH_TOKEN);
    Blynk.connect();
    
    int attempts = 0;
    while (!Blynk.connected() && attempts < 10) {
      delay(500);
      lcd.print(".");
      attempts++;
    }
    
    if(Blynk.connected()) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi: Reseau ");
      lcd.print(connectedNetworkIndex + 1);
      lcd.setCursor(0, 1);
      lcd.print("Blynk ready!");
      delay(2000);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Blynk timeout");
      lcd.setCursor(0, 1);
      lcd.print("Mode local");
      delay(2000);
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi: ECHEC");
    lcd.setCursor(0, 1);
    lcd.print("Mode local");
    delay(2000);
  }
  
  // Timer - toutes les 2 secondes
  timer.setInterval(2000L, sendTemperature);
  
  Serial.println("═══════════════════════════════════════════");
  Serial.println("Système opérationnel - 3 aquariums");
  Serial.println("Contrôle buzzer:");
  Serial.println("  • Bouton physique GPIO19");
  Serial.println("  • Bouton Blynk V4 (à distance)");
  Serial.println("═══════════════════════════════════════════");
}

// ------------------- LOOP -------------------
void loop() {
  // Vérification et reconnexion WiFi automatique
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck >= 30000) {  // Vérifier toutes les 30 secondes
    lastWiFiCheck = millis();
    
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("\n⚠️  WiFi déconnecté! Tentative de reconnexion...");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi perdu!");
      lcd.setCursor(0, 1);
      lcd.print("Reconnexion...");
      
      connectToWiFi();
      
      if(WiFi.status() == WL_CONNECTED && !Blynk.connected()) {
        Blynk.connect();
      }
    }
  }
  
  // Exécution Blynk seulement si connecté
  if(Blynk.connected()) {
    Blynk.run();
  }
  
  timer.run();
  
  // ⚡ Lecture du bouton PHYSIQUE dans loop() pour réactivité instantanée
  // Le bouton physique a la PRIORITÉ sur le bouton Blynk
  if (digitalRead(BUTTON_PIN) == LOW) {  
    // Bouton physique APPUYÉ → désactive l'alarme
    if(alarmeActive) {  // Si c'était actif, afficher le changement
      Serial.println("🔘 Bouton physique: Alarme DÉSACTIVÉE");
    }
    alarmeActive = false;
    digitalWrite(BUZZER_PIN, LOW);  // Forcer buzzer OFF immédiatement
    
    // Synchroniser avec Blynk
    Blynk.virtualWrite(V4, 0);  // Mettre le bouton Blynk à OFF
    
  } else {
    // Bouton physique RELÂCHÉ
    // L'état de l'alarme est contrôlé par Blynk (via BLYNK_WRITE(V4))
    // Donc on ne fait rien ici, BLYNK_WRITE(V4) gère l'état
  }
  
  // ----- BUZZER (géré en continu dans loop) -----
  if(alerteDetectee && alarmeActive) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}
