#!/bin/bash

PWD="$(pwd)"
cd ..
cd ..
BASE_DIR="$(pwd)"
ARDUPI_DIR="$BASE_DIR/arduPi"
ARDUPIAPI_DIR="$BASE_DIR/arduPi-api"
EXAMPLES_DIR="$BASE_DIR/examples/4G"
LIBRARY_DIR="$BASE_DIR/libraries/arduPi4G"

#compile arduPi
cd "$ARDUPI_DIR"
file="./arduPi.o"
if [ -e $file ]; then
  if [ "$1" == "-clean" ]; then
    echo "arduPi.o -> purged"
    rm ./arduPi.o
  else
    echo "arduPi already compiled"
  fi
else 
  if [ "$1" != "-clean" ]; then
    echo "Compiling arduPi..."
    g++ -c arduPi.cpp -o arduPi.o
  fi
fi 

#compile arduPi-api
cd "$ARDUPIAPI_DIR"
file="./arduPiUART.o"
if [ -e $file ]; then
  if [ "$1" == "-clean" ]; then
    echo "arduPiUART.o -> purged"
    rm ./arduPiUART.o
  else
	echo "arduPiUART already compiled"
  fi
else 
  if [ "$1" != "-clean" ]; then
	echo "Compiling arduPiUART..."
    g++ -c arduPiUART.cpp -o arduPiUART.o
  fi
fi 

file="./arduPiUtils.o"
if [ -e $file ]; then
  if [ "$1" == "-clean" ]; then
    echo "arduPiUtils.o -> purged"
    rm ./arduPiUtils.o
  else
	echo "arduPiUtils already compiled"
  fi
else 
  if [ "$1" != "-clean" ]; then
	echo "Compiling arduPiUtils..."
    g++ -c arduPiUtils.cpp -o arduPiUtils.o
  fi
fi 

file="./arduPiMultiprotocol.o"
if [ -e $file ]; then
  if [ "$1" == "-clean" ]; then
    echo "arduPiMultiprotocol.o -> purged"
    rm ./arduPiMultiprotocol.o
  else
	echo "arduPiMultiprotocol already compiled"
  fi
else 
  if [ "$1" != "-clean" ]; then
	echo "Compiling arduPiMultiprotocol..."
    g++ -c arduPiMultiprotocol.cpp -o arduPiMultiprotocol.o
  fi
fi 

#compile library
cd "$LIBRARY_DIR"
file="./arduPi4G.o"
if [ -e $file ]; then
  if [ "$1" == "-clean" ]; then
    echo "arduPi4G.o -> purged"
    rm ./arduPi4G.o
  else
	echo "arduPi4G already compiled"
  fi
else 
  if [ "$1" != "-clean" ]; then
	echo "Compiling arduPi4G..."
    g++ -c arduPi4G.cpp \
      -I"$ARDUPIAPI_DIR" \
      -I"$ARDUPI_DIR" \
      -o arduPi4G.o 
  fi
fi 

sleep 1


#compile example
cd "$EXAMPLES_DIR"
file="./$1"

if [ "$1" != "-clean" ]; then
  if [ -e $file ]; then
    if [ "$1" != "" ]; then
    echo "Compiling Example..."
       
    g++ -lrt -lpthread -lstdc++ "$1" \
      "$LIBRARY_DIR/arduPi4G.o" \
      "$ARDUPIAPI_DIR/arduPiUART.o" \
      "$ARDUPIAPI_DIR/arduPiUtils.o" \
      "$ARDUPIAPI_DIR/arduPiMultiprotocol.o" \
      "$ARDUPI_DIR/arduPi.o" \
      -I"$ARDUPI_DIR" \
      -I"$ARDUPIAPI_DIR" \
      -I"$LIBRARY_DIR" \
      -o "$1_exe" 
    else
      echo "---------------HELP------------------"
      echo "Compiling: ./mak.sh filetocompile.cpp"
      echo "Cleaning:  ./mak.sh -clean"
      echo "-------------------------------------"
    fi
  else
    echo "ERROR No such file or directory: $file"
  fi
else
  echo "Spotless Kitchen!!"
fi
 
sleep 1

exit 0

