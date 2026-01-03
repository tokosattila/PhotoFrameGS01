# PhotoFrameGS01

> E-Paper Digital Photo Frame with Deep Sleep & Remote Management

## 🔧 Hardware

| Component | Specification |
|-----------|--------------|
| **Board** | LilyGo T5 4.7" E-Paper |
| **MCU** | ESP32-WROVER-E |
| **Display** | ED047TC1 (960×540, 16 grayscale) |
| **Flash** | 16MB |
| **PSRAM** | 4MB |
| **Battery** | Li-Ion 18650 (optional) |

## ✨ Features

- **Photo Display** — JPEG slideshow from LittleFS
- **Deep Sleep** — Configurable wake intervals (minutes to daily)
- **Maintenance Mode** — Button-triggered for configuration
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
│   ├── FileSystem.cpp/h        # LittleFS operations
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
│   ├── TimeDate.cpp/h          # NTP & RTC
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
├── test_FileSystem/            # FileSystem utility tests
├── test_Telnet/                # Telnet command tests
├── test_TimeDate/              # Date/time utility tests
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
[wifi]
ssid = YourNetwork
password = YourPassword

[display]
interval = 60        ; minutes between image changes
brightness = 100     ; 0-100%

[services]
ftp_enabled = true
ftp_user = admin
ftp_pass = admin
telnet_enabled = true
telnet_port = 23
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
