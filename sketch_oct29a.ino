#include <M5StickCPlus.h>
//Accelerometer 
float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

//Previous x,y,z readings
float prevX = 0;
float prevY = 0;
float prevZ = 0;

//Tracking time change event
unsigned long eventTiming = 0;
bool isEvent =false;

//Sensitivity threshold (will change to calcualted averages later....)
const float THRESHOLD = 0.7;

//Color library for changes
uint16_t normal = BLACK;
uint16_t head_swing = RED;
uint16_t head_butt = BLUE;

//Setting screen,color,and size
void drawStaticLabels(uint16_t bgColor) {
  M5.Lcd.fillScreen(bgColor);
  M5.Lcd.setTextColor(WHITE, bgColor);
  M5.Lcd.setTextSize(1);

  M5.Lcd.setCursor(60, 15);
  M5.Lcd.println("Sheep-ish TEST");
  M5.Lcd.setCursor(30, 30);
  M5.Lcd.println("  X       Y       Z");
  M5.Lcd.setCursor(30, 70);
  M5.Lcd.println("  Orientation    Time(sec)");
}

void setup() {
  // put your setup code here, to run once:
  M5.begin();             // Init M5StickC Plus.
  M5.Imu.Init();          // Init IMU 
  M5.Lcd.setRotation(3);  // Rotate the screen landscape
  Serial.println("\nM5StickC Plus Accelerometer Starting...");

  //Setting screen,color,and size
  M5.Lcd.fillScreen(normal);
  M5.Lcd.setTextColor(WHITE,normal);
  M5.Lcd.setTextSize(1);
  drawStaticLabels(normal);
}

void loop() {
  M5.Imu.getAccelData(&accX, &accY, &accZ);
  //Timing
  unsigned long t= millis();
  //t= t/1000;

  // Compute orientation (simple approximation)
  String orientation;
  if (abs(accX) > abs(accY)) {
    if (accX > 0.5) orientation = "Landscape Right";
    else if (accX < -0.5) orientation = "Landscape Left";
    else orientation = "Flat / Neutral";
  } else {
    if (accY > 0.5) orientation = "Portrait Up";
    else if (accY < -0.5) orientation = "Portrait Down";
    else orientation = "Flat / Neutral";
  }
  

  bool isHeadEvent = (abs(accX-prevX)> THRESHOLD || abs(accY-prevY) > THRESHOLD );
  bool isButtEvent = (abs(accZ-prevZ) > THRESHOLD );

  // Keep track of last event types
  static String lastEvent = "";  // "xy" or "z"
  static unsigned long lastEventTime = 0;


  //Checking for events head swings
  if(isHeadEvent){
    lastEvent = "xy";
    lastEventTime = t;
    Serial.printf("%lu,%.3f,%.3f,%.3f,%s,Head movement detected\n", t, accX, accY, accZ, orientation.c_str());
  }

  //Checking for events head butts
  if(isButtEvent){
    lastEvent = "z";
    lastEventTime = t;
    Serial.printf("%lu,%.3f,%.3f,%.3f,%s,Head movement detected\n", t, accX, accY, accZ, orientation.c_str());
  }

  if(isHeadEvent || isButtEvent){
    isEvent = true;
    eventTiming = t;

    uint16_t color;
    if (lastEvent == "z") color = head_butt;   // Blue
    else color = head_swing;                   // Red

    M5.Lcd.fillScreen(color);
    M5.Lcd.setTextColor(WHITE, color);
    M5.Lcd.setCursor(50, 80);
    M5.Lcd.println(lastEvent == "z" ? "Head Butt!" : "Head Swing!");
  }

  //Reset after 1 second of display
  if(isEvent && (t-eventTiming > 1000)){
    isEvent = false;
    drawStaticLabels(normal);
  }

  //Display 
  M5.Lcd.setCursor(30, 40);
  M5.Lcd.printf(" %5.2f   %5.2f   %5.2f   ", accX, accY, accZ);
  M5.Lcd.setCursor(30, 80);
  M5.Lcd.printf("%-15s", orientation.c_str());
  M5.Lcd.setCursor(150, 80);
  M5.Lcd.printf("%5lu", t / 1000);

  //Setting previous readings
  prevX= accX;
  prevY = accY;
  prevZ= accZ;


  //Print statements for terminal
  Serial.print(t);
  Serial.print(",");
  Serial.print(accX, 4);
  Serial.print(",");
  Serial.print(accY, 4);
  Serial.print(",");
  Serial.print(accZ, 4);
  Serial.print(",");
  Serial.println(orientation);

   
  delay(100);
}
