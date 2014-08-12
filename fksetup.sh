#!/bin/bash

# License: WTFPL ( http://www.wtfpl.net/ )

UDEVNAME=/etc/udev/rules.d/80-FinalKey.rules
BINNAME=/usr/bin/finalkey
BCKBINNAME=/usr/bin/finalkeybackup
ICONNAME=/usr/share/pixmaps/finalkey.png
XICONNAME=/usr/share/pixmaps/finalkey.xbm
DESKTOPNAME=/usr/share/applications/finalkey.desktop
SHAREDIR=/usr/share/finalkey
GUINAME=$SHAREDIR/GUI.jar

if !which minicom &> /dev/null
then
  UNMET="  minicom\n"
fi

if !which xterm &> /dev/null
then
  UNMET=$UNMET"  xterm\n"
fi

if !which setsid &> /dev/null
then
  UNMET=$UNMET"  setsid\n"
fi

if !which gunzip &> /dev/null
then
  UNMET=$UNMET"  gzip\n"
fi

if !which base64 &> /dev/null
then
  UNMET=$UNMET"  base64/coreutils\n"
fi

if !which lsof &> /dev/null
then
  UNMET=$UNMET"  lsof\n"
fi

if !which gcc &> /dev/null
then
  UNMET=$UNMET"  gcc (Debian/Ubuntu: build-essential, Archlinux: base-devel)"
fi

if !which java &> /dev/null
then
   echo "Java not found, GUI option won't be installed."
fi

if [ "$UNMET" != "" ]
then
  echo "There are unmet dependencies, please install:"
  echo -e "$UNMET"
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

#The base64 encoded data that putFile takes as first argument are created with: gzip -c THEFILENAME | base64
function putFile
{
  echo $1 | base64 -d | gunzip > $2
}

