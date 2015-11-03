# qtcsdr

**qtcsdr** makes a ham transceiver out of your Raspberry Pi + RTL-SDR!

![qtcsdr](/screenshot.png?raw=true)

Click on the video to see it in action:

<a href="https://www.youtube.com/watch?v=3Y8qJ6nmNPY" target="_blank"><img src="http://ha5kfu.sch.bme.hu/up/levlista/qtcsdr-screenshot-youtube.png" 
alt="qtcsdr"/></a>

Big thanks to *Evariste, F5OEO* for <a href="https://github.com/F5OEO/rpitx/">rpitx</a> that makes this possible.

## What you will need to make it work
* You will need an RTL-SDR dongle which will be the SDR receiver.
* You will need a Raspberry Pi 2, which will be the SDR transmitter.<br />With the help of the fantastic <a href="https://github.com/F5OEO/rpitx">rpitx project</a>, we can transmit AM/NFM/WFM/LSB/USB on the "GPIO 18" (number 12) pin of the Pi (<a href="http://301o583r8shhildde3s0vcnh.wpengine.netdna-cdn.com/wp-content/uploads/2015/04/Raspberry-Pi-GPIO-compressed.jpg">see pinout over here</a>).
* You will need **a proper low-pass filter for the transmitter output**, see the explanation below.<br />Soon you will be able to purchase the QRPi filter + amplifier board that was featured at TAPR DCC 2015, <a href="http://rfsparkling.com/qrpi">see details here.</a><br />(You can also build your own filter circuit based on HOWTOs found on the web.)
* To transmit, you will need an USB audio card, because the Raspberry Pi doesn't have a microphone input. 
* You may or may not need an external powered USB hub to supply enough current to the audio card and the RTL-SDR.
* As the OS for the Raspberry Pi 2, you will need at least **Raspbian Jessie (09/2015)** as it has the Qt5 packages in its repos (or you would have to manually download Qt5 binaries or build Qt5 from source).

## Warning

To transmit on ham radio bands, you will need a ham radio license.

Even if you do have a ham radio license:

* Do not transmit on an antenna without using a low-pass filter!
* Do not transmit on an antenna without using a low-pass filter!
* Do not transmit on an antenna without using a low-pass filter!
 
The Raspberry Pi GPIO 18 is a digital output, which generates square wave signals, so that if you transmit on 28 MHz, then you will also transmit on:

* 28 × 3 = 84 MHz
* 28 × 5 = 140 MHz
* 28 × 7 = 196 MHz<br/>...

...and actually you may distrub some important radio communication services. Do not do this!

If you need an SDR transmitter that doesn't generate harmonics like this, then you should buy a proper one, for example the <a href="https://greatscottgadgets.com/hackrf/">HackRF</a>.

Also NEVER EVER transmit or do any tests with your transmitter in the airband (where flights communicate)! <br />
The VHF airband is between 108 MHz and 137MHz: never transmit there. You have been warned!

## How to set it up

The easy way is to use the scripts that come with *qtcsdr*:

    git clone https://github.com/ha7ilm/qtcsdr
    cd qtcsdr
    ./rpi-install.sh
    ./rpi-test.sh

If *rpi-test.sh* succeeds, it will display the command line for running *qtcsdr*.

As already mentioned, you will need at least Raspbian Jessie (09/2015) if you want to install *qtcsdr* with *rpi-install.sh*. 

---

**Anyway, if something failed with the scripts, the setup procedure is detailed below.**

A short list of requirements:

* Qt5
* <a href="http://sdr.osmocom.org/trac/wiki/rtl-sdr">rtl_sdr</a>
* <a href="https://github.com/simonyiszk/csdr">csdr</a> (dev branch)
* <a href="https://github.com/ha7ilm/pgroup">pgroup</a>
* <a href="https://github.com/ha7ilm/rpitx">rpitx</a> - if you want to transmit<br />(it was written by F5OEO, and I modified it to make it work with **qtcsdr**)
* **ncat** from the **nmap** package (this will distribute the I/Q signal between processes).

Guide:

    sudo apt-get install nmap qt5-default qt5-qmake git libfftw3-dev

    #Install the SDR transmitter software rpitx by F5OEO
    git clone https://github.com/ha7ilm/rpitx.git
    cd rpitx
    bash install.sh
    cd ..
    
    #Install pgroup
    git clone https://github.com/ha7ilm/pgroup.git
    cd pgroup
    make && sudo make install
    cd ..
    
    #Install csdr, the DSP tool
    git clone https://github.com/simonyiszk/csdr.git
    cd csdr
    git fetch
    git checkout dev
    make && sudo make install
    cd ..
    
    #Install qtcsdr, the GUI
    git clone https://github.com/ha7ilm/qtcsdr.git
    cd qtcsdr
    mkdir build
    cd build
    qmake ..
    make -j4
    cd ../..
    
    #Install RTL-SDR driver and tools  --  skip if already done
    sudo apt-get install cmake libusb-1.0-0-dev 
    git clone https://github.com/keenerd/rtl-sdr
    cd rtl-sdr/ && mkdir build && cd build
    cmake ../ -DINSTALL_UDEV_RULES=ON
    make && sudo make install && sudo ldconfig
    cd ../..

    #Disable the DVB-T driver, which would prevent the rtl_sdr tool from accessing the stick
    #(if you want to use it for DVB-T reception later, you should undo this change):
    sudo bash -c 'echo -e "\n# for RTL-SDR:\nblacklist dvb_usb_rtl28xxu\n" >> /etc/modprobe.d/blacklist.conf'
    sudo rmmod dvb_usb_rtl28xxu # disable that kernel module for the current session
    
    #Go back to qtcsdr directory
    cd qtcsdr/build

* Now you should plug the USB audio card and the RTL-SDR into the USB hub connected to the Pi. 
* Also plug microphone and headphones in the audio card.

This is the syntax of the **qtcsdr** command:

    ./qtcsdr --rpitx --alsa <alsa_device_id>

You will need to get the ALSA device ID of your USB audio card.

    aplay -L

You will see multiple entries. For me this is the correct one for the USB device:

    hw:CARD=Device,DEV=0
        USB PnP Sound Device, USB Audio
        Direct hardware device without any conversions

...now we know that our ALSA device is: **hw:CARD=Device,DEV=0**

So we can run:

    ./qtcsdr --rpitx --alsa hw:CARD=Device,DEV=0

## Troubleshooting

### RTL-SDR

To see whether RTL-SDR is working, you can run the following:

    rtl_sdr - | xxd | head
    
Now you should see the raw I/Q samples in hexadecimal, something like this:

    $ rtl_sdr - | xxd | head
    Found 1 device(s):
    0:  Realtek, RTL2838UHIDIR, SN: 00000001
    
    Using device 0: Generic RTL2832U OEM
    Found Rafael Micro R820T tuner
    Sampling at 2048000 S/s.
    LO: 106000 kHz, MixDiv: 32, PLLDiv: 58, VCO 3392000 kHz, SDM: 58254 
    Tuned to 100000000 Hz.
    Tuner gain set to automatic.
    Reading samples in async mode...
    0000000: 778c 7a74 8c82 7583 8376 838c 7977 897e  w.zt..u..v..yw.~
    0000010: 7d8c 8173 828a 7578 8e7c 7f8d 8175 8d86  }..s..ux.|...u..
    0000020: 6f82 8873 7e8b 7c78 8582 7086 857e 8390  o..s~.|x..p..~..
    0000030: 7a76 8e7f 7485 7f74 8393 7478 8d7c 7989  zv..t..t..tx.|y.
    0000040: 8474 8887 797a 877d 7c87 8275 8b85 757f  .t..yz.}|..u..u.
    0000050: 877c 7f8b 7f76 8b88 737f 8677 7e88 7c72  .|...v..s..w~.|r
    0000060: 8880 747f 8476 8489 7e74 8f80 7986 887a  ..t..v..~t..y..z
    0000070: 818b 7777 917d 7684 7b77 818d 757d 8b83  ..ww.}v.{w..u}..
    0000080: 7488 8173 868e 7679 8d79 7385 7f70 8885  t..s..vy.ys..p..
    0000090: 727a 8a7d 7d8c 7d75 8685 797e 8a77 7a8c  rz.}}.}u..y~.wz.
    Signal caught, exiting!
    Signal caught, exiting!
    Short write, samples lost, exiting!
    
    User cancel, exiting...

This is correct.

But if you just see an error message, then something is wrong.

### USB Audio

I'm using a cheap CM108-based USB stick, which is not too good quality, but works. 
If you know the ALSA device ID, you can check that it works:

You can record something from the microphone input...

    arecord -f S16_LE -r48000 -c1 -D <alsa_device_id> | csdr mono2stereo_i16  > ~/test_audio.raw

...and then play it back.

     cat ~/test_audio.raw | aplay -f S16_LE -r48000 -c2 -D <alsa_device_id>

Don't forget to remove the test file from the SD card.

    rm ~/test_audio.raw

# Notes

* The transmitted signal is usually more clear if listened to with an external communications receiver rather than the RTL-SDR.
* Sometimes the RTL-SDR dongle is quite off frequency from the transmission. Switching off the receiver, changing the center frequency a bit and starting the receiver again may help.
