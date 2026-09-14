# Sunrise-Simulator
The aim of this project is to create a sunrise simulator to replace my mobile phone as an alarm clock. It works independently, without the need for an app. A second aim is to be cheaper than the alarm clocks currently available on the market, whilst still incorporating all the features I want.

## License

This project is licensed under the CC BY-NC-SA 4.0 License.

You may use, modify and distribute this project for non-commercial purposes.
Commercial use is not permitted.

See the [LICENSE](./LICENSE) file for details.

## Features

### General
- Run on main power and a backup battery
- Permanent time display + backlight when a button is pressed (The time is updated every minute via the RTC module. The local time is updated via Wi-Fi every 24 hours at 3:30am to take account of time changes)

### Alarm clock
- Set the alarm time
- Select the wake-up interval (default 30 mins)
- Choose an alarm tone (selection of 5 sounds)
- Alarm scheduling feature (weekdays/weekends)
- Ability to turn off the alarm before it ring
- Ability to set multiple alarms simultaneously

### lighting
- Switching on the light in bedside lamp mode
- Selecting easily the light 
- Selecting the brightness level

## Material
The idea is to reuse personal items rather than buying new ones; that is why some components may not be optimised. The whole project is designed to work with a 12V LED strip.

### Hardware

| Function                       | Component                                              |          Price |
| ------------------------------ | ------------------------------------------------------ | -------------: |
| Microcontroller / Main Control | ESP32 DevKit V1                                        |          €5.99 |
| Lighting                       | WS2812B LED Strip                                      | Personal stock |
| Power Supply                   | 12 V – 1.5 A AC/DC Power Supply                        |    Second-hand |
| Power Connector                | DC Female PCB Connector                                |          €0.32 |
| Voltage Regulation             | MP1584EN Buck Converter                                |          €1.15 |
| **Audio / Alarm**              | MAX98357A Audio Amplifier                              |          €1.72 |
|                                | 4 Ω – 3 W Speaker                                      | Personal stock |
|                                | Micro SD / TF / SD Mini Card Reader – SPI, 3.3 V / 5 V |          €2.07 |
| **Display**                    | 2.9" E-Ink / E-Paper Display – SPI https://github.com/WeActStudio/WeActStudio.EpaperModule/tree/master                    |         €13.54 |
| **Backlight**                  | 2 White LEDs + Diffuser                                | Personal stock |
| **Navigation**                 | Bipolar Toggle Switch                                  |          €1.50 |
|                                | 4 Momentary Push Buttons                               |          €8.53 |
| **RTC**                        | DS3231 Real-Time Clock Module                          |          €6.79 |
| **Power Path Controller**      | MCP73871                                               |          €4.90 |
|                                | 18650 Battery                                      |         €12.90 |
|                                | Battery Adapter                                        |          €1.02 |
|                                | **Total**                                              |      **€62.42** |