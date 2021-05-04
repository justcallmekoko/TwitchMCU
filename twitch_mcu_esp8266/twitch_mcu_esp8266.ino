#include <WiFi.h>
//#include <ESP8266WiFi.h>
//#include <ESP8266WiFiMulti.h>
//#include <ESP8266WebServer.h>
//#include <ESP8266HTTPUpdateServer.h>
#include <IRCClient.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <ArduinoJson.h>
#include "secret.h"


// Definitions
#define IRC_SERVER   "irc.chat.twitch.tv"
#define IRC_PORT     80
//#define TWITCH_OAUTH_TOKEN TWITCH_OAUTH_TOKEN
#define TWITCH_BOT_NAME "ESP32Bot"

#define LED 16
#define Pixels 512 // Number of pixels in the ring

#define NUM_PATTERNS 6

// Patterns
#define LOAD_RING    0
#define TWINKLE      1
#define PULSE_RED    2
#define PULSE_PURPLE 3
#define RAINBOW      4
#define MSG          5

uint32_t init_time;

// WiFi Stuff
char ssid[] = SSID;       // your network SSID (name)
char password[] = PASS;  // your network key

WiFiClient wiFiClient;

// Twitch Stuff
const String twitchChannelName = "willstunforfood";
String ircChannel = "";
DynamicJsonBuffer jsonBuffer;

IRCClient client(IRC_SERVER, IRC_PORT, wiFiClient);

String help_msg = "View the full list of commands at https://github.com/justcallmekoko/TwitchMCU";


// LED Stuff
int width = 64;
int height = 8;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(Pixels, LED, NEO_GRB + NEO_KHZ800);

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(width, height, LED,
  NEO_MATRIX_BOTTOM    + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);
  
int x = matrix.width();
int mess_len = 0;
String ticker_message = "Hello, World!";

int current_pattern = TWINKLE;

float scanning[3] = {255, 6, 0};
float deauthing[3] = {255, 6, 0};
float serving[3] = {0, 170, 255};
float clicon[3] = {255, 255, 0};
float purple[3] = {255, 0, 255};

const uint16_t colors[] = {
  matrix.Color(255, 0, 0),
  matrix.Color(0, 255, 0),
  matrix.Color(255, 255, 0),
  matrix.Color(0, 0, 255),
  matrix.Color(255, 0, 255),
  matrix.Color(0, 255, 255),
  matrix.Color(255, 255, 255)};


int ring_speed = 1; // (ms * fade_delay)
int fade_delay = 4; // ms
int max_interval = 2; // TIME BETWEEN PIXELS FIRING ((0 - x) * 10ms)

int current_itter = ring_speed;
int current_pixel = 0;
int current_fade_itter = 1;
bool increasing = true;
bool show_led = true;
float redStates[Pixels];
float blueStates[Pixels];
float greenStates[Pixels];
float fadeRate = 0.80;

float wheel_speed = 0.3; // Low is slower
float k = 255;

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    //return strip.Color(0, 0, 0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  //return strip.Color(0, 0, 0);
}

void singleRainbow()
{
  for(int i = Pixels - 1; i >= 0; i--)
    strip.setPixelColor(i, Wheel((i * 256 / 50 + (int)k) % 256));     
    
  strip.show();

  k = k - wheel_speed;
  if (k < 0)
    k = 255;
}

