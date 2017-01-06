# DeviceDriver
## OverView
[led_fire/](https://github.com/Ryou-Watanabe/DeviceDriver/tree/master/led_fire) - 名前は燃えているが燃えないのでご安心を。1秒ごとにLEDが点灯する。また、1~9(秒/分)を計測することができる。指定された時間が経つとLEDが点灯によって通知する。カップラーメン計測用として作成した。個人的には重宝する(?)。

## Demo
[Youtube](https://youtu.be/E_c5L9rQi_g)

## Requirement
* Linux raspberrypi 4.4.22-v7+
* Raspberry Pi 3

回路に関しては以下を参照。GPIO25 PinとGroundを接続。

![回路図](https://github.com/Ryou-Watanabe/DeviceDriver/blob/images/robosys_img1.png)


## Installation
`$ git clone https://github.com/Ryou-Watanabe/DeviceDriver.git`

## Usage
```
$ cd ./DeviceDriver/led_fire/
$ make
$ sudo insmod led_fire.ko
$ sudo mknod /dev/ledtimer c 243 0
$ sudo chmod 666 /dev/ledtimer
```

### Example
* 5分設定にする場合  
 * 分設定にする  
`$ echo m > /dev/ledtimer`  
 * その後、5分設定にする  
`$ echo 5 > /dev/ledtimer`

* 3秒設定にする場合  
 * 秒設定にする  
`$ echo s > /dev/ledtimer`  
 * その後、3秒設定にする  
`$ echo 3 > /dev/ledtimer`

## Licence
[MIT](https://github.com/Ryou-Watanabe/DeviceDriver/blob/master/LICENSE)

## Author
[Ryou Watanabe](https://github.com/Ryou-Watanabe)
