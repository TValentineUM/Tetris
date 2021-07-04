#!/usr/bin/env bash

echo "Compiling Server"
cd src/server/
make clean
make
cd ../../
echo "To run the server: ./src/server/server"
echo echo

echo "Compiling the client"
read -p "Would you like to compile the program as a shared library? [Y/n]" yn
echo
cd src/client/
case $yn in
    [Yy]* ) make clean;
            make;
            break;;
    [Nn]* ) make clean;
            make normal;
            break;;
        * ) echo "Please answer [Y/n]";;
esac
cd ../../
echo "To run the client: ./src/client/client"