#Source code for the backup utility, how elegant!
putFile H4sICPMtf1MAA2ZpbmFsa2V5YmFja3VwLmMAxVprU9tI1v6Mf0XDVsDOGttAJpXAOFXEwA67DKSA\
LDMvIZQstWwtsuRVSzjeSfa373NOd+tmc0lqt14qAbvV5/S531rdlw3xUpwEroyU3BVXl0cfTkRTjNN0utvtzmazziz1p2EnkmlXtGjve8e9y6YijeNQ+HEi3EQ6aRCNhBN5IpEqjRP6NuRtqkMgl2Mp\
4iydZqnwg1CKQInAk1EauE4IRCLFcxm5yXyaSk9IOU3iSVvIL66cpnxGHEnC43hekAZxBKjhPJU42n0j3LF071Q2EfJeJnOxs83P9MGH9zIC9jgbjfmQGhH5mW0RpLSg0iSORuEcJ91DJB4RRwzx6ekY\
OzRoJByhHF+Kaei4si1U5o5F7GPVkx64IjY+XrwHbODedSdyEoMy10k8ESeEapglER0J9E4Ug7JEHAXE1t/kXAwlWJbAFEqWKxFOxzJH9H8/gmycyTTMGcLRTIJjxJ5z6Nw7QegM8YXE6MWzKIwdTzjp\
LiEyWnbnQ5mo1AnvZNLx7ro+kXIn590xkIGCrtgU+ynTkQYTSae58WQSpIa8QLW1fJxCtOBK3EU4j3gcO/cSbEEXwyS+kxEEBsZTEUpHpfgMnEq6WRKkJCXs0tpW0Kr0crYvSf5ZGoS0beLMydiyMCVt\
hLFSRJbnpA59Z5HFYRjPiMKZM1fM78rvcSZimEkC84PkIcZApYW1FmKu7SxpZzYOIGg3jlIniKBmPpJZnzmabcIlvc1sWmDSblFDFKRjosGNkySbpt0gunfCwBMQfI2OAtqQaUC14fhBMpk5eKxdyg+g\
RAZdqelsBvkSr9dMA0jYviFN0RaiQbrpAgqcvgHMIxnJxAlDtmGYpVJdq1+WL3E/11u9GCu5ypaqbZjAKWg/DFKSmz2lsZx/+MzEGUkyKa0GIMGJubExlinsZhYn2m5WKPQY+M14Ci4ofnAAilLJgpfk\
DoQ7i8LMvZuzvIwEm/s+duWS46CztdO2Llpd337V1lAUCEFJhLAFkQV+KSrmLLAKvcD3ZQKR50S3KqIb2NBqgqloDrVHChgBCzA3pxaJi6UVSR23WCAInDnutg6BWNHCA78xIievwug0y52ufzd07zpD\
6MRY22SuP9CakSkEpuDTShMxtBkhD1VirQy0Jo4RnkYjSIEAGIdLTp+bspiQOH0TUSJngpgKUxmSabhh5tkwCGcrwuB5PdeIplWIU6LJqQlJLEopncW5lFTbiIdNVRaJSsOyQdm9ywWpowGZ5ixQYzqE\
AgmRZs3qCRzGyOtYjBlrHMf+o8a/nLI2wyDCbiBKyZFRZ1pyEWReF+7d5mQefM8ZOao69cS3tY2chlmsaRCcm/BUY0FOXDRD6/5L7BB+zPhV5SFsvHweAlcR6Dlcx5GmD48Z0SzAFhBj9ZTKqCMuCEUQ\
3YFEZNmcMvr1C0kiRhYPQk+bxWVMKXEK09f8sXk5YmBXEy1RogmJNvKoFKCFaayCLyIMhlyvMKqR64rNTZV6ffftW7EZi7IsOnlyNuy6ebC4Ik8aS+d+Tr+TlPFPqCpQY3YoHV5RE4gQbiSiYDROSyGM\
cPw1mEzmYjBOIC0qCKO2rnrcOAs9kpBH4WMoISFw5CN7IzrSOWqCE+1nFEZJW5c5KVzfky6cyNZScHkKjSh+YEN3VHopJJ+5kTKppW0Nb+ZEVCIxqmAC00QVEWi+QBqYo5Q4zcJwM5H/zCi6QLEjiCEb\
2iDRbTT+5EnITIrb9xcHtxdnH88HhyvdrrgY4zhECH2u608cVD/ODPt1yJHiZ+ggiDvjd9UlKKu+RkGoupZFkKBXXfPdKA2rS8hAUe0ESHESxGrhWOSr8trPaq66MCVebICEDOnbwIo0ne8tWbuNQ2+v\
0WhkwPXmNuXyuYlKBoKzSy9RYkOz9msoo1bjj4YwPyVA0Rc94LJPEArgx03s39xsiQKiDhlEnCv7+qA//3lvYSfVqc18O3a+2RMB/i3HW8c/gTf1RZMI/GwOa4l10fvS21o8yv7Q7nfv+uKRLbDIJnC3\
eO9n8P7lzeCR3ZrLB3B+ayx+SmRKLQGw7zW+NRr3MQpB/+4kRoxuosNxkpdiqmZtqlqosWibtWHmC6E1RE9C+Hm/p0+cwihTv7k2iKMIpR1n0LhIhZ1P0VpL72TdNUGC4zUZN7C2t1rU7eHnj8ZK4Ddp\
7bp30+9v/GkDD1awKlDISedur7Gi2dC/K2cfIFrgoE/RB8RwReEgjbkhCImx1R8lQW2I9XUYG7gVfehCfP2qWe8DJKeOVsjAVr4JGSpZw7HLOCyKbbHIFEOJHBXJFZyuPMTrRTCKSMhBVBYtJaem1hjr\
D04JH2niM5jTe9AdKpnSUrvXVsG/ZOzz85ZxL3vAlaMjNTkINcjT9Id1+O5xHdoT/4KsXzkJlklmNpLpOQ6aNxet0X6cqJENHHXD/A5SSTUb1xVN9Z6jX4Y7r8BtPRvuoGYZz4X7vQK382y4mwrcqyWG\
aODqpqg1VjdEkvxeYZ7/H5I/+0HJn9bhRLc7cFJXz27oMSqg0zMuAJC2Ewr3SqGf65gDrOFeooPzaDaCEPMxmoaZHU7lH1OqOEbo4DsdY9srNgg3e63C/Rdo/NsPWsfND1rHp+QB83gSMKoA/vRAgFs0\
Kx0HGkVWam5p19dZ6Z9ZkOaOf3R8cvhS+J5OTFSOZMrmI5/jHhuXDgRuGCtJhsf2+c3s8b3SFl/v8b18Cwr3tKkRFwFoAsUxDU4yck3IeUlf7vXZ3e4B9xczqizbtdkCZIT6VFCkVWLsKD2Ycsc4OOJy\
PZSppBFgKhKYSjzR9oYGhLognjbCuHgqiXbBp5Ia1SYTwfLf6tHPzZ5d8+T9EfwO69uvbvbYLXkdMf56a/sNbzSyIHbEKmmZVGe/7RTysQb+UaHH2hXVXkDsDy6Pz04FKUVcX17+fnD49+PB4Q3sm1Qs\
1kytYbZt2h4ZnJgWq76TMQnsvLR9PXK3/DIl1wuoxwIoCnP66iNPdOrwOQ1AcaBHPT63SCjJnSxMFeHrQkBdO5ISzbOpnvC26siapVGOiHlX0Vgr7vJsc8dTCRYOtXiri6ioV9LtRqmdJZXSQj6/Y3xt\
bun7emolvVXCVuTdkodoey1r0ji6VR5yvzudN61BtMVahfU1XRDkdcdSELLx650bUT5PmEqfvET4typl0uz/bvdK2k5KUJMjVEajK25EoTgzgNGzWkIhNRTa3xlPjaG0qYynWvleoFxdUJqIar9xe6x5\
DwlLieZ1ognB3OQQIfI6HqdIVD/of++olRvFsacn6nQUexoV3k40p66TXDWeSpq58ihI25MTUjqbd3KMV9ykwvlhp0RYqGKf7HSE6M9NO80mZCLzEUeQbvDctq1lQqOAJIssQnZVd+JdpEnurfk6jg4/\
OOn4+s3W2234dre7H05ildpoouNHiBD0qvf2NZpWYlbT4ETCcel+gwcgE+cL7BCKQHE4wh8V675bjbnx/kemaLIeSUuV+UPSJhoItCRwS5aW+enHkxOT4jnJ2xhymCRxsisGfAKFNvhWHKLF3nihkD3W\
2hYhpcOKnReVYk6JMliNqGDbLPfNSHzVkn+hxPY77esROva1MpUWp8km08HEg8NNSdklhMlasZMTB+8rMYZFH6WpanIdurPd1ltaFENJCK1alfApMiKgwFLM5Xk2LDL44HCeD9md6TSkex0qNj5F+sgP\
dH0hdVoTC9tUrc7Iq4ypyXFMW7nu2GqZDmOltsVIPG9ILAPHKY9DvPtAsVOYiVsQ0V1OqIsmUsMGbMu9o6mcmVtnyl7cUH0/SpxJqZXIs39JUIbVTNdSVXkhcJSh9W9Taj4ZCYpYIFQo5bSwsG9ldHlh\
N3YivjpAO2Wkb+JPlaayyEUtpubIPjyCoQxrgykXFtSFcaaAgSTky5C2TFI7bTSNOF0hnGVpf4cr78eFsWqFockzoJubWg4warMifhY9W6mLCiMLnmxZAnkvVM2VNWjNn7Wwyxr4pvNH49H+VAulyED5\
rAl/23xDcIlPpo/t0sQ+TQKJEONmCV97MACJHCdQc6tQrNEcRCKAkonGvk8zPC0I0Uxd+LeTpknz4vLg+PSWAsbpGURJeNisNreWi6iQDTDwsYRGVcx+uUwMEwhIBLUnHvwBfzpn0g2mrVMsXzxyTSwv\
hKnj3oZ+6IzEel/8+3Dwy9kCaiCkdRJC28SXYYCaKYuQfKIRXL4iGrVUNJeDi/2jk48Xv3yflNTzpaQLdz/M1BiFuoeySk8u6L9FfD3QCr8ReaFny65djVpHbj0jKWyMBiZeEPGOIhppCFsbcbTHZp0d\
8kTEqNaM+dOcjB8bYyaY3JjxxQxbnhLk6dkVxGhNoiTKanVeSalsBstEWSsdTe2oGytYm85+ebQ4uz0/uDr/ir+nZwPU1UJTrPkRzfU0RZXRs8Kjaa/RgrYjihDiF2SkkNyK7YaYFblHCTOlIkSLQWmR\
s0qIEU19wIuA+4ACa6tTjj5twfNum8a5k+RDKTu3hTj87fjy9mj/+OTj+WEhFfIren8gxslE3NRBvkKLlijNhxlsk4PS2NtAQCjvncwT53TVwftcH5KK1ZRuZoy8mvztNm29f/u61+MjeVfw+K78CJ7H\
ac/8QE3QRZrZgAVIeLjLHk4/8HLx7w/754en73NH73bFr1SQv4m2HoIYXFyefXi/9/Dj4/87XPb0K54OLt4QrThlOez55cXg8mKvIAaqoXaWX25I4rBA617//dfj0xu9EbCiR7Uuve+wpdd63FkoupZR\
+lUTqsp5Wkn2AP1XAluXH6NOl4qvAenqr3La5fGvhzfV0/j1ip947fPnzzV0vc5P9AZJHHlKo6aciUjEKXupbM4P9w9Qmw5Ozgb7J3sWDycfNJD8dF0Eo4i8102TED1KJJVRPWstcWbWsMzdEdtLbh9H\
FBHZLtr6bQAuDqlThWMEw4yaVm0qrg6e2gEvB8ccr7WXGCdVVSctYlHhrYvOWvdJg0S75HJP1I643A91cArsMLE61u2iMqI+vFG5JNoz3TCF6sm0qbvXrRsq5nVgXCsXg3/oU3QXgBK8D0PiIMhg2ww2\
zJsA4F1FZngijxECPblAlCrZZVsPEOnejsVhjsiz3FMSqdeU+dFn3KHuiitzr0o9M3eqnVLoL10f2EseTld2KFse4FrE5/qKk3DaFwGW3zOs/bbW3hLLHgRgNH+y7Frh/OB3vq65chK6y9iF9ZKakU+i\
2OSrvPvBz35p5GLffVhylb5qByZWb2I1v0HIx9BirXJHFOkbdoeGdvkdtX7VKhcoIRVVA3iG2mqXKiXUJXkS10kW9c3t3bIhehYVc/OFwfq6mayvr5uNuvOsXMJcbdC9VWVsTgNvgQfUY0AddIm4mUhX\
BvfIRnh+58wJDeMyEXva1tOBme1LrLV1aJMx0sq5R3z5k2MpyqojJwiz3KwegD6vQvMUur+zvVdDlnA02EX06b5+1ev1xFqbtrbMvnq5qBdZflp2YqtNDTyigH1atuR10+HnD/WFdA3B1hPw5Ao0ggoi\
l2dCSAoDzjJmpiRgwvQunKffWJhJHncl9CpDYKZFwWQaznlWYF+t0rHWjkupFad0RG9MOqGbhfp1LurauUN9SMqHD+nITFETSUNXxa9jFPoSbLO95bpjExs8Ey9JQlL6APKjmGrXfN62fKix5OiFM3Sb\
SpeKWUSSHEWoVT398ty9E2Ywl96XF7/Zgh00GzPMxxIrxewDdAVRoMb0AgndEehIvnjZU554WHINqUUkLgWOIDJh4+LjYHB4cVGtzpfmMj1GR2Lq98s1s85jNGW2iazIY7NKHhOrtOu7U5kJi9+XypaG\
RBvyotJbHfnbB5XMJGwYf17GkeWMk6ftZdF/8R0BHfttHcmx3xaTNvpXRVa9X2KbXNbR17JqtUy1mEsU59IZWqOxFQ5N1WR6vbN988CFq70UqyUPHaaqkUi8q093tLFR+1Iel9lrWv2+A9aRYqpXto9e\
EBqwrRrY++eBbdfADp8As3A7Nbi/PO+4VzWw4+eB/VQDO30e2Osa2E0BxmpguNJNqdC18Er1fQl+rRK9CIq1KLellf92SMx9GoGj7tL2DtcG4mXmtFUxJ+asSN3WJ5gKSuCvdQIXAR3uGwcvkiwHL3pi\
2hj7naU7/HlnOx+8a38Z3tB1GnOjB95DIkDv7vd3dsrVkn0TrGUUg06H31rTqKgGaD2Q0exQFJlM3yT5KG/0u96GsaX54sH89pjIOUnR1Xr+kpBmnt5fg2Gx/CxXK4UxreS2tDStzUqVueR3HmtG9aSa\
t5dFDfGDYUMsiRtPBYAH4sZTHvlQ3Dj4sbhR8uSyqy6T6fL3v/7XLluo0vwtpauyZSD30/DRK6WoRbTVwuWpAuBb4z852tonAjYAAA== "/tmp/fkbck.c"

