#include <dummy.h>

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Base64.h>

const char* ssid = "SAL WIFI";          // Your Wi-Fi SSID
const char* password = "client@sal123!";  // Your Wi-Fi Password

const char* smtp_server = "smtp.gmail.com";
const int smtp_port = 465;
const char* smtp_user = "kwameb792@gmail.com";  // Your Gmail address
const char* smtp_pass = "qbznedhjkeiptzrr";     // App password from Gmail (not your usual password)

WiFiClientSecure client;

const int trigPin = 5;  // GPIO5 for Trig pin
const int echoPin = 4;  // GPIO4 for Echo pin
long duration;
int distance;
int threshold = 15;  // Set threshold for mail detection (in cm)

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
}

void sendEmail() {
  client.setInsecure();
  if (!client.connect(smtp_server, smtp_port)) {
    Serial.println("Connection to SMTP server failed");
    return;
  }

  // Communicate with the SMTP server
  client.println("EHLO smtp.gmail.com");
  client.println("AUTH LOGIN");

  // Encode user and password in base64 and send
  String smtp_user_base64 = base64::encode((char*)smtp_user);
  String smtp_pass_base64 = base64::encode((char*)smtp_pass);
  client.println(smtp_user_base64);
  client.println(smtp_pass_base64);

  // Start sending the email
  client.println("MAIL FROM:<" + String(smtp_user) + ">");
  client.println("RCPT TO:<kwameb792@gmail.com>");  // Change this to your recipient email
  client.println("DATA");
  
  // Email content
  client.println("From: Mailbox Notification <" + String(smtp_user) + ">");
  client.println("To: Kwame <kwameb792@gmail.com>");
  client.println("Subject: Mail Alert");
  client.println();
  client.println("You've received mail in your mailbox!");
  client.println(".");
  
  client.println("QUIT");
  Serial.println("Email sent");
}

void loop() {
  // Measure the distance using the Ultrasonic Sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < threshold) {  // If the distance is less than the threshold, mail is detected
    Serial.println("Mail detected!");
    sendEmail();
    delay(60000);  // Wait 1 minute before checking again to prevent multiple emails
  }

  delay(1000);  // Check every second
}
