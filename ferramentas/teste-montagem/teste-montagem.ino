/*
 * Teste da montagem na protoboard — não precisa de Wi-Fi nem de biblioteca.
 *
 * Grave este sketch ANTES do firmware do Bitcoin Tracker e abra o monitor
 * serial em 115200. Ele confere as três ligações:
 *   - OLED respondendo no I2C (SDA no GPIO21, SCL no GPIO22)
 *   - botão no GPIO4
 *   - LED no GPIO16 (acende enquanto o botão estiver apertado)
 */

#include <Wire.h>

const int PINO_BOTAO = 4;
const int PINO_LED = 16;

void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  pinMode(PINO_LED, OUTPUT);

  Wire.begin(21, 22);
  Serial.println();
  Serial.println("=== Teste da montagem ===");
  bool achou = false;
  for (uint8_t endereco : { 0x3C, 0x3D }) {
    Wire.beginTransmission(endereco);
    if (Wire.endTransmission() == 0) {
      Serial.printf("OK    OLED encontrado no endereco 0x%02X\n", endereco);
      achou = true;
    }
  }
  if (!achou) {
    Serial.println("FALHA OLED nao respondeu. Confira os fios 1 e 2 (SDA/SCL) e o VCC/GND do OLED.");
  }

  // O LED pisca 3 vezes no início: se não piscar, o problema está no LED,
  // no resistor ou no fio 3 — não no botão.
  for (int i = 0; i < 3; i++) {
    digitalWrite(PINO_LED, HIGH); delay(200);
    digitalWrite(PINO_LED, LOW);  delay(200);
  }
  Serial.println("      O LED deve ter piscado 3 vezes.");
  Serial.println("      Agora aperte o botao: o LED acende enquanto ele estiver apertado.");
}

void loop() {
  bool apertado = digitalRead(PINO_BOTAO) == LOW;
  digitalWrite(PINO_LED, apertado);

  static int anterior = -1;
  if (apertado != anterior) {
    Serial.println(apertado ? "OK    botao apertado -> LED aceso" : "      botao solto -> LED apagado");
    anterior = apertado;
  }
  delay(20);
}
