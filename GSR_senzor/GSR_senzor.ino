#include <Firebase_Arduino_WiFiNINA.h>
#include <NTPClient.h>

#define FIREBASE_HOST "gsrsenzor-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "DbHtMw0FwbOKZNp0aowsLji4eQx0sLq6VTCydK5p"
#define WIFI_SSID "chrenova"
#define WIFI_PASSWORD "student2"

FirebaseData firebaseData;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String path = "/GSRData";

const int GSRPin = A0;
int gsrAverage = 0;
int maxgsrvalue = 0;
int mingsrvalue = 10000;

void setup() {
  Serial.begin(9600);

  Serial.print("Pripojenie k sieti Wi-Fi");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }
  Serial.println("Pripojené!");
  Serial.print("IP adresa:");
  Serial.println(WiFi.localIP());

  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);

  timeClient.begin();
  timeClient.setTimeOffset(7200);
}

void loop() {
  
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(GSRPin);
    delay(5);
  }
  gsrAverage = sum / 10;
  Firebase.setInt(firebaseData, path + "/gsr_avarage", gsrAverage);

  if (gsrAverage > maxgsrvalue) {
    maxgsrvalue = gsrAverage;
   
    Firebase.setInt(firebaseData, path + "/max_gsr_value", maxgsrvalue);
  }
  if (gsrAverage < mingsrvalue) {
    mingsrvalue = gsrAverage;
    
    Firebase.setInt(firebaseData, path + "/min_gsr_value", mingsrvalue);
  }
  

  char timeStamp[20];
  timeClient.update();
  
  sprintf(timeStamp, "%02d:%02d:%02d", timeClient.getHours(), timeClient.getMinutes(), timeClient.getSeconds());
  String dataPath = String("GSRData/data/") + timeStamp;

 

  if (Firebase.setInt(firebaseData, dataPath, gsrAverage)) {
    Serial.print("GSR Average sent to Firebase at path: ");
    Serial.print(dataPath);
    Serial.print(" with value: ");
    Serial.println(gsrAverage);
  } else {
    Serial.print("Failed to send GSR average to Firebase: ");
    Serial.println(firebaseData.errorReason());
  }

  delay(2000);
}
