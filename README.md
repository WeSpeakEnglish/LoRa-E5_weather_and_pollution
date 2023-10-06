# This PolluTAG based on LoRa E5 mini modules from seed.
It is custom FW, so you need to erase the STM32 chip first [Erase Factory AT Firmware](https://wiki.seeedstudio.com/LoRa_E5_mini/#21-erase-factory-at-firmware), you may need to retrieve and save the Device EUI before erasing of the Factory AT Firmware before we program with SDK

# Sensors for this custom build
1. SHT40 for humidity and temperature over I2C2 
2. ZE27-O3 Ozone sensor over USART2
3. SN-GCJA5 PM sensor over I2C2 


## Whole PRJ is ready to compile in CubeIDE
*LoRa-E5_weather_and_pollution.ioc* is configuration file for CubeMX
*decoder.js* - is decoder for payload for datacake - just copy this JS code into decoder section

### It is sending the payload over LoRaWAN appx. every 2.5 mins
Usually it takes up to 10 min. to react on downlink command (depend of communication stability)

### Downlink port #2
Encoder for downlink (measurements.LED_ON is interface fild on datacake):
```javascript 
function Encoder(measurements, port) {
    if(measurements.LED_ON.value) return [1];
    else return [0];
}
```