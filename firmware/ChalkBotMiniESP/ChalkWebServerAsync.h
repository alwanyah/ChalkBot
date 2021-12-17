
#ifndef ChalkWebServerAsync_H
#define ChalkWebServerAsync_H

#include <WiFi.h>

#include <AsyncTCP.h>

//https://github.com/me-no-dev/ESPAsyncWebServer
#include <ESPAsyncWebServer.h>

#include "math/Pose2D.h"

class ChalkWebServerAsync
{
public:
  AsyncWebServer server;

  // commands that can be handeled by the server
  enum Command {
    NONE = 0,
    DRIVE = 1,
    MOTOR_TEST = 2,
    DRIVE_IMU = 3,
    GOTO = 4
  } lastCommand = NONE;

  unsigned long timeOfLastCommand = 0;

  // log messages (experimental)
  String message = "";

  // for parsing commands
  char receive_buffer[128];

  // motor test
  int16_t motor_speed[5];

  struct {
    int16_t x_pwm    = 0; // [-255,255] drive
    int16_t a_pwm    = 0; // [-255,255] turn
    int16_t p_pwm    = 0; // [0,255] print
    int16_t duration = 0; // in ms
  } driveRequest;

  struct {
    int16_t vx       = 0;
    float va         = 0;
    int16_t p        = 0;
    int16_t duration = 0;
  } driveIMURequest;

  // target point for the robot to drive to
  struct {
    Vector2d target;
    int16_t p_pwm = 0;
  } gotoPointRequest;

  // status of the robot
  String status_motion = "unknown";
  String status_imu = "unknown";

  // reported orientation
  double orientation = 0.0;

  // reported odometry pose
  Pose2D robotPose;

  int16_t debug = 0;

  IPAddress ipAddress;

  ChalkWebServerAsync() : server(80) {
    //AsyncWebServer server(80);
  } 

  char* make_string(uint8_t* inputData, size_t len) {
    assert(len < sizeof(receive_buffer));
    memcpy(receive_buffer, inputData, len);
    receive_buffer[len] = '\0';
    return receive_buffer;
  }
  
  void initAP(const std::string& ssid, const std::string& password) 
  {
    // Set Static IP address
    IPAddress local_IP(10, 0, 4, 99);
    // Set your Gateway IP address
    IPAddress gateway(10, 0, 4, 1);
    IPAddress subnet(255, 255, 0, 0);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str(), password.c_str());
    delay(1000);
     
