# qtcsdr

This will be once a Qt-based GUI for *csdr*.

![qtcsdr](/screenshot.png?raw=true)

Requirements:

* Qt5
* <a href="http://sdr.osmocom.org/trac/wiki/rtl-sdr">rtl_sdr</a>
* <a href="https://github.com/simonyiszk/csdr">csdr</a>
* <a href="https://github.com/ha7ilm/pgroup">pgroup</a>
* ncat from the *nmap* package
* mplayer

# What you will need
* You will need an RTL-SDR receiver

# How to setup

    sudo apt-get install qt5-default qt5-qmake
    
	git clone https://github.com/ha7ilm/pgroup.git
    cd pgroup
    make && sudo make install
    cd ..
    
    git clone https://github.com/ha7ilm/qtcsdr.git
    cd qtcsdr
    mkdir build
    cd build
    qmake ..
    make -j4
    
    ./qtcsdr
    
