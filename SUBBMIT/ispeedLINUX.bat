clear

echo ===============================================================
echo Testing NO.1 
echo Protocol              : TCP
echo Number of Blocks Sent : 1000 bytes
echo Number of Blocks      : 50000 
echo ===============================================================

$1 -TCP -a $2 -p $3 -s 1000 -n 50000 && sleep 3s


echo ===============================================================
echo Testing NO.2 
echo Protocol              : TCP
echo Number of Blocks Sent : 2000 bytes
echo Number of Blocks      : 25000 
echo ===============================================================


$1 -TCP -a $2 -p $3 -s 2000 -n 25000 && sleep 3s

echo ===============================================================
echo Testing NO.3 
echo Protocol              : TCP
echo Number of Blocks Sent : 5000 bytes
echo Number of Blocks      : 10000 
echo ===============================================================


$1 -TCP -a $2 -p $3 -s 5000 -n 10000 && sleep 3s


echo ===============================================================
echo Testing NO.4 
echo Protocol              : TCP
echo Number of Blocks Sent : 10000 bytes
echo Number of Blocks      : 5000 
echo ===============================================================


$1 -TCP -a $2 -p $3 -s 10000 -n 5000 && sleep 3s


echo ===============================================================
echo Testing NO.5 
echo Protocol              : UDP
echo Number of Blocks Sent : 1000 bytes
echo Number of Blocks      : 50000 
echo ===============================================================

$1 -UDP -a $2 -p $3 -s 1000 -n 50000 && sleep 4s


echo ===============================================================
echo Testing NO.6 
echo Protocol              : UDP
echo Number of Blocks Sent : 2000 bytes
echo Number of Blocks      : 25000 
echo ===============================================================

$1 -UDP -a $2 -p $3 -s 2000 -n 25000 && sleep 4s


echo ===============================================================
echo Testing NO.7 
echo Protocol              : UDP
echo Number of Blocks Sent : 5000 bytes
echo Number of Blocks      : 10000 
echo ===============================================================

$1 -UDP -a $2 -p $3 -s 5000 -n 10000 && sleep 4s


echo ===============================================================
echo Testing NO.8 
echo Protocol              : UDP
echo Number of Blocks Sent : 10000 bytes
echo Number of Blocks      : 5000 
echo ===============================================================

$1 -UDP -a $2 -p $3 -s 10000 -n 5000


echo ==  All tests are done!  ==