#!/bin/bash
##############################################################
#
# Install Gribouillot for Ubuntu in user space
#
# Tested on Ubuntu 17.04, Gnome Unity
#
# Should work with any freedesktop based environment
#
##############################################################

# QtLibs/		contains the required libQt5 libraries
# plugins/ 		contains other specific Qt5 libraries
# gribouillot_2.0_x64:	the dynamic executable
# install_gribouillot:	this install script
# gribouillot.desktop:	the freedesktop application file
# gribouillot.png:	the application icon

# Create necessary local directories
mkdir -p "$HOME/bin"
mkdir -p "$HOME/.local/share/applications"
mkdir -p "$HOME/.local/share/mime/applications"
mkdir -p "$HOME/.local/share/icons/hicolor/64x64"

cp gribouillot_2.0_x64 "$HOME/bin"
cp gribouillot.png "$HOME/.local/share/icons/hicolor/64x64"
cp grib.xml "$HOME/.local/share/mime/applications"
cp -r QtLibs "$HOME/.local/share/QtLibs"
cp -r plugins "$HOME/.local/share/QtLibs"

#Finalize .desktop entry with user's $HOME path
sed -i -e "s|HOME|${HOME}|g" gribouillot.desktop
cp gribouillot.desktop "$HOME/.local/share/applications"

#Need to source .profile to activate new bin path
update-mime-database $HOME/.local/share/mime
update-desktop-database $HOME/.local/share/applications
gtk-update-icon-cache ~/.local/share/icons/hicolor/ -t
source .profile
