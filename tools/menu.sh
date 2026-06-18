#!/bin/bash

echo "======================"
echo " Cosmic Engine Dev CLI"
echo "======================"

echo "1) Build C/C++ engine"
echo "2) Setup Python venv"
echo "3) Run Python image"
echo "4) Clean build"
echo "0) Exit"

read -p "Select: " opt

case $opt in
  1)
    make
    ;;
  2)
    python3 -m venv venv
    source venv/bin/activate
    pip install requests pillow
    ;;
  3)
    source venv/bin/activate
    python3 tools/run_img.py
    ;;
  4)
    make clean
    ;;
  0)
    exit 0
    ;;
esac
