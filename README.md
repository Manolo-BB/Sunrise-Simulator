# Sunrise-Simulator
The aim of this project is to create a sunrise simulator to replace my mobile phone as an alarm clock. It works independently, without the need for an app.

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
- Microcontroleur (ESP32)
- Module RTC
- Hauts parleurs
- Ecran d'affichage
- Boutons
- Leds
- Module de gestion de la puissance

