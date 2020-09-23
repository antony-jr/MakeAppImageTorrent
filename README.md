# Make AppImage Torrent

Make AppImage Torrent is a very simple and powerful cli tool to make torrent file which is to be 
uploaded along .zsync file to enable decentralized update from QAppImageUpdate library.

Having a decentralized update mechanism can really help if you are hosting the update binaries 
from a low bandwidth server. But in Github this is not the issue but still decentralized update
can benefit a local network which updates the same AppImages because the data blocks will be 
shared between the local computers which will be hundred times faster than downloading it from
the internet.


# How to use

Download the required binaries(AppImage) from the releases and use it as below.

```
 $ ./mkappimagetorrent.AppImage YourNewAppImageWhichWillBeUploaded.AppImage
```

Now you should have a file named as the same as your given appimage but with a 
*.torrent* suffix.

You should upload this .torrent file along with your .zsync file to your desired
release type as mentioned in the AppImage documentation.


# License

The BSD 3-clause "New" or "Revised" License.

Copyright (C) 2020, Antony jr.   
All Rights Reserved.
