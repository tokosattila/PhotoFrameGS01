# PhotoFrameGS01

E-ink digital picture frame with remote image updates via FTP and configuration settings adjustable through Telnet. Features deep sleep mode for extended battery life.

> **📌 Note:** This project is designed for the older LilyGo T5 4.7" E-Paper (WROVER-E). For the newer **LilyGo T5 4.7" E-Paper Plus** version, check out [PhotoFrameGS02](https://github.com/tokosattila/PhotoFrameGS02.git).

## 🔧 Hardware

| Component | Specification |
|-----------|--------------|
| **Board** | LilyGo T5 4.7" E-Paper |
| **MCU** | ESP32-WROVER-E |
| **Display** | ED047TC1 (960×540, 16 grayscale) |
| **Flash** | 16MB |
| **PSRAM** | 4MB |
| **Battery** | Li-Ion 18650 (optional) |

## 🔄 Operating Modes

| Mode | Description |
|------|-------------|
| **Photo Frame Mode** | JPEG slideshow from LittleFS with grayscale rendering, deep sleep between wake intervals (minutes to daily) |
| **Maintenance Mode** | Button-triggered mode for configuration & remote management |
| **Low Battery Mode** | Auto shutdown with battery icon display |

## ✨ Features

- **WiFi Connectivity** — NTP time sync, remote image fetch
- **FTP Server** — Upload/manage images wirelessly
- **Telnet Console** — Remote monitoring & commands
- **Battery Monitoring** — Auto low-power mode
- **Grayscale Rendering** — 16-level dithering for photos

## 📁 Project Structure

```
src/
├── Main.cpp                    # Application entry point
├── App/
│   ├── Button.cpp/h            # Debounced button handling
│   ├── Configuration.cpp/h     # INI config management
│   ├── Connection.cpp/h        # WiFi management
│   ├── Display.cpp/h           # E-Paper driver wrapper
│   ├── LittleFs.cpp/h          # LittleFS operations
│   ├── FTP.cpp/h               # FTP server
│   ├── Global.h                # Global definitions & macros
│   ├── Telnet.cpp/h            # Telnet console
│   ├── Telnet/
│   │   ├── Command.h           # Base command interface
│   │   └── Commands/           # Telnet command implementations
│   │       ├── BatInfoCommand.h
│   │       ├── CatCommand.h
│   │       ├── ClearCommand.h
│   │       ├── ConfigCommand.h
│   │       ├── DateCommand.h
│   │       ├── ExitCommand.h
│   │       ├── FetchCommand.h
│   │       ├── FileSystemInfoCommand.h
│   │       ├── HelpCommand.h
│   │       ├── ListCommand.h
│   │       ├── LogoutCommand.h
│   │       ├── MemInfoCommand.h
│   │       ├── NetInfoCommand.h
│   │       ├── NvsInfoCommand.h
│   │       ├── RebootCommand.h
│   │       ├── ResetCommand.h
│   │       ├── SketchInfoCommand.h
│   │       └── TimeStampCommand.h
│   ├── NTP.cpp/h               # NTP time synchronization
│   └── Utils.cpp/h             # System utilities
├── Fonts/                      # OpenSans bitmap fonts (6-26pt)
│   └── opensans*.h             # 26 font variants
└── Images/
    └── DefaultImage.h          # Default fallback image

test/
├── mocks/                      # Mock classes for testing
│   ├── MockString.h
│   └── MockWiFiClient.h
├── test_Button/                # Button unit tests
├── test_Configuration/         # Configuration parser tests
├── test_FetchCommand/          # Fetch command tests
├── test_LittleFs/              # LittleFS utility tests
├── test_Telnet/                # Telnet command tests
├── test_NTP/                   # NTP time utility tests
├── test_Utils/                 # Utility function tests
└── test_Wrappers/              # Type wrapper tests
```

## 🛠️ Build

### Requirements
- [PlatformIO](https://platformio.org/)
- ESP32 toolchain

### Commands
```bash
# Build
pio run

# Upload
pio run -t upload

# Upload filesystem
pio run -t uploadfs

# Run tests (native)
pio test -e native

# Monitor
pio device monitor
```

## ⚙️ Configuration

Place `config.ini` in LittleFS root:

```ini
[device]
appname = PHOTO FRAME GS01
version = v1.0

[display]
jpg_brightness = 30      ; 0-100%
jpg_contrast = 35        ; 0-100%
jpg_gamma = 135          ; gamma correction
image_file =             ; current image file
images_dir = /images     ; images directory

[ntp]
ntp_server = pool.ntp.org
ntp_port = 123
ntp_gmt_offset = 7200    ; GMT+2 in seconds
ntp_update = 60000       ; update interval ms

[ap mode]
ap_enable = true
ap_ssid = PhotoFrameGS01
ap_password = 123456789
ap_ip = 192.168.4.1
ap_gateway = 192.168.4.1
ap_subnet = 255.255.255.0

[sta mode]
sta_ssid = YourNetwork
sta_password = YourPassword

[static ip]
sta_enable = false
sta_ip = 192.168.0.83
sta_gateway = 192.168.0.1
sta_subnet = 255.255.255.0
sta_dns1 = 192.168.0.1
sta_dns2 = 8.8.8.8

[mdns]
mdns_enable = false
mdns_hostname = photoframegs01

[timer]
wake_up = 4              ; 0=Off, 1=1min, 2=5min, 3=30min, 4=Daily; 5=Weekly; 6=Nonthly

[telnet]
telnet_enable = true
telnet_port = 23
telnet_username = admin
telnet_password = 123456789
telnet_session = 3600000 ; session timeout ms

[ftp]
ftp_enable = true
ftp_port = 21
ftp_username = admin
ftp_password = 123456789
```

## 📡 Telnet Commands

| Command | Description |
|---------|-------------|
| `help` | Help information |
| `clear` | Clear display |
| `list` | List directories and files |
| `cat <filename>` | Show file content |
| `date` | Show current date and time |
| `timestamp` | Show current timestamp |
| `nvsinfo` | Show NVS usage info |
| `meminfo` | Show memory usage info |
| `sketchinfo` | Show sketch usage info |
| `fsinfo` | Show filesystem usage info |
| `netinfo` | Show network info |
| `batinfo` | Show battery voltage and percentage |
| `config <key> [value]` | Get or set config value |
| `fetch <url> [filename]` | Download image (max. 200kB, type: *.jpg, *.jpeg) |
| `reset config` | Factory reset config |
| `reboot` | Restart device |
| `logout` | Logout telnet session |
| `exit` | Exit telnet |

## 🔌 Pin Configuration

| Pin | Function |
|-----|----------|
| GPIO39 | Button 1 |
| GPIO34 | Button 2 |
| GPIO36 | Button 3 / Battery ADC |
| GPIO0 | Button 4 (Boot) |

## 📦 Dependencies

- [LilyGoEPD47](https://github.com/Xinyuan-LilyGO/LilyGo-EPD47) — E-Paper driver
- [JPEGDEC](https://github.com/bitbank2/JPEGDEC) — JPEG decoder
- [SimpleFTPServer](https://github.com/xreef/SimpleFTPServer) — FTP server
- [ArduinoHttpClient](https://github.com/arduino-libraries/ArduinoHttpClient) — HTTP client
- [Unity](https://github.com/ThrowTheSwitch/Unity) — Unit testing

## 📄 License

MIT

---

**Author:** Szeklerman  
**Hardware:** LilyGo T5 4.7" E-Paper / ESP32-WROVER-E
