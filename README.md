Firmware for the Final Key - A hardware password manager for Linux, Windows and OSX with encryption.
==============
This software comes without any kind of warranty, and nobody is responsible.<br>
Instructions: http://finalkey.net/
- A hacked version of arduino is needed, it can be found here: https://github.com/DusteDdk/ArduinoIDE_1.6.3-FinalKey<br>
- The FinalKey can be used with minicom, or putty and is quite convnient to use this way.
- There is also a GUI available, the source is here: https://github.com/DusteDdk/FinalKeyGui
- The finalkeybackup.c is a linux tool to allow taking backup of your key (lock the key with q and close minicom before trying that).<br>
- The fksetup.sh script can be run on Ubuntu/Debian/Archlinux and will fix permissions to device and add /dev/FinalKey rule (for all leonardo boards, no incompatibilities with existing Arduino setups though).
- There is much more info on the website!

Credits:
--------------
<pre>Directory                       - Description           - Author                - License       - URL
FinalKey                        - Firmware for FinalKey - DusteD                - GPL v3        - http://finalkey.net/
libraries/EncryptedStorage      - Data store/retrieve   - DusteD                - GPL v3        - http://finalkey.net/
libraries/I2ceep                - I2C EEPROM access     - DusteD                - GPL v3        - http://finalkey.net/
libraries/TermTool              - Lazy mess, macros     - DusteD                - GPL v3        - http://finalkey.net/
libraries/AES                   - AES Encryption        - MarkT / Brian Gladman - Header: AES.h - http://forum.arduino.cc/index.php/topic,88890.0.html http://utter.chaos.org.uk/~markt/AES-library.zip
libraries/Entropy               - Random Number Gen.    - Walter Anderson       - GPL v3        - http://code.google.com/p/avr-hardware-random-number-generation/
ArduinoHacks                    - Changes to Arduino    - DusteD / Arduino      - LGPL          - http://arduino.cc

File                            - Description           - Author                - License       - URL
finalkeybackup.c                - Backup program        - DusteD                - WTFPL         - http://finalkey.net/
</pre>

Donate to FinalKey project
===
If you enjoy having your passwords safely encrypted on an external device, consider donating a few Dogecoin to my wallet at: DHu41ELvTho6SUiuNDnyQYdDmDrYiWZuvR
