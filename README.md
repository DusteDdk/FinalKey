Firmware for the Final Key - A hardware password manager for Linux, Windows and OSX with encryption.
==============
This software comes without any kind of warranty, and nobody is responsible.<br>
Instructions: http://cyberstalker.dk/finalkey/
- ArduinoHacks are for ARDUINO 1.5.5 BETA 2013.11.28<br>
- The diff is for 1.0.5 (du'h, it's already in the name..).<br>
- The finalkeybackup.c is a linux tool to allow taking backup of your key (lock the key with q and close minicom before trying that).<br>
- The fksetup.sh script can be run on Ubuntu/Debian/Archlinux and will fix permissions to device and add /dev/FinalKey rule (for all leonardo boards, no incompatibilities with existing Arduino setups though).

Credits:
--------------
<pre>
Directory                       - Description           - Author                - License       - URL
FinalKey                        - Firmware for FinalKey - DusteD                - WTFPL         - http://cyberstalker.dk/finalkey/
libraries/EncryptedStorage      - Data store/retrieve   - DusteD                - WTFPL         - http://cyberstalker.dk/finalkey/
libraries/I2ceep                - I2C EEPROM access     - DusteD                - WTFPL         - http://cyberstalker.dk/finalkey/
libraries/TermTool              - Lazy mess, macros     - DusteD                - WTFPL         - http://cyberstalker.dk/finalkey/
libraries/AES                   - AES Encryption        - MarkT / Brian Gladman - Header: AES.h - http://forum.arduino.cc/index.php/topic,88890.0.html http://utter.chaos.org.uk/~markt/AES-library.zip
libraries/Entropy               - Random Number Gen.    - Walter Anderson       - GPL v3        - http://code.google.com/p/avr-hardware-random-number-generation/
ArduinoHacks                    - Changes to Arduino    - DusteD / Arduino      - LGPL          - http://arduino.cc
</pre>

