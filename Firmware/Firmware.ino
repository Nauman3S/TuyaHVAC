/*
 * @FileName: Firmware.ino
 * @Author: Nauman
 * @Email: NaumanShakir3S@gmail.com
 * @LastEditors: Nauman Shakir
 * @Date: 2021-09-03
 * @LastEditTime: 2021-09-02
 * @Copyright: 3STechLabs
 * @Company: https://3STechLabs.com
 * @Description: HVAC automation using ESP8266 and arduino nano
 * @Github:https://github.com/Nauman3S/TuyaHVAC
 */

#include <TuyaWifi.h>
#include <SoftwareSerial.h>
unsigned char faultV = 0;
#include "relayHandle.h"
#include "dht22.h"

TuyaWifi my_device;

/* Current LED status */
unsigned char led_state = 0;
unsigned char led_state2 = 0;
float temp;
float TempHigh = 30;
/* Connect network button pin */
int key_pin = 7;

/* Data point define */
#define DPID_SWITCH 20
#define DPID_SWITCH_GATE 21
#define DPID_TEMP_CURRENT 1
#define DPID_HIGH_LIMIT_TEMP 2
#define DPID_FAULT 3

/* Stores all DPs and their types. PS: array[][0]:dpid, array[][1]:dp type. 
 *                                     dp type(TuyaDefs.h) : DP_TYPE_RAW, DP_TYPE_BOOL, DP_TYPE_VALUE, DP_TYPE_STRING, DP_TYPE_ENUM, DP_TYPE_BITMAP
*/
unsigned char dp_array[][5] =
    {
        {DPID_SWITCH, DP_TYPE_BOOL},
        {DPID_SWITCH_GATE, DP_TYPE_BOOL},
        {DPID_TEMP_CURRENT, DP_TYPE_VALUE},
        {DPID_HIGH_LIMIT_TEMP, DP_TYPE_VALUE},
        {DPID_FAULT, DP_TYPE_VALUE},

};

unsigned char pid[] = {"ma67l9sgmdyg3d2k"};
unsigned char mcu_ver[] = {"1.0.0"};

/* last time */
unsigned long last_time = 0;

void setup()
{
  // Serial.begin(9600);
  Serial.begin(9600);
  setupRelays();
  setupDHT22();

  //Initialize led port, turn off led.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  //Initialize networking keys.
  pinMode(key_pin, INPUT_PULLUP);

  //Enter the PID and MCU software version
  my_device.init(pid, mcu_ver);
  //incoming all DPs and their types array, DP numbers
  my_device.set_dp_cmd_total(dp_array, 1);
  //register DP download processing callback function
  my_device.dp_process_func_register(dp_process);
  //register upload all DP callback function
  my_device.dp_update_all_func_register(dp_update_all);

  last_time = millis();
}

void loop()
{
  my_device.uart_service();
  temp = getTemp();
  //Enter the connection network mode when Pin7 is pressed.
  if (digitalRead(key_pin) == LOW)
  {
    delay(80);
    if (digitalRead(key_pin) == LOW)
    {
      my_device.mcu_set_wifi_mode(SMART_CONFIG);
    }
  }
  /* LED blinks when network is being connected */
  if ((my_device.mcu_get_wifi_work_state() != WIFI_LOW_POWER) && (my_device.mcu_get_wifi_work_state() != WIFI_CONN_CLOUD) && (my_device.mcu_get_wifi_work_state() != WIFI_SATE_UNKNOW))
  {
    if (millis() - last_time >= 1000)
    {
      dp_update_all(); //send readings
      last_time = millis();

      if (led_state == LOW)
      {
        led_state = HIGH;
      }
      else
      {
        led_state = LOW;
      }
      digitalWrite(LED_BUILTIN, led_state);
    }
  }
  //automate
  if (led_state == HIGH)
  { //turn on cooling
    controlRelay(1, 1);
  }
  else
  {
    controlRelay(1, 0);
  }
  if (led_state2 == HIGH)
  { //turn on heating
    controlRelay(2, 1);
  }
  else
  {
    controlRelay(2, 0);
  }
  delay(10);
}

/**
 * @description: DP download callback function.
 * @param {unsigned char} dpid
 * @param {const unsigned char} value
 * @param {unsigned short} length
 * @return {unsigned char}
 */
unsigned char dp_process(unsigned char dpid, const unsigned char value[], unsigned short length)
{
  switch (dpid)
  {
  case DPID_SWITCH:
    led_state = my_device.mcu_get_dp_download_data(dpid, value, length); /* Get the value of the down DP command */
    if (led_state)
    {
      //Turn on
      controlRelay(1, 1)
    }
    else
    {
      //Turn off
      controlRelay(1, 0)
    }
    //Status changes should be reported.
    my_device.mcu_dp_update(dpid, value, length);
    break;

  case DPID_SWITCH_GATE:
    led_state2 = my_device.mcu_get_dp_download_data(dpid, value, length); /* Get the value of the down DP command */
    if (led_state2)
    {
      //Turn on
      controlRelay(2, 1)
    }
    else
    {
      //Turn off
      controlRelay(2, 1)
    }
    //Status changes should be reported.
    my_device.mcu_dp_update(dpid, value, length);
    break;

  case DPID_TEMP_CURRENT:
    int k = my_device.mcu_get_dp_download_data(dpid, value, length); /* Get the value of the down DP command */
    //Status changes should be reported.
    my_device.mcu_dp_update(dpid, value, length);
    break;

  default:
    break;
  }
  return SUCCESS;
}

/**
 * @description: Upload all DP status of the current device.
 * @param {*}
 * @return {*}
 */
void dp_update_all(void)
{
  my_device.mcu_dp_update(DPID_SWITCH, led_state, 1);
  my_device.mcu_dp_update(DPID_SWITCH_GATE, led_state2, 1);
  my_device.mcu_dp_update(DPID_TEMP_CURRENT, temp, 1);
  my_device.mcu_dp_update(DPID_HIGH_LIMIT_TEMP, TempHigh, 1);
  my_device.mcu_dp_update(DPID_FAULT, faultV, 1);
}