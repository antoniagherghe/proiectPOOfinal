#!/bin/bash
# Builds the game if needed and starts it from the script's own folder.
cd "$(dirname "$0")"
[ -x ./farm ] || make
./farm
echo
read -p "Press enter to close..."
