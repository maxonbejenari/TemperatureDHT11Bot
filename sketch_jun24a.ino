#include <ESP8266WiFi.h> // include to work with ESP module
#include <WiFiClientSecure.h> // allows us to provide secure client server connection, Telegram is a web service and we need to connect with it, Also it uses HTTPS and this lib help us
#include <UniversalTelegramBot.h> // to connect our bot and work with it
#include <DHT.h> // for our senzor

#define DHTPIN 2 // 2, because, second pin in DHT11 is data and we get data from this pin
#define DHTTYPE DHT11 // type in our case is dht11(maybe you use dht22)
DHT dht(DHTPIN, DHTTYPE); // dht is object of class DHT, DHT has constructor for (pin and type)

const char* ssid = "iPhone"; // name of wifi
const char* password = "ft12345678"; // password of wifi

const char* botToken = "7548678510:AAHH__lsRN_JmZE3MoSzqUTpwjVY3pBD6mA"; // token of your telegram bot
String chat_id = "6290509974"; // chat id of chat with your bot

WiFiClientSecure client; // client is object of class WifiClientSecure
UniversalTelegramBot bot(botToken, client); // bot is object of class UniversalTelegramBot

unsigned long lastCheckTime = 0; // last time then we ckeck new updates from bot, initial is 0
const unsigned long checkInterval = 1000; // its time that bot verify for new request, we pause bot for 1 second for better performance


void setup() {
  Serial.begin(9600); // baud rate(in my esp module docs says to set 9600 maybe in yours 115200)
  dht.begin(); // method to start reading from senzor

  WiFi.begin(ssid, password); // method to connect our module with wifi
  Serial.print("Connecting to internet");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n Connected to internet");
  client.setInsecure(); // permit connection HTTP without valid certificate
}

void loop() {
  if (millis() - lastCheckTime > checkInterval) { // chack if last action was done millis() return time in mls from starting board
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);  // verify and process new messages from telegram (+1 to exclude processing one message two times)
    while (numNewMessages) { // while we have messages we proceess them
      for (int i = 0; i < numNewMessages; i++) { // goes in all messages 
        String text = bot.messages[i].text; // get conext from user's message
        String from_id = bot.messages[i].chat_id; // get chat_id
        String from_name = bot.messages[i].from_name; // get name of user

        if (text == "/temp") { // verify if text is /temp
          float t = dht.readTemperature(); // get temperature from senzor
          float h = dht.readHumidity(); // get humidity from senzor

          if (!isnan(t) && !isnan(h)) { // isnan() return true/false (Not a Number)
            String response = "Temperature: " + String(t) + "\nHumidity: " + String(h);
            bot.sendMessage(from_id, response, ""); // send our response
          } else {
            bot.sendMessage(from_id, "Err to read from dht"); // t and h is not a number(something get wrong and senzor read our data incorrect)
          }
        } else {
          bot.sendMessage(from_id, "Please type /temp", ""); // user type wrong /temp
        }
      }
      numNewMessages = bot.getUpdates(bot.last_message_received + 1); // verify if we have not new message when we process our last
    }
    lastCheckTime = millis(); // set lastCheckTime to mls of our module life
  }
}
