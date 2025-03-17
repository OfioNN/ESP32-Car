#include <WiFi.h>
#include <WebServer.h>

// Ustawienia WiFi
const char* ssid = "***";
const char* password = "***";

// Definicja pinów silników
#define IN1 18  // Silnik lewy do przodu
#define IN2 19  // Silnik lewy do tyłu
#define IN3 22  // Silnik prawy do przodu
#define IN4 23  // Silnik prawy do tyłu

WebServer server(80);

// Strona WWW z przyciskami sterującymi
void handleRoot() {
    server.send(200, "text/html",
        "<!DOCTYPE html>"
        "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>"
        "<style>"
        "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #282c34; color: white; text-align: center; padding: 20px; margin: 0; }"
        "h1 { color: #bbbbbb; font-size: 36px; margin-bottom: 20px; }"
        "button { font-size: 24px; width: 72px; height: 72px; margin: 10px; border: none; border-radius: 25px; background: #02AC1E; color: white; cursor: pointer; box-shadow: 3px 3px 5px rgba(0,0,0,0.8); transition: transform 0.3s, background 0.3s ease; }"
        "button:hover { background: #1e8e2f; transform: scale(1.1); }"
        "button:active { background: #0C3A00; transform: scale(0.85); }"
        "svg { width: 50px; height: 50px; fill: #2854C5; }"
        "</style></head><body>"
        "<h1>OfioN ~ F1 Car</h1>"
        "<button onclick=\"sendCommand('/przod')\"><svg xmlns='http://www.w3.org/2000/svg' height='72px' viewBox='0 -960 960 960' width='72px' fill='#2854C5'><path d='M440-160v-487L216-423l-56-57 320-320 320 320-56 57-224-224v487h-80Z'/></svg></button><br>"
        "<button onclick=\"sendCommand('/lewo')\"><svg xmlns='http://www.w3.org/2000/svg' height='72px' viewBox='0 -960 960 960' width='72px' fill='##DA0049'><path d='M400-240 160-480l240-240 56 58-142 142h486v80H314l142 142-56 58Z'/></svg></button>"
        "<button onclick=\"sendCommand('/stop')\"><svg xmlns='http://www.w3.org/2000/svg' height='72px' viewBox='0 -960 960 960' width='72px' fill='#DA0049'><path d='M320-320h320v-320H320v320ZM480-80q-83 0-156-31.5T197-197q-54-54-85.5-127T80-480q0-83 31.5-156T197-763q54-54 127-85.5T480-880q83 0 156 31.5T763-763q54 54 85.5 127T880-480q0 83-31.5 156T763-197q-54 54-127 85.5T480-80Zm0-80q134 0 227-93t93-227q0-134-93-227t-227-93q-134 0-227 93t-93 227q0 134 93 227t227 93Zm0-320Z'/></svg></button>"
        "<button onclick=\"sendCommand('/prawo')\"><svg xmlns='http://www.w3.org/2000/svg' height='72px' viewBox='0 -960 960 960' width='72px' fill='#2854C5'><path d='m560-240-56-58 142-142H160v-80h486L504-662l56-58 240 240-240 240Z'/></svg></button><br>"
        "<button onclick=\"sendCommand('/tyl')\"><svg xmlns='http://www.w3.org/2000/svg' height='72px' viewBox='0 -960 960 960' width='72px' fill='#2854C5'><path d='M440-800v487L216-537l-56 57 320 320 320-320-56-57-224 224v-487h-80Z'/></svg></button>"
        "<script>"
        "function sendCommand(url) {"
        "    fetch(url)"
        "        .then(response => response.text())"
        "        .then(data => console.log(data))"
        "        .catch(error => console.error('Błąd:', error));"
        "}"
        "</script>"
        "</body></html>"
    );
}

// Funkcje sterujące kierunkiem
void forward() {  // Przód: oba silniki do przodu
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    server.send(200, "text/plain", "Przód");
}

void back() {  // Tył: oba silniki do tyłu
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    server.send(200, "text/plain", "Tył");
}

void left() {  // Lewo: lewy silnik do przodu, prawy silnik do tyłu (obrót w miejscu)
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    server.send(200, "text/plain", "Lewo");
}

void right() {  // Prawo: lewy silnik do tyłu, prawy silnik do przodu (obrót w miejscu)
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    server.send(200, "text/plain", "Prawo");
}

void stop() {  // Stop: zatrzymanie obu silników
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    server.send(200, "text/plain", "Stop");
}

void setup() {
    Serial.begin(115200);

    // Konfiguracja pinów silników
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    // Połączenie WiFi
    WiFi.begin(ssid, password);
    Serial.print("Łączenie z WiFi");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nPołączono!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nNie udało się połączyć z WiFi!");
    }

    // Konfiguracja serwera
    server.on("/", handleRoot);
    server.on("/przod", forward);
    server.on("/tyl", back);
    server.on("/lewo", left);
    server.on("/prawo", right);
    server.on("/stop", stop);
    
    server.begin();
    Serial.println("Serwer HTTP uruchomiony!");

    // Na start zatrzymujemy silniki
    stop();
}

void loop() {
    server.handleClient();
    delay(10); // Zapobiega przeciążeniu ESP32
}
