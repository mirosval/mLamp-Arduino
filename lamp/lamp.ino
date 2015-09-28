/*
This RFduino sketch demonstrates a full bi-directional Bluetooth Low
Energy 4 connection between an iPhone application and an RFduino.

This sketch works with the rfduinoLedButton iPhone application.

The button on the iPhone can be used to turn the green led on or off.
The button state of button 1 is transmitted to the iPhone and shown in
the application.
*/

/*
 Copyright (c) 2014 OpenSourceRF.com.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <RFduinoBLE.h>

//#define DEBUG

#ifdef DEBUG
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINT(x) Serial.print(x)
#else
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINT(x)
#endif

// Lamp Modes
const byte MODE_RESET = 0 << 0; // resets the lamp to 0, 0, 0 after the client disconnects
const byte MODE_HOLD = 1 << 0; // will leave the lamp on with the last color set

// pin 3 on the RGB shield is the red led
// (can be turned on/off from the iPhone app)
int led_r = 3;
int led_g = 4;
int led_b = 2;

// these hold the current color
uint8_t color_r = 0;
uint8_t color_g = 0;
uint8_t color_b = 0;

// mode is either:
// 0 - resets
// 1 - holds
// See MODE_RESET and MODE_HOLD
byte mode = 0;

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  DEBUG_PRINTLN("RFduino Setup");
  
  // led turned on/off from the iPhone app
  pinMode(led_r, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(led_b, OUTPUT);

  // button press will be shown on the iPhone app)
//  pinMode(button, INPUT);

  // this is the data we want to appear in the advertisement
  // (if the deviceName and advertisementData are too long to fix into the 31 byte
  // ble advertisement packet, then the advertisementData is truncated first down to
  // a single byte, then it will truncate the deviceName)
  RFduinoBLE.advertisementData = "mlamp";
  
  // start the BLE stack
  RFduinoBLE.begin();
}

void loop() {
  // I found that I get the best results when I set the color from the loop
  delay(10);
  
  // set the color
  analogWrite(led_r, color_r);
  analogWrite(led_g, color_g);
  analogWrite(led_b, color_b);
}

void RFduinoBLE_onConnect()
{
  DEBUG_PRINTLN("Connecting"); 
}

void RFduinoBLE_onDisconnect()
{
  DEBUG_PRINTLN("Disconnecting");
  DEBUG_PRINT("Mode: ");
  DEBUG_PRINTLN(mode);
  
  if(mode & MODE_HOLD) {
    DEBUG_PRINTLN("Mode was set to MODE_HOLD, so will leave LEDs on");
  } else {
    // MODE_RESET
    DEBUG_PRINTLN("Mode was set to MODE_RESET, so turning off LEDs");
    // don't leave the led on if they disconnect
    color_r = 0;
    color_g = 0;
    color_b = 0;
  }
}

void RFduinoBLE_onReceive(char *data, int len)
{
  DEBUG_PRINTLN("BLE onReceive called");
  if(len != 4) {
    return;
  }
  
  // 2nd byte is mode
  mode = (byte)data[0];
  
  // 3rd, 4th, 5th are r,g,b
  byte r = data[1];
  byte g = data[2];
  byte b = data[3];

  DEBUG_PRINT("R: ");
  DEBUG_PRINT(r);
  DEBUG_PRINT(" G: ");
  DEBUG_PRINT(g);
  DEBUG_PRINT(" B: ");
  DEBUG_PRINT(b);
  DEBUG_PRINTLN("");

  // set the current color to the color that we received
  color_r = r;
  color_g = g;
  color_b = b;
}
