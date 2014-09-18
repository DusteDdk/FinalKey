#!/bin/bash

# License: WTFPL ( http://www.wtfpl.net/ )

if [ "$2" != "" ]
then
  FK_ROOT_DIR="$2"
fi

UDEVNAME="$FK_ROOT_DIR/etc/udev/rules.d/80-FinalKey.rules"
BINNAME="$FK_ROOT_DIR/usr/bin/finalkey"
BCKBINNAME="$FK_ROOT_DIR/usr/bin/finalkeybackup"
ICONNAME="$FK_ROOT_DIR/usr/share/pixmaps/finalkey.png"
XICONNAME="$FK_ROOT_DIR/usr/share/pixmaps/finalkey.xbm"
DESKTOPNAME="$FK_ROOT_DIR/usr/share/applications/finalkey.desktop"
GUIDESKTOPNAME="$FK_ROOT_DIR/usr/share/applications/finalkey-gui.desktop"
SHAREDIR="$FK_ROOT_DIR/usr/share/finalkey"
GUINAME="$SHAREDIR/GUI.jar"

UNMET=""

if [ "$FK_ROOT_DIR" != "" ]
then
  echo "Using: $FK_ROOT_DIR as root directory."
  mkdir -p "$FK_ROOT_DIR/etc/udev/rules.d/"
  mkdir -p "$FK_ROOT_DIR/usr/bin/"
  mkdir -p "$FK_ROOT_DIR/usr/share/pixmaps/"
  mkdir -p "$FK_ROOT_DIR/usr/share/applications/"
  mkdir -p "$SHAREDIR"
fi

echo "Checking dependencies..."
if ! which minicom &> /dev/null
then
  UNMET="  minicom\n"
fi

if ! which xterm &> /dev/null
then
  UNMET=$UNMET"  xterm\n"
fi

if ! which setsid &> /dev/null
then
  UNMET=$UNMET"  setsid\n"
fi

if ! which gunzip &> /dev/null
then
  UNMET=$UNMET"  gzip\n"
fi

if ! which base64 &> /dev/null
then
  UNMET=$UNMET"  base64/coreutils\n"
fi

if ! which lsof &> /dev/null
then
  UNMET=$UNMET"  lsof\n"
fi

if ! which gcc &> /dev/null
then
  UNMET=$UNMET"  gcc (Debian/Ubuntu: build-essential, Archlinux: base-devel)"
fi

if [ "$UNMET" != "" ]
then
  echo "There are unmet dependencies, please install:"
  echo -e "$UNMET"
  echo "Installation aborted."
  exit 1
else 
  echo "All required programs available."
fi

if [ `whoami` != "root" ]
then
  sudo bash $0 $USER $FK_ROOT_DIR
exit
else
  if [ "$1" == "" ]
  then
    echo "You should run this as the user you want to use The Final Key with, alternative: sudo $0 USERNAME"
    exit 1
  fi
fi

#The base64 encoded data that putFile takes as first argument are created with: gzip -c THEFILENAME | base64
function putFile
{
  echo $1 | base64 -d | gunzip > $2
}

#Source code for the backup utility, how elegant!
putFile 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\
Sgpr6zUAAA== "/tmp/fkbck.c"

echo "Building the backup utility."
if ! gcc --std=c99 -o "$BCKBINNAME" "/tmp/fkbck.c"
then
  echo
  echo "Could not build the backup utility."
  echo "When the problem is resolved, re-run $0 to enable backup."
  echo "The installation will continue, but the backup feature will not work."
else
  echo "Done."
  chown $1 $BCKBINNAME 
  chmod u+x $BCKBINNAME
fi
rm -f /tmp/fkbck.c

# The "finalkey" script which opens an xterm with minicom in it.
putFile H4sICIfi6VMAA2ZpbmFsa2V5AL1V227aQBB9jr/iZINI0soYylskWlXNpVGiNmpo8xCiZjFjvGFZ\
W14TQMrHd3wBbFKqqg+1EOxl5szMmePhYN8bKuMNpQ0d5wDXyidj6QR3/fObaxwhTNP4xPPm83lr\
ngaxbhlKPRw7zvnV6dmP3mNCcqSVmcAb0bN3rozUV7R8dFSAezQ+YL+HNh6cNCTjAIuUkinc4RgJ\
jeAGYwy19Cdw+xBfohR+ZAz5KY0E3koFd8HWh28UH9+oxVTGJ/BmNvFsKBPy4vzIekEWdELL1mI4\
PYRLEOSHUbYYiMHAFB8MBu1u974zBT/9kJCnCs4VysJUYyONwPnCLm1K01YJIdBswmqiGJ22cEhb\
4oK4TG2jAK7BC8YJXzZyYtB8XzBiZlqzXVl/lQG2JrPFwUed8bn8jzwklNUvy7iSieCzVXwVmYyM\
Gl07+ABKRnJO7iEaHYFeD2LItc1igQe8vFTPE7JplBBf5E5rgnL/fcxD5YdYlVSAbJO65ccpZPWK\
LN3SQZoRykAIZiaviKvlXdZxZWwqteaOZ3WGzMNU+qEy1BJbkJeFJca+X2K6ycwgmFhKZ3HLhhWH\
hUrRKbeBKhfrfa2eRgeNd2h0q0ak6/yNZ+r3HBUMPclnuZOXvdu7/s+L/lW31wZnatWosHefZAJR\
kdAqL+/i+2WLL0UNEk1nLy+rzb9Zl/cKVtgYhmhkC9QjrSakl/m7Y3iOIAoC5SuWTWSY/iSa4msi\
fU3Hok5PhrlSzYHgd4nymbFVch6zuhYbXX4mHYvanVt76nebPqD2uPhU6P6V6Pmqz+9ttt0JxY36\
S6iLRMbcvD9glfo4v7w+y7Bu5fNa0oyXHbd2+q70nju7+FZuS/e8Da8BVuuNeNfSLYWzGVzluOLB\
xRJM6d8nVL/aQqZGZLOKSWYUjsST67T+r8I6XGfGX/z5BV5J/1O+BgAA $BINNAME