    // Configures static IP address
    if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
      Serial.println("AP Failed to configure");
    } else {
      delay(1000);
      Serial.println("AP configured to: ");
      Serial.println(WiFi.softAPIP());
    }

    ipAddress = WiFi.softAPIP();

    registerRequests();
    
    // Start server
    server.begin();
  }
  
  void init(const std::string& ssid, const std::string& password) 
  {
    
    WiFi.mode(WIFI_AP_STA);
    //WiFi.mode(WIFI_STA);
    Serial.print("connecting to: ");
    Serial.println(ssid.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    /*
    // connect to existing wifi
    // Set Static IP address
    IPAddress local_IP(10, 0, 4, 99);
    // Set your Gateway IP address
    IPAddress gateway(10, 0, 4, 1);
    IPAddress subnet(255, 255, 0, 0);
    if (!WiFi.config(local_IP, gateway, subnet)) {
      Serial.println("STA Failed to configure");
    }
    */

    ipAddress = WiFi.localIP();

    //WiFi.setHostname("chalkbot");

    registerRequests();
    
    // Start server
    server.begin();
  }


  void registerRequests() 
  {
    // registed get responses
    server.on("/log", HTTP_GET, [&](AsyncWebServerRequest *request){
      request->send(200, "text/plain", message);
    });

    server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
      String str;
      str += "<p>ChalkBot Control. Use apropriate get requests.</p>";
      str += "<p>IMU: " + String(status_imu) + "</p>";
      request->send(200, "text/plain", str);
    });

    server.on("/status_motion", HTTP_POST, [&](AsyncWebServerRequest *request) {
          Serial.println("/status_motion");
          Serial.println(status_motion);
          request->send(200, "text/plain", status_motion);
    });

    server.on("/status_imu", HTTP_POST, [&](AsyncWebServerRequest *request) {
      Serial.println("/status_imu");
      Serial.println(status_imu);
      request->send(200, "text/plain", status_imu);
    });

    server.on("/orientation", HTTP_POST, [&](AsyncWebServerRequest *request) {
      Serial.println("/orientation");
      Serial.println(orientation);
      request->send(200, "text/plain", String(orientation));
    });

    server.on("/pose", HTTP_POST, [&](AsyncWebServerRequest *request) {
      Serial.println("/pose");
      String msg = String("") 
        + robotPose.translation.x + ";"
        + robotPose.translation.y + ";"
        + robotPose.rotation;
        
      request->send(200, "text/plain", msg);
    });

    server.on("/debug", HTTP_POST, [&](AsyncWebServerRequest *request) {}, NULL,
        [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
          sscanf(make_string(data_website, len), "%d", &debug);

          Serial.println("/debug");
          Serial.println(debug);
          request->send(200);
    });

    server.on("/motor", HTTP_POST, [](AsyncWebServerRequest* request){}, NULL,
        [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
          Serial.println("/motor");
          
          sscanf(make_string(data_website, len), "%d;%d;%d;%d;%d", 
            &(motor_speed[0]), 
            &(motor_speed[1]), 
            &(motor_speed[2]), 
            &(motor_speed[3]), 
            &(motor_speed[4]));

          timeOfLastCommand = millis();
          lastCommand = MOTOR_TEST;

          request->send(200);
    });
    
    server.on("/drive", HTTP_POST, [](AsyncWebServerRequest* request){}, NULL,
        [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
          Serial.println("/drive");
          
          sscanf(make_string(data_website, len), "%d;%d;%d;%d", 
            &driveRequest.x_pwm, 
            &driveRequest.a_pwm, 
            &driveRequest.p_pwm, 
            &driveRequest.duration);

          timeOfLastCommand = millis();
          lastCommand = DRIVE;

          request->send(200);
    });

    server.on("/drive_imu", HTTP_POST, [](AsyncWebServerRequest* request){}, NULL,
        [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
          Serial.println("/drive_imu");
          
          sscanf(make_string(data_website, len), "%d;%f;%d;%d", 
            &driveIMURequest.vx, 
            &driveIMURequest.va, 
            &driveIMURequest.p, 
            &driveIMURequest.duration);

          //Serial.println((const char*)data_website);
          //Serial.println(driveIMURequest.vx);
          //Serial.println(driveIMURequest.va);
          //Serial.println(driveIMURequest.p);
          //Serial.println(driveIMURequest.duration);

          timeOfLastCommand = millis();
          lastCommand = DRIVE_IMU;

          request->send(200);
    });

    server.on("/goto_point", HTTP_POST, [](AsyncWebServerRequest* request){}, NULL,
        [&](AsyncWebServerRequest* request, uint8_t* data_website, size_t len, size_t index, size_t total) {
          Serial.println("/goto_point");
          
          sscanf(make_string(data_website, len), "%lf;%lf;%d", 
            &gotoPointRequest.target.x, 
            &gotoPointRequest.target.y, 
            &gotoPointRequest.p_pwm);

          /*
          Serial.print(gotoPointRequest.target.x);
          Serial.print(",");
          Serial.print(gotoPointRequest.target.y);
          Serial.print(",");
          Serial.print(gotoPointRequest.p_pwm);
          */
          
          timeOfLastCommand = millis();
          lastCommand = GOTO;

          request->send(200);
    });
  }


  IPAddress gerServerIP() {
    return ipAddress;
  }

  void log(const char* msg) {
    message += msg;
    message += '\n';
    Serial.print(message);
  }
};


#endif // ChalkWebServerAsync_H
