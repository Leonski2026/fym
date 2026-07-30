#!/bin/bash

set -e

g++ -O3 -std=c++20 "$HOME/fym/main.cpp" -o "$HOME/fym/fym"

ordnerpfad="/usr/local/bin/fym" 

if [[ -d "$ordnerpfad" ]]; then
  sudo rm -rf "$ordnerpfad"
fi

sudo mv ~/fym/fym /usr/local/bin/fym

echo "ready to use!"
