#!/bin/bash

set -e

g++ -O3 -std=c++20 "$HOME/fym/main.cpp" -o "$HOME/fym/fym"

dicpath="/usr/local/bin/fym" 

if [[ -f "$dicpath" ]]; then
  sudo rm -rf "$dicpath"
fi

sudo mv ~/fym/fym /usr/local/bin/fym

ls -l /usr/local/bin/fym
sudo pkill -f '/usr/local/bin/fym' || true


echo "ready to use!"