if ! gcc --std=c99 -o "$BCKBINNAME" "/tmp/fkbck.c"
then
  echo
  echo "Could not build the backup utility."
  echo "When the problem is resolved, re-run $0 to enable backup."
  echo "The installation will continue, but the backup feature will not work."
else
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

# .desktop file so it will appear in the menu.
putFile H4sIAMLJ9lIAAz3LPQ7CMAxA4d2nyAm4AMqASvlRJaZ0QgxWakLUxIkcSyW3BxbGJ33vfqS2aqlm\
ZJX+ANcr2UOtKXrUWBhumMm6F5lTZExmog5DyZlY7QVl2VDIVGxtK7KYjIyBBMY3efv8DevXX33h\
f+0qBxhQKRSJ1OzZTftZY4raAT4+IJanjgAAAA== $DESKTOPNAME
chmod a+r $DESKTOPNAME

#The udev file
echo SUBSYSTEMS==\"usb\", KERNEL==\"ttyACM*\", ATTRS{idVendor}==\"2341\", ATTRS{idProduct}==\"8036\",SYMLINK+=\"FinalKey\",GROUP=\"$1\" OWNER=\"$1\" > $UDEVNAME
echo SUBSYSTEMS==\"usb\", KERNEL==\"ttyACM*\", ATTRS{idVendor}==\"1d50\", ATTRS{idProduct}==\"60a6\",SYMLINK+=\"FinalKey\",GROUP=\"$1\" OWNER=\"$1\" >> $UDEVNAME


if which java &> /dev/null
then
  if uname -a | grep -i x86_64 &> /dev/null
  then
    echo "Downloading gui..."
    mkdir -p $SHAREDIR
    wget http://cyberstalker.dk/finalkey/gui/FinalKey_0.1-Linux_amd64.jar -O $GUINAME
  fi
fi

echo -e "\nThe following files were installed:\n$ICONNAME\n$XICONNAME\n$DESKTOPNAME\n$BINNAME\n$BCKBINNAME\n$UDEVNAME"

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

echo -e "If you have The Final Key plugged in at the moment, you need to unplug-\nand insert it again before you can use it.\n\nThe Final Key installed for user: $1.\nRun: finalkey to begin using it.\n"

