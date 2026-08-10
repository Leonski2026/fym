#!/usr/bin/env fish


g++ -O3 -std=c++20 "$HOME/fym/main.cpp" -o "$HOME/fym/fym"; or exit 1

set dicpath "/usr/local/bin/fym" 


if test -f "$dicpath"
    sudo rm -rf "$dicpath"
end


sudo mv ~/fym/fym /usr/local/bin/fym

ls -l /usr/local/bin/fym
sudo pkill -f '/usr/local/bin/fym'; or true

echo "ready to use!"

