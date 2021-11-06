# nyssetaulu
ESP32 bus timetable display.

* Displays the next two arrival times of a bus to a stop, in the 10 next arrivals to that stop. Updates every 60s and shows a progress bar between the updates.
* Uses Arduino on a NodeMCU ESP32S board hooked into a I2C SSD1306-based 128x32 OLED display.

![Picture of the device](https://imgur.com/a/lUmuzCT)

## Configuration
* Add your WiFi info into `src/config.h`
* Change the stop id in `src/main.cpp` `PAYLOAD` variable to match your stop. The default `tampere:0566` is "Yliopisto D" in Tampere. You can use https://reittiopas.tampere.fi/pysakit/tampere%3A0566 to figure out the ID, but it also reads in the physical stop signs.
* Change the `src/main.cpp` `BUS_LINE` variable to match the headsign of the bus you want to track. Please note that these are direction and route specific.
* (Optional) if you want to change the display, anything that has a u8g2 class works. Just change the classname of the `u8g2` variable.