// LED Pattern
void LoadRing(int r, int g, int b)
{
  if (show_led)
  {
    current_itter++;
    if (current_itter >= ring_speed * fade_delay)
    {   
      current_itter = 0;
      current_pixel++;
      if (current_pixel >= Pixels)
        current_pixel = 0;
        
      uint16_t i = current_pixel;
      if (redStates[i] < 1 && greenStates[i] < 1 && blueStates[i] < 1) {
        redStates[i] = r;
        greenStates[i] = g;
        blueStates[i] = b;
      }
    }
  
    if (current_fade_itter > fade_delay)
    {
      current_fade_itter = 1;
      
      for(uint16_t l = 0; l < Pixels; l++) {
        if (redStates[l] > 1 || greenStates[l] > 1 || blueStates[l] > 1) {
          strip.setPixelColor(l, redStates[l], greenStates[l], blueStates[l]);
          
          if (redStates[l] > 1) {
            redStates[l] = redStates[l] * fadeRate;
          } else {
            redStates[l] = 0;
          }
          
          if (greenStates[l] > 1) {
            greenStates[l] = greenStates[l] * fadeRate;
          } else {
            greenStates[l] = 0;
          }
          
          if (blueStates[l] > 1) {
            blueStates[l] = blueStates[l] * fadeRate;
          } else {
            blueStates[l] = 0;
          }
          
        } else {
          strip.setPixelColor(l, 0, 0, 0);
        }
      }
    }
    //strip.setPixelColor(0, 0, 255, 0);
    //strip.setPixelColor(11, 255, 0, 0);
    strip.show();
    
    current_fade_itter++;
    
    //delay(10);
  }
}

void Twinkle()
{
  if (show_led)
  {
    current_itter++;
    if (current_itter >= ring_speed)
    {
      current_itter = 0;
  
      if (random(max_interval) == 1) {
        uint16_t i = random(Pixels);
        if (redStates[i] < 1 && greenStates[i] < 1 && blueStates[i] < 1) {
          redStates[i] = random(256);
          greenStates[i] = random(256);
          blueStates[i] = random(256);
        }
      }
    }
  
    if (current_fade_itter > fade_delay)
    {
      current_fade_itter = 1;
      for(uint16_t l = 0; l < Pixels; l++) {
        if (redStates[l] > 1 || greenStates[l] > 1 || blueStates[l] > 1) {
          strip.setPixelColor(l, redStates[l], greenStates[l], blueStates[l]);
          
          if (redStates[l] > 1) {
            redStates[l] = redStates[l] * fadeRate;
          } else {
            redStates[l] = 0;
          }
          
          if (greenStates[l] > 1) {
            greenStates[l] = greenStates[l] * fadeRate;
          } else {
            greenStates[l] = 0;
          }
          
          if (blueStates[l] > 1) {
            blueStates[l] = blueStates[l] * fadeRate;
          } else {
            blueStates[l] = 0;
          }
          
        } else {
          strip.setPixelColor(l, 0, 0, 0);
        }
      }
    }
    current_fade_itter++;
    strip.show();
    //delay(10);
  }
}

void PulseRing(int r, int g, int b)
{
  if (show_led)
  {
    float modifier = 0;
    
    if (current_fade_itter > fade_delay)
    {
      if (increasing)
        modifier = 2 - fadeRate;
      else
        modifier = fadeRate;
  
      for(uint16_t l = 0; l < Pixels; l++)
      {
        redStates[l] = redStates[l] * modifier;
        
  
        greenStates[l] = greenStates[l] * modifier;
  
        
        blueStates[l] = blueStates[l] * modifier;
  
        if (redStates[l] < 1 && greenStates[l] < 1 && blueStates[l] < 1)
        {
          increasing = true;
          redStates[l] = r * 0.0037;
          greenStates[l] = g * 0.0037;
          blueStates[l] = b * 0.0037;
        }
        else if (redStates[l] >= 255 || greenStates[l] >= 255 || blueStates[l] >= 255)
        {
          increasing = false;
          redStates[l] = r;
          greenStates[l] = g;
          blueStates[l] = b;
        }
        
        strip.setPixelColor(l, redStates[l], greenStates[l], blueStates[l]);
      }
      
      current_fade_itter = 0;
  
      /*
      Serial.print(redStates[0]);
      Serial.print(" ");
      Serial.print(greenStates[0]);
      Serial.print(" ");
      Serial.println(blueStates[0]); 
      */   
    }
    
    current_fade_itter++;
    
    strip.show();
  }
}

void displayMessage() {
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(ticker_message);
  
  mess_len = ticker_message.length() * -6;
   
  if(--x < mess_len) {
    x = matrix.width();
  }
  matrix.show();
  
  delay(45);
}


