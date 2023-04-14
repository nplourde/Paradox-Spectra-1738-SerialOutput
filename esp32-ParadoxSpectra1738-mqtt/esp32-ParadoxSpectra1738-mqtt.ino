#include <WiFi.h>
#include <PubSubClient.h>

/***************************************
 *  WiFi
 **************************************/
#define WIFI_SSID   "APO"
#define WIFI_PASSWD "001St3w4rt"

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "192.168.4.159";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("\nConnecting");

  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  

  // connect to mqtt broker
  client.setServer(mqtt_server, 1883);

  //start UART
  Serial2.begin(9600);
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ParadoxSpectra1738"))
    {
      Serial.println("connected");
    } 
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop()
{
  //  update mqtt broker connection
  if (!client.connected())
  {
    reconnect();
  }

  client.loop();

  //check UART for data
  if(Serial2.available() > 3)
  {
    size_t len = Serial2.available();

  Serial.print("len: ");
  Serial.println(len);  

    uint8_t sbuf[4];
    
    if(Serial2.readBytes(sbuf, 4))
    {
      int EventId = sbuf[0] >> 2;
      int CategoryId = ((sbuf[0] & 3) << 4) + (sbuf[1] >> 4);

      Serial.print("EventId: ");
      Serial.println(EventId);  

      Serial.print("CategoryId: ");
      Serial.println(CategoryId);          

      switch(EventId)
      {        
        case 0:
        {
          String eventString = "ParadoxSpectra1738/Zone/" + String(CategoryId,DEC);
          Serial.println(eventString);  
          client.publish(eventString.c_str(), "OK");
        }
        break;
        
        case 1:
        {
          String eventString = "ParadoxSpectra1738/Zone/" + String(CategoryId,DEC);
          Serial.println(eventString);
          client.publish(eventString.c_str(), "OPEN");
        }
        break;

        default:
        break;
      }
    }
  }
}
