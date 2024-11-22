ninja -C build/default

status=$?
[ $status -ne 0 ] && read -p "ninja error $status Press enter to continue..."

cd build/bin
SDLPlayground.exe