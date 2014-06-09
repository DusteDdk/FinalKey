#!/bin/bash
#
# Copyright (c) 2014 Joshua Wells
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

BCKBINNAME=/usr/bin/finalkeybackup
BINNAME=/usr/bin/finalkey
DESKTOPNAME=/usr/share/applications/finalkey.desktop
ICONNAME=/usr/share/pixmaps/finalkey.png
UDEVNAME=/etc/udev/rules.d/80-FinalKey.rules
XICONNAME=/usr/share/pixmaps/finalkey.xbm

ROOT_UID=0
#Make sure user is root
if [ "$UID" -ne "$ROOT_UID" ]
  then
    echo "This program needs to be run as root."
    exit 1;
fi

#Ask if user really wants to delete The Final Key
echo -e "\nAre you sure you want to remove The Final Key from your computer?"
echo -n "If so, type \"yes\" to continue. Type anything else to quit:"
read CONFIRM

#See if "yes" was typed or not.
if [ "$CONFIRM" != "yes" ]
  then
    echo "Canceled."
    exit 1;
fi

    echo -e "\nRemoving Files..."

    #Delete all files installed by fksetup.sh
    for DELETE in $BCKBINNAME $BINNAME $DESKTOPNAME $ICONNAME $UDEVNAME $XICONNAME
    do
       rm $DELETE
       RMSTATE=$?

       #Print files successfully deleted.
       if [ "$RMSTATE" -eq 0 ]
         then
         echo "$DELETE"
       fi

       #Print files unsuccessfully deleted.
       if [ "$RMSTATE" -ne 0 ]
         then
         ERROR=1
         echo -e "\nRemove failed: $DELETE\n"
       fi
    done

#Tell user if files were successfully removed or not
if [ "$ERROR" = 1 ]
  then
    echo -e "\nOne or more files could not be deleted."
  else
    echo -e "\nThe Final Key was successfully uninstalled."
fi

exit 0;