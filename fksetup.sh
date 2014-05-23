#!/bin/bash

if !which minicom &> /dev/null
then
  UNMET="minicom "
fi

if !which xterm &> /dev/null
then
  UNMET=$UNMET"xterm "
fi

if !which setsid &> /dev/null
then
  UNMET=$UNMET"setsid "
fi

if !which gunzip &> /dev/null
then
  UNMET=$UNMET"gzip "
fi

if !which base64 &> /dev/null
then
  UNMET=$UNMET"base64/coreutils "
fi

if !which lsof &> /dev/null
then
  UNMET=$UNMET"lsof "
fi

if [ "$UNMET" != "" ]
then
  echo "Before installing The Final Key, please install the following:"
  echo "$UNMET"
  echo "Installation aborted."
  exit 1
fi

if [ `whoami` != "root" ]
then
  sudo bash $0 $USER
exit
else
  if [ "$1" == "" ]
  then
    echo "You should run this as the user you want to use The Final Key with, alternative: sudo $0 USERNAME"
    exit
  fi
fi

UDEVNAME=/etc/udev/rules.d/80-FinalKey.rules
BINNAME=/usr/bin/finalkey
ICONNAME=/usr/share/pixmaps/finalkey.png
XICONNAME=/usr/share/pixmaps/finalkey.xbm
DESKTOPNAME=/usr/share/applications/finalkey.desktop

echo SUBSYSTEMS==\"usb\", KERNEL==\"ttyACM*\", ATTRS{idVendor}==\"2341\", ATTRS{idProduct}==\"8036\",SYMLINK+=\"FinalKey\",GROUP=\"$1\" OWNER=\"$1\" > $UDEVNAME

function putFile
{
  echo $1 | base64 -d | gunzip > $2
}

#The binaries are created with: gzip -c THEFILENAME | base64

# The "finalkey" script which opens an xterm with minicom in it.
putFile H4sICLkMf1MAA2ZpbmFsa2V5AL2SwUrDQBRF9/mK27GkoKTT0p1QRajdFMRFcdMITpKXZuhkUjJT\
TcCP96WN0ApuXBhmEV4ec+855GogE21lolwRLFeLx5f5W00qM9ruIDN6l0ttlVlR+xboHBsM7zGY\
Y4LXwBdkA6DxVJeIki1qyhDlWyRGpTtEa4inyiOtrKXUUyZwozSihrdH15rHz7op1f4W8uBq6QpV\
k9wfR07mXeiO2nGTlCNEBEFpUXUvsYhjezqI48lstpmW4GddEI5VwV2hHex5NnwF7gvXOk/luL9C\
IAzhDNEe04kIyDhiIMY0rsoRWXxiW/PH4VEMwruTEXswhvd6/nMDvE32h4MH0/ls/9FDTR2/6nMV\
i+DZd76ubCfjQtcvPoDeCODIO52dkfZ8TPpRaE9/R2JDF11EB1dqy7dwEqMuLn9DhFwo1wGf4AtK\
A3xFvAIAAA== $BINNAME
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

# .desktop file so it will appear in the menu.
putFile H4sIAMLJ9lIAAz3LPQ7CMAxA4d2nyAm4AMqASvlRJaZ0QgxWakLUxIkcSyW3BxbGJ33vfqS2aqlm\
ZJX+ANcr2UOtKXrUWBhumMm6F5lTZExmog5DyZlY7QVl2VDIVGxtK7KYjIyBBMY3efv8DevXX33h\
f+0qBxhQKRSJ1OzZTftZY4raAT4+IJanjgAAAA== $DESKTOPNAME
chmod a+r $DESKTOPNAME

echo -e "\nThe following files were installed:\n$ICONNAME\n$XICONNAME\n$DESKTOPNAME\n$BINNAME\n$UDEVNAME\n"

if which systemctl &> /dev/null
then
  systemctl restart udev &> /dev/null
elif which restart &> /dev/null
then
  restart udev &> /dev/null
else
  echo "Restart udev or computer for changes to take effect."
fi

echo -e "If you have The Final Key plugged in at the moment, you need to unplug-\nand insert it again before you can use it.\n\nThe Final Key installed for user: $1.\nRun: finalkey to begin using it.\n"

