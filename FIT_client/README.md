## Compiler(On ubuntu shell)

put your program in example, and add your project path in example.pro, compile with QT 4.8.6. (bad way actually)

    cd ~/qt-everywhere-opensource-src-4.8.6
    
    ./configure -embedded arm -xplatform qws/linux-arm-pxa270-g++ -prefix build-pxa270 -release -no-opengl -opensource -verbose -no-glib -no-xrender -no-xrandr -no-xfixes -no-xcursor -no-xinerama -no-xsync -no-xvideo -no-xshape -little-endian -confirm-license -no-rpath
    
    make
   
    make install

## execution(On pxa270 shell)

export your lib

    export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH
    
export your font

    export QT_QWS_FONTDIR=$(pwd)/lib/fonts
    
run
  
    ./FIT_client -qws
    
**if any compliling problem can see more detial in reference**
