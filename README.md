# Aabenraa Organ Installation Controller


## Physical Installation
### Controller assembly
[Controller assembly Drawing v7.pdf](https://github.com/smukkejohan/aabenraa-gpac-controller/files/10342706/Controller.assembly.Drawing.v7.pdf)

### Installation
[give peace a chance main installation drawing v5.pdf](https://github.com/smukkejohan/aabenraa-gpac-controller/files/10342707/give.peace.a.chance.main.installation.drawing.v5.pdf)

### Site
![MålPassagen](https://user-images.githubusercontent.com/38772/210519919-574ac5c0-9e48-4860-a961-9906abb37edd.jpeg)

## Operating Manual
### Setting the clock and manual activation
![aabenraa-orgel-menu-navigation](https://user-images.githubusercontent.com/38772/210520046-7f6a06d0-f011-4fd7-808e-e96ac831d977.png)

### Changing battery
The coin cell battery enables the controller to keep its clock synced during a power outage. Depending on circumstances the battery need to be changed every few years, to ensure this feature keeps working. Replace with a new coin cell battery of same type.  

## Software Development
Checkout this project and open it in Visual Studio code with platform IO. Make sure boards and libraries are installed as defined in the platformio.ini file. The controller uses a teensy41.

### Uploading new firmware
1. Disconnect 5V supply cable from controller or disconnect mains power to the PSU's using the G1 and G2 breakers. 
2. Connect controller to your development computer with a microusb cable. 
3. Upload code using platform io. 
4. Disconnect usb cable. 
5. Reconnect 5V supply. 

Take appropriate precautions if uploading new firmware without disconnecting the mains power.


