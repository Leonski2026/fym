#!/usr/bin/bash

# Beende das Skript sofort bei einem Fehler
set -e

# Kompiliere das Programm
g++ -O3 -std=c++20 "$HOME/fym/main.cpp" -o "$HOME/fym/fym"

dicpath="/usr/local/bin/fym" 

# Überprüfung und Löschen der alten Datei
if [ -f "$dicpath" ]; then
  sudo rm -rf "$dicpath"
fi

# Verschieben in den System-Ordner
sudo mv ~/fym/fym /usr/local/bin/fym

ls -l /usr/local/bin/fym
sudo pkill -f '/usr/local/bin/fym' || true

echo "ready to use!"

