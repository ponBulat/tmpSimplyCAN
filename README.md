# tmpSimplyCAN

simplyCAN from Ixxat работает как serial port через собственную библиотеку simplyCAN.so

В ubuntu 20.04/21.04/21.10 устройство USB simplyCAN определяется как /dev/ttyACM0

По умолчанию запись и чтение ttyACM0 возможно только под sudo.

Чтобы была возможность записи и чтения под любым пользователем можно изменить правила udev, например вот так:
```bash
sudo gedit /etc/udev/rules.d/7-bulat.rules
```
в открывшемся окне записать:
```
KERNEL=="ttyACM0", ATTRS{manufacturer}=="IXXAT", MODE="0666"
```
Закрыть окно и перезагрузить ПК.