void parseCommand(String command) {
  //Serial.println("Command: " + (String)command);

  if (command == "help") {
    sendTwitchMessage(help_msg);
    return;
  }

  JsonObject& json = jsonBuffer.parseObject(command);
  if (!json.success()) {
    Serial.println("Could not parse json: " + (String)command);
    Serial.println("Must be a string message. Sending to LED: " + (String)command);
    ticker_message = command;
    matrix.setTextColor(matrix.Color(random(100, 255), random(100, 255), random(100, 255)));
    current_pattern = MSG;
    matrix.fillScreen(0);
  }
  else {
    Serial.println("Successfully parsed json: " + (String)command);
    // Parse json commands
    if (json.containsKey("pattern")) {
      current_pattern = json["pattern"];
      ResetWheel();
      matrix.fillScreen(0);
      matrix.show();
    }
//    else if (json.containsKey("msg")) {
//      ticker_message = json["msg"].as<String>();
//      matrix.setTextColor(matrix.Color(random(0, 255), random(0, 255), random(0, 255)));
//      current_pattern = MSG;
//      matrix.fillScreen(0);
//    }
  }
}


// Callback when reading Twitch Chat
void callback(IRCMessage ircMessage) {
  //Serial.println("In CallBack");

  if (ircMessage.command == "PRIVMSG" && ircMessage.text[0] != '\001') {
    //Serial.println("Passed private message.");
    ircMessage.nick.toUpperCase();

    String message("<" + ircMessage.nick + "> " + ircMessage.text);

    //prints chat to serial
    Serial.println(message);

    // Get commands
    int command_start = ircMessage.text.indexOf("$!");
    //Serial.println((String)command_start);
    if (command_start == 0) {
      int command_end = ircMessage.text.length();
      //Serial.println((String)command_end);
      String command = ircMessage.text.substring(command_start + 2, command_end);
      parseCommand(command);
    }

    return;
  }
}


// Function to send messages to Twitch Chat
void sendTwitchMessage(String message) {
  client.sendMessage(ircChannel, message);
}

void ResetWheel()
{
  for(uint16_t l = 0; l < Pixels; l++) {
    redStates[l] = 0;
    greenStates[l] = 0;
    blueStates[l] = 0;
    strip.setPixelColor(l, redStates[l], greenStates[l], blueStates[l]);
  }
  strip.show();
}

void runCurrentPattern(int pattern) {
  if (pattern == LOAD_RING)
    LoadRing(scanning[0], scanning[1], scanning[2]);
  else if (pattern == TWINKLE)
    Twinkle();
  else if (pattern == PULSE_RED)
    PulseRing(255, 0, 0);
  else if (pattern == PULSE_PURPLE)
    PulseRing(255, 0, 255);
  else if (pattern == RAINBOW)
    singleRainbow();
  else if (pattern == MSG)
    displayMessage();
}

void sendPing() {
  //Serial.println("Pinging ircChannel");
  //client.sendMessage("PING " + ircChannel);
}


void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  
  // Connect to WiFi
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(115200);

  strip.begin();
  strip.setBrightness(25);
  strip.show();

  matrix.begin();
  matrix.setBrightness(5);
  matrix.setTextWrap(false);
  matrix.fillScreen(0);
  matrix.show();

  ircChannel = "#" + twitchChannelName;

  connectWiFi();

  client.setCallback(callback);

  init_time = millis();

  ResetWheel();
}


void loop() {

  // Try to connect to chat. If it loses connection try again
  
  if (!client.connected()) {
    Serial.println("Attempting to connect to " + ircChannel );
    // Attempt to connect
    // Second param is not needed by Twtich
    if (client.connect(TWITCH_BOT_NAME, "", TWITCH_OAUTH_TOKEN)) {
      client.sendRaw("JOIN " + ircChannel);
      Serial.println("connected and ready to rock");
      //sendTwitchMessage("Ready to go Boss!");
    } else {
      Serial.println("failed... try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    return;
  }
  client.loop();

  runCurrentPattern(current_pattern);

  // Check wifi
  if (WiFi.status() != WL_CONNECTED)
    connectWiFi();

  if (millis() - init_time >= 1000) {
    init_time = millis();
    sendPing();
  }

  delay(1);
}
