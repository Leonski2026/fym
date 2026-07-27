#!/bin/bash

# Exit immediately if any command fails
set -e
g++ -std=c++20 -O3 fym/main.cpp -o fym
echo -e "\033[36m=== Setting Up Pre-compiled fym Compiler ===\033[0m"

# 1. Verify that the fym binary exists in the current directory
if [ ! -f "fym" ]; then
    echo -e "\033[31m[Error] Binary file named 'fym' not found in this folder!\033[0m"
    echo "Please make sure your pre-compiled 'fym' file is right next to this script."
    exit 1
fi

# 2. Ensure the destination directory exists (Creates it if missing)
echo -e "\033[33m[1/4] Ensuring /usr/local/bin/ directory exists...\033[0m"
sudo mkdir -p /usr/local/bin/

# 3. Move the binary to the system binary directory for global access
echo -e "\033[33m[2/4] Moving 'fym' to /usr/local/bin/ (Makes the command available everywhere)...\033[0m"
sudo mv fym /usr/local/bin/

# 4. Grant proper execution permissions to the system binary
echo -e "\033[33m[3/4] Setting execution permissions...\033[0m"
sudo chmod +x /usr/local/bin/fym

echo -e "\033[36m=== [4/4] Setting up logos for fym and fymfiles ===\033[0m"

# Find the JPEG logo file (supports .jpeg or .jpg)
LOGO_FILE=""
if [ -f "fym-logo.jpeg" ]; then
    LOGO_FILE="fym-logo.jpeg"
elif [ -f "fym-logo.jpg" ]; then
    LOGO_FILE="fym-logo.jpg"
fi

if [ -z "$LOGO_FILE" ]; then
    echo -e "\033[31m[Warning] No 'fym-logo.jpeg' or 'fym-logo.jpg' found. Skipping logo installation.\033[0m"
else
    echo "Found logo file: $LOGO_FILE"
    
    # --- APP ICON & DESKTOP ENTRY SETUP ---
    sudo mkdir -p /usr/local/share/icons/hicolor/scalable/apps/
    sudo mkdir -p /usr/local/share/applications/
    sudo cp "$LOGO_FILE" /usr/local/share/icons/hicolor/scalable/apps/fym.jpeg

    echo "Generating desktop entry..."
    sudo bash -c 'cat > /usr/local/share/applications/fym.desktop << EOF
[Desktop Entry]
Type=Application
Name=fym Build System
Comment=Your Custom Build System
Exec=/usr/local/bin/fym
Icon=fym
Terminal=true
Categories=Development;Building;
EOF'

    # --- FILE ICON SETUP FOR 'fymfile' (Like .c files) ---
    echo "Registering 'fymfile' as an official source code file type..."
    sudo mkdir -p /usr/share/mime/packages/
    sudo bash -c 'cat > /usr/share/mime/packages/fymfile.xml << EOF
<?xml version="1.0" encoding="utf-8"?>
<mime-info xmlns="http://freedesktop.org">
  <mime-type type="text/x-fymfile">
    <comment>fym Source Code File</comment>
    <glob pattern="fymfile"/>
    <glob pattern="*.fymfile"/>
  </mime-type>
</mime-info>
EOF'

    # Update system's file-type database and copy file icon
    sudo update-mime-database /usr/share/mime
    sudo mkdir -p /usr/share/icons/hicolor/scalable/mimetypes/
    sudo cp "$LOGO_FILE" /usr/share/icons/hicolor/scalable/mimetypes/text-x-fymfile.jpeg

    echo "Updating system caches..."
    sudo gtk-update-icon-cache -f /usr/local/share/icons/hicolor || true
    sudo gtk-update-icon-cache -f /usr/share/icons/hicolor || true
    sudo update-desktop-database /usr/local/share/applications || true
fi

# 5. Success verification
echo -e "\033[32m=== [SUCCESS] fym is now globally set up! ===\033[0m"
echo "You can close this window now."
echo "Go to any project folder containing a 'fymfile' and simply run: fym"
echo -e "\033[33mTip: Restart your file manager or log out/in to see the new icons on your fymfiles.\033[0m"
