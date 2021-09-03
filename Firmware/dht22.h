/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
*/

//Libraries
#include <DHT.h>;

//Constants
#define DHTPIN 4          // what pin we're connected to
#define DHTTYPE DHT22     // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

void setupDHT22()
{

    dht.begin();
}

float getTemp()
{
    delay(2);
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp = dht.readTemperature();
    //Print temp and humidity values to serial monitor
    // Serial.print("Humidity: ");
    // Serial.print(hum);
    // Serial.print(" %, Temp: ");
    // Serial.print(temp);
    // Serial.println(" Celsius");
    // delay(10000); //Delay 2 sec.
    if (temp <= 0)
    {
        faultV = 1;
        return temp;
    }
