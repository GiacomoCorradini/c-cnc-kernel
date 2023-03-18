# Goodies

This forder contains useful stuff.

## MQTT

### Running the broker

The MQTT broker can be launched using the Docker container `eclipse-mosquitto`. Note that it must be launched from the PowerShell console and **not** from the WSL console, for the latter runs a virtual machine with its own network that is not reachable from the outside. So, open the PowerShell in this folder and type:

```PowerShell
docker run -it --rm -p1883:1883 -v"${PWD}/mosquitto.conf:/mosquitto/config/mosquitto.conf" eclipse-mosquitto
```

Note that that the `mosquitto.conf` file must be in the current directory.

### Installing `libmosquitto`

We are going to build a client for the MQTT broker. To do so, we are using a well-known library named `libmosquitto`. The library could be installed with the Ubuntu package manager as `sudo apt install libmosquitto-dev`. Unfortunately, this command installs a pretty old version of the library, version 1.6.9 at the time of writing, while the current version is 2.0.14.

Consequently, we are going to download the source of the latest version and compile it. Proceed as follows, in a WSL terminal:

```sh
cd ~
wget https://mosquitto.org/files/source/mosquitto-2.0.14.tar.gz
tar xzf mosquitto-2.0.14.tar.gz
cd mosquitto-2.0.14
cmake -Bbuild . -DCMAKE_BUILD_TYPE=Release -DWITH_TLS=OFF -DDOCUMENTATION=OFF -DWITH_LIB_CPP=OFF -DWITH_SRV=OFF -DWITH_CJSON=OFF
cmake --build build
sudo cmake --install build
```

Now you can verify that the installation went fine with the command:

```sh
mosquitto_sub --help
```