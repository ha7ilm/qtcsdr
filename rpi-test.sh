#!/bin/bash
read -r -p "Do you have your RTL-SDR and an USB audio card connected? [y/n] " response
if [[ $response =~ ^([yY][eE][sS]|[yY])$ ]]
then
	echo "Installing deps..."
else
	echo "Please connect these, then retry."
	exit 0
fi

(rtl_sdr - | xxd | head) 2>&1 | tee /tmp/qtcsdr_test_output
cat /tmp/qtcsdr_test_output | grep "0000080:" > /dev/null
if [ $? -ne 0 ]; then
	echo
	echo "There is some kind of problem with your RTL-SDR. Aborting."
	cat /tmp/qtcsdr_test_output | grep "blacklist the kernel module" > /dev/null
	if [ $? -eq 0 ]; then
		echo "It seems that the dvb_usb_rtl28xxu kernel module is active, which doesn't let your RTL-SDR work."
		echo "You should blacklist this kernel module (rpi-install.sh can do it for you), or a temporary fix is to run:"
		echo 
		echo "    sudo rmmod dvb_usb_rtl28xxu"
		echo
		echo "(You will have to run it every time you plug in the USB dongle, unless that kernel module is blacklisted.)"
		echo
	fi
	exit 1
fi

echo
printf "\033[0;32mRTL-SDR is okay!\033[0m\n" 
read -r -p "Press any key to continue." response

echo
echo "Now I will list all the ALSA devices on your system."
echo "You will have to choose one USB audio device ID." 
echo
echo "Entries will look something like this:"
echo 
printf "hw:CARD=Device,DEV=0\033[1;33m       <==== This is the device ID of the entry; you will need this.\033[0m\n"
echo "    USB PnP Sound Device, USB Audio"
echo "    Direct hardware device without any conversions"
echo 
read -r -p "Press any key to list the devices." response

#aplay -L | egrep --color "USB|$"
aplay -L
echo
read -r -p "Please enter the chosen device ID: " alsadevice

echo
echo "Now we will try to record 15 seconds of audio from the microphone input."
read -r -p "Press any key to continue." response
arecord -f S16_LE -d15 -r48000 -c1 -D $alsadevice | csdr mono2stereo_i16  > /tmp/test_audio.raw

echo
echo "Now we will play back the recorded audio on the headphones output."
read -r -p "Press any key to continue." response
cat /tmp/test_audio.raw | aplay -f S16_LE -r48000 -c2 -D $alsadevice

read -r -p "Was the playback okay? [y/n] " response
if [[ $response =~ ^([yY][eE][sS]|[yY])$ ]]
then
	echo
	echo "Great! Now you can start qtcsdr by running:"
	echo
	echo "    qtcsdr --rpitx --alsa $alsadevice"
	echo 
else
	echo "Before running qtcsdr, you should first solve the problem with your USB audio device. Aborting."
	exit 1
fi

