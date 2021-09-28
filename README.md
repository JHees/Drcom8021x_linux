# Drcom8021x_linux
forked from https://github.com/sunke-github/Drcom8021x_linux

## 声明:
Drcom8021x 软件是根据 EasyDrcom软件重构而来,该源码地址:https://github.com/coverxit/EasyDrcom .
本软件遵循GPL v3.0,使用本软件源码请注明原作者,来源和出处.
## 免责声明:
该软件是做交流学习使用,请勿用作非法用途. 使用本软件带来的法律问题由使用者付全责与软件作者无关.

## Dependencies
* pcap
* clipp
## Build and install:
### install dependency:
```bash
# pcap
apt install libpcap-dev
# clipp
git clone https://github.com/muellan/clipp.git
cd clipp
cmake .
make
sudo make install
```

### Compile the code:
```bash
mkdir build
cd build
cmake ..
make
```
If u like to install in the system path, u need to compile in release:

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
```

### Systemd service
Using the persistent [systemd](https://wiki.archlinux.org/index.php/systemd#Basic_systemctl_usage) service

Start service immediately:

```
sudo systemctl start drcom8021x
```

Start on boot:
```
sudo systemctl enable drcom8021x
```

