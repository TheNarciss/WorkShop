#include <WiFi.h>
#include <WebSocketsServer.h>

// Informations de connexion Wi-Fi
const char* ssid = "PoleDeVinci_IFT";
const char* password = "*c.r4UV@VfPn_0";

WebSocketsServer webSocket = WebSocketsServer(81);
const int ledPin = 13; // Pin à laquelle la LED est connectée
const int irPin = 35;  // Pin à laquelle le récepteur IR est connecté
const int newInputPin = 19; // Nouvelle pin d'entrée
bool ledState = false;  // État de la LED

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connexion au WiFi...");
    }

    Serial.println("Connecté au WiFi");
    Serial.print("Adresse IP: ");
    Serial.println(WiFi.localIP());

    pinMode(ledPin, OUTPUT); // Configure la pin de la LED en sortie
    digitalWrite(ledPin, ledState); // Éteindre la LED au démarrage

    pinMode(irPin, INPUT); // Configure la pin IR en entrée
    pinMode(newInputPin, INPUT_PULLUP); // Configure la nouvelle pin d'entrée en entrée

    

    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("Déconnecté: %u\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("Connecté: %s\n", ip.toString().c_str());
            break;
        }
        case WStype_TEXT:
            Serial.printf("Message reçu: %s\n", payload);
            if (strcmp((char*)payload, "Switch1") == 0) {
                // Basculer l'état de la LED
                ledState = !ledState; 
                digitalWrite(ledPin, ledState); // Appliquer l'état
            }
            // Envoyer une confirmation au client
            webSocket.sendTXT(num, "Bien reçu");
            break;
    }
}

void loop() {
    webSocket.loop();
    
    static unsigned long lastTime = 0;
    static unsigned long irCheckTime = 0; // Temps pour vérifier les pins

    // Vérifier l'état des récepteurs toutes les 5 secondes
    if (millis() - irCheckTime > 10) { // 5000 ms = 5 secondes
        irCheckTime = millis();
        float irValue = analogRead(irPin); // Lire la sortie de la pin IR
        float newInputValue = digitalRead(newInputPin); // Lire la nouvelle pin d'entrée

        Serial.print("Valeur IR: ");
        Serial.println(irValue); // Afficher la valeur sur le moniteur série
        Serial.print("Nouvelle valeur: ");
        Serial.println(newInputValue); // Afficher la nouvelle valeur sur le moniteur série

        // Envoyer les valeurs au Serial Plotter
        Serial.print("IRValue: ");
        Serial.println(irValue);
        Serial.print("NewInputValue: ");
        Serial.println(newInputValue);
        
        // Envoyer les valeurs IR et nouvelle pin ensemble au serveur WebSocket
        String message =String(irValue) + "," + String(newInputValue);
        webSocket.broadcastTXT(message); // Envoyer les valeurs au serveur
    }

    // Logique de message régulier (8 secondes)
    if (millis() - lastTime > 8000) {
        lastTime = millis();
        String message = "Message du serveur ESP32";
        webSocket.broadcastTXT(message);
    }
}
