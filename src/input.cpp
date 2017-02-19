/*
 * -------------------------------------------------------------------
 * EmonESP Serial to Emoncms gateway
 * -------------------------------------------------------------------
 * Adaptation of Chris Howells OpenEVSE ESP Wifi
 * by Trystan Lea, Glyn Hudson, OpenEnergyMonitor
 * All adaptation GNU General Public License as below.
 *
 * -------------------------------------------------------------------
 *
 * This file is part of OpenEnergyMonitor.org project.
 * EmonESP is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * EmonESP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with EmonESP; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "emonesp.h"
#include "input.h"
#include "energyic_UART.h"
#include "string"
#include "sstream"

String input_string="";
String last_datastr="";

boolean input_get(String& data)
{
  boolean gotData = false;

  // If data from test API e.g `http://<IP-ADDRESS>/input?string=CT1:3935,CT2:325,T1:12.5,T2:16.9,T3:11.2,T4:34.7`
  if(input_string.length() > 0) {
    data = input_string;
    input_string = "";
    gotData = true;
  }
  // If data received on serial
  // else if (Serial.available()) {
    // Could check for string integrity here
    // data = Serial.readStringUntil('\n');
    // gotData = true;
  // }
  // Get data from ATM90E26
  else {
    /*Repeatedly fetch some values from the ATM90E26 */
    unsigned short s_status = GetSysStatus();
    if(s_status == 0xFFFF)
      {
        #if defined(ESP8266)
        //Read failed reset ESP, this is heavy
        ESP.restart();
        #endif
      }
    // Sestaviti string: CT:<številka>  ,Current:<stevilka>..
    String str_txt_vol, str_txt_cur, str_txt_ap, str_txt_pf, str_txt_comma;
    str_txt_comma = String(",");  //comma
    // Voltage
    str_txt_vol = String("U:");
    float _vol = GetLineVoltage();
    // Current
    str_txt_cur = String("I:");
    float _cur = GetLineCurrent();
    // Active power
    str_txt_ap = String("P:");
    float _ap = GetActivePower();
    // Power Factor
    str_txt_pf = String("PF:");
    float _pf = GetPowerFactor();
    // Build data string
    data = str_txt_vol + _vol + str_txt_comma
          + str_txt_cur + _cur + str_txt_comma
          + str_txt_ap + _ap + str_txt_comma
          + str_txt_pf + _pf;
    gotData = true;
  }

  if(gotData)
  {
    // Get rid of any whitespace, newlines etc
    data.trim();

    if(data.length() > 0) {
      DEBUG.printf("Got '%s'\n", data.c_str());
      last_datastr = data;
    } else {
      gotData = false;
    }
  }

  return gotData;
}
