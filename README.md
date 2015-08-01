# WLED

Wireless LED bars!

I've got this set of [Revoltec LED backlight bars](http://www.amazon.de/Revoltec-Backlight-SMD-15-Entertainment-Beleuchtung/dp/B00405D7DS/ "No affiliate link."). They're meant as a form of static ambilight mounted behind a screen. The bars are normally controlled via a small IR remote control with annoying membrane keys. Finding the RGB color I want *right now* by pressing those keys endlessly for each color channel was no great joy either. Therefore I use a Carambola 2 as... well... GPIO-Wifi adapter. :) This way I can even implement different color models (HSV!).

To put the C2 in control, the ATMega8 was removed from the original PCB and I soldered the C2's GPIOs to the corresponding lanes where that ATMega used to give the orders. As they're both working with 3.3V levels, everything is fine and I can even power the C2 through the original 12V power supply (and a voltage converter). The PCB now receives three PWM signals (RGB) and converts them to 12V for the bars. I'm not sure if USB connectors are a very wise choice for that. Plugging a USB stick in there will probably smell pretty funny.

Thanks to [Mathias Gottschlag](https://github.com/mgottschlag) for providing me with his C2 and lots of hardware knowledge. (I'm really more of a software guy...)

![Flicker-free!](img/2015-07-22 flicker-free.jpg)

# Installation

You need to copy `src/web/framework7` to `host:/www/wled` manually once. After that, `make && make web` installs everything else to the right locations. This is done so that subsequent `web` target calls don't take so long. The web interface will then be available at http://host/wled.

For wledd to start automatically, you'll need to `/etc/init.d/wledd enable` to create the necessary symlinks.

*Hint*: Bookmarking the web interface on the iOS home screen makes it behave quite like a native app.