chown $1 $BINNAME
chmod u+x $BINNAME

# The Icon
putFile H4sIACvJ9lIAA+sM8HPn5ZLiYmBg4PX0cAkC0gogzMEGJIurqr4AKbYkb3cXBjD46by7HkhxFnhE\
FjMwcAuDMCPDrDkSQEH2Ek9fV/Z7TJwMvKqyTYnyQCHZzJCIEuf83NzUvBKwfuei1MSS1BSF8syS\
DAV3T9+AFL1UdqD4MU8Xx5CIw2+nv+ZqUOBpPS94/79tdddXdcVFt3wPKzkKCH5/++hZwwMBQUaF\
20GLvk9pKSjNB9o5wSa22v1sRm15m5tWVUu0aa6MGAeQvrvHfrXjhu0F+izuHN8dRF6z/t5rtlVc\
USnrzpUEOQNm/pPM6VGusaFVkQeS3q2b+YHrDHdgVeb6o8eVxSq2SDAecEryFFmkqVMwm9M48sMP\
G+eZBb8kfjDaMQYYX8xdOWE/9z1Z5rV7skQvfF5afX59Rde+hK5zGrf+RlUfFt0lOIuRp26uCcir\
nq5+LuucEpoAEdVnhmIBAAA= $ICONNAME
chmod a+r $ICONNAME

# Icon for xterm
putFile H4sIAHHT9lIAA7VSQQrCMBC89xULvfZQLFJFfImItMmmCUoPNqIi/t2mKTHZGrEHL0OYmZ0MyaYc\
hWoReqhOR7wfroprCcUiSakiUTVSG6nTlVYMLm2nmhY5MFmd375a6W63hy08EgDIb0JkPZYDCswc\
850v7JmNjB9FJId5aRBzyovaIBcfogQ3WIvJCHrdvJ6rAZGPHhtlJbTmJR1nll9TnvinUe4W0iTW\
0PF+VPC83kg5P+rH2/8RFfx4GEWkuXywDLEmsSWJ71V0FQOE5yZ5AVvZ0g6BAwAA $XICONNAME
chmod a+r $XICONNAME

# .desktop files so it will appear in the menu.
putFile H4sICKWtGlQAA2ZpbmFsa2V5LmRlc2t0b3AAPcs7DsIwDADQ3afICAsXQBlQKR9VZSoTYrBSU6w2\
dpRYKrk9YmF8w3scqcymybViuT5hqIn8IaWFAxqrwA0j+eFN7sSCi+uouk3PwkHjFhqNkcT8BfO4\
YiaXsJRV8+giCk6Uof1Q8K9fnanCNaj8tUsyQYNGk2am4s9Dt78bL2wV4Atr1YqwmAAAAA== $DESKTOPNAME
chmod a+r $DESKTOPNAME

putFile H4sICLOtGlQAA2ZpbmFsa2V5LWd1aS5kZXNrdG9wAD3LsQ6CMBAA0P2+oqMu/oDpYBCRkDDBZBwu\
cNYL9Nq0Z7B/ry6uL3m3M+VFQzS1aCp3GEoke4px5QmVg0CPnuzwJHNhwdV0VMyuGds9VMF7ErVX\
TPOGiUzEnLeQZuNR0FGC+k2Tffza8l3uxdBOQf5yiOKgQiUXElO2zdAdR+WVtQB8AHNUDRCYAAAA $GUIDESKTOPNAME
chmod a+r $GUIDESKTOPNAME


#The udev file
echo SUBSYSTEMS==\"usb\", KERNEL==\"ttyACM*\", ATTRS{idVendor}==\"2341\", ATTRS{idProduct}==\"8036\",SYMLINK+=\"FinalKey\",GROUP=\"$1\" OWNER=\"$1\" > $UDEVNAME
echo SUBSYSTEMS==\"usb\", KERNEL==\"ttyACM*\", ATTRS{idVendor}==\"1d50\", ATTRS{idProduct}==\"60a6\",SYMLINK+=\"FinalKey\",GROUP=\"$1\" OWNER=\"$1\" >> $UDEVNAME


if uname -a | grep -i x86_64 &> /dev/null
then
  echo "Downloading gui..."
  mkdir -p $SHAREDIR
  if wget -q --no-cache http://finalkey.net/gui/linux-current.jar -O $GUINAME
  then
    echo "Done."
  else
    echo "Could not download the GUI."
  fi

  if ! which java &> /dev/null
  then
    echo "java was not found, the GUI won't work."
    echo "Install java if you wish to use the GUI, or"
    echo "use the 'finalkey' command instead of 'finalkey gui'"
  fi
else
  echo "The GUI is only available on linux x86_64."
fi

  
echo -e "\nThe following files were installed:\n$ICONNAME\n$XICONNAME\n$DESKTOPNAME\n$GUIDESKTOPNAME\n$BINNAME\n$BCKBINNAME\n$UDEVNAME"

if [ -f "$GUINAME" ]
then
  echo $GUINAME
fi


echo " "

if which udevadm &> /dev/null
then
  udevadm control --reload-rules &> /dev/null
elif which restart &> /dev/null
then
  restart udev &> /dev/null
else
  echo "Restart udev or computer for changes to take effect."
fi

echo -e "If you have The Final Key plugged in at the moment, you need to unplug-\nand insert it again before you can use it.\n\nThe Final Key installed for user: $1.\nRun: finalkey --help to begin using it.\n"

