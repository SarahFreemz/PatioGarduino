# PatioGarduino
Automated irrigation system using Arduino/C++ and an ESP8266 Wi-Fi module. The system parses JSON weather forecasts from the OpenWeatherMap API to skip pump cycles when significant rain is expected, monitors supply tank water level, logs watering cycles, and runs a DC pump powered by a solar panel through a charge controller with SLA battery backup. Developed with PlatformIO in Visual Studio Code.

The basic physical setup is a DC water pump powered by a solar panel managed by a charge controller and SLA battery. An Arduino sends variable power through a MOFSET to increase or decrease water flow through irrigation tubing run to each planter. 

Next steps: Build a web-based dashboard to show watering metrics, monitor battery power, low water/low power notifications, re-configure watering schedules, and run manual cycles. Add sensors for soil moisture. 

## Project Photos

<a href="images/USER_SCOPED_TEMP_DATA_MSGR_PHOTO_FOR_UPLOAD_1591991998194_6677298405994052760.jpeg">
  <img src="images/USER_SCOPED_TEMP_DATA_MSGR_PHOTO_FOR_UPLOAD_1591991998194_6677298405994052760.jpeg" alt="Initial testing" width="150" style="margin: 5px;">
</a>

<a href="images/USER_SCOPED_TEMP_DATA_MSGR_PHOTO_FOR_UPLOAD_1593490455168_6683583390074497181.jpeg">
  <img src="images/USER_SCOPED_TEMP_DATA_MSGR_PHOTO_FOR_UPLOAD_1593490455168_6683583390074497181.jpeg" alt="Adjustable solar panel bracket/mount" width="150" style="margin: 5px;">
</a>

<a href="images/IMG_20200403_121623119.jpg">
  <img src="images/IMG_20200403_121623119.jpg" alt="Irrigation lines 1" width="150" style="margin: 5px;">
</a>

<a href="images/IMG_20200403_121627293.jpg">
  <img src="images/IMG_20200403_121627293.jpg" alt="Irrigation lines 2" width="150" style="margin: 5px;">
</a>

<a href="images/IMG_20200403_151122_01.jpg">
  <img src="images/IMG_20200403_151122_01.jpg" alt="Irrigation lines 3" width="150" style="margin: 5px;">
</a>

<a href="images/USER_SCOPED_TEMP_DATA_MSGR_PHOTO_FOR_UPLOAD_1586541424154.jpeg">
  <img src="images/USER_SCOPED_TEMP_DATA_MSGR_PHOTO_FOR_UPLOAD_1586541424154.jpeg" alt="Patio setup 1" width="150" style="margin: 5px;">
</a>

<a href="images/IMG_20200329_185708862.jpg">
  <img src="images/IMG_20200329_185708862.jpg" alt="Patio setup 2" width="150" style="margin: 5px;">
</a>
