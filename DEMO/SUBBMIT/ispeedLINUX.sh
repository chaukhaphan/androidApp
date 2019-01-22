#!/bin/bash
clear
echo ""
echo "== WE ARE TESTING THE SPEED OF TCP/IP AND UDP. PLEASE ENTER THE FOLLOWING INFORMATION =="
echo ""
echo ""
read -p "Please enter the executable file path:" filename
read -p "Please enter the IP:" IP
read -p "Please enter the PORT:" port
echo ""


echo "======================= WE ARE TESTING TCP ==========================="
sleep 1s
echo ""
echo ""
echo "SEND #1:"
echo "Type: TCP, IP:$IP, Block Size: 1000, Number of Block: 1000"
echo "RECIEVE #1:"
$filename -TCP -a $IP -p $port -s 1000 -n 1000

sleep 1s
echo ""
echo ""
echo "SEND #2:"
echo "Type: TCP, IP:$IP, Block Size: 2000, Number of Block: 1000"
echo "RECIEVE #2:"
$filename -TCP -a $IP -p $port -s 2000 -n 1000


sleep 1s
echo ""
echo ""
echo "SEND #3:"
echo "Type: TCP, IP:$IP, Block Size: 5000, Number of Block: 800"
echo "RECIEVE #3:"
$filename -TCP -a $IP -p $port -s 5000 -n 800

sleep 1s
echo ""
echo ""
echo "SEND #4:"
echo "Type: TCP, IP:$IP, Block Size: 10000, Number of Block: 500"
echo "RECIEVE #2:"
$filename -TCP -a $IP -p $port -s 10000 -n 500

sleep 1s
echo ""
echo ""
echo "SEND #5:"
echo "Type: TCP, IP:$IP, Block Size: 1000, Number of Block: 2000"
echo "RECIEVE #5:"
$filename -TCP -a $IP -p $port -s 1000 -n 2000

sleep 1s
echo ""
echo ""
echo "======================= WE ARE TESTING UDP ==========================="
sleep 1s
echo ""
echo ""
echo "SEND #1:"
echo "Type: UDP, IP:$IP, Block Size: 1000, Number of Block: 1000"
echo "RECIEVE #1:"
$filename -UDP -a $IP -p $port -s 1000 -n 1000

sleep 1s
echo ""
echo ""
echo "SEND #2:"
echo "Type: UDP, IP:$IP, Block Size: 2000, Number of Block: 1000"
echo "RECIEVE #2:"
$filename -UDP -a $IP -p $port -s 2000 -n 1000

sleep 1s
echo ""
echo ""
echo "SEND #3:"
echo "Type: UDP, IP:$IP, Block Size: 5000, Number of Block: 800"
echo "RECIEVE #3:"
$filename -UDP -a $IP -p $port -s 5000 -n 800

sleep 1s
echo ""
echo ""
echo "SEND #3:"
echo "Type: UDP, IP:$IP, Block Size: 10000, Number of Block: 500"
echo "RECIEVE #4:"
$filename -UDP -a $IP -p $port -s 10000 -n 500

sleep 1s
echo ""
echo ""
echo "SEND #5:"
echo "Type: UDP, IP:$IP, Block Size: 1000, Number of Block: 2000"
echo "RECIEVE #5:"
$filename -UDP -a $IP -p $port -s 1000 -n 2000
