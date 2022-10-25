# FEED WAGON CONTROLLER
This is the second hardware part of the feed wagon controller project. This is the gateway. It receives information using LoRa from a device that is in the feed wagon. 

The information received is:
* Battery voltage
* Motor voltage
* Status of reed sensor

The information is sent every second. The gateway analyse the data to find errors and send it to a MQTT broker. Moreover, it is shown in a oled screen. When a problem is detected, a buzzer will sound until a button is pressed. 


And more, much more...


