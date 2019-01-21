ECHO ===============================================================
ECHO Testing #1 
ECHO Protocol              : TCP
ECHO Number of Blocks Sent : 1000 bytes
ECHO Number of Blocks      : 50000
ECHO ===============================================================

CALL %1 -TCP -a %2 -p %3 -s 1000 -n 50000
timeout /t 3 /nobreak

ECHO ===============================================================
ECHO Testing #2 
ECHO Protocol              : TCP
ECHO Number of Blocks Sent : 2000 bytes
ECHO Number of Blocks      : 25000 
ECHO ===============================================================


CALL %1 -TCP -a %2 -p %3 -s 2000 -n 25000
timeout /t 3 /nobreak

ECHO ===============================================================
ECHO Testing #3 
ECHO Protocol              : TCP
ECHO Number of Blocks Sent : 5000 bytes
ECHO Number of Blocks      : 10000 
ECHO ===============================================================


CALL %1 -TCP -a %2 -p %3 -s 5000 -n 10000
timeout /t 3 /nobreak

ECHO ===============================================================
ECHO Testing #4 
ECHO Protocol              : TCP
ECHO Number of Blocks Sent : 10000 bytes
ECHO Number of Blocks      : 5000 
ECHO ===============================================================


CALL %1 -TCP -a %2 -p %3 -s 10000 -n 5000
timeout /t 3 /nobreak


ECHO ===============================================================
ECHO Testing #5 
ECHO Protocol              : UDP
ECHO Number of Blocks Sent : 1000 bytes
ECHO Number of Blocks      : 50000 
ECHO ===============================================================

CALL %1 -UDP -a %2 -p %3 -s 1000 -n 50000
timeout /t 5 /nobreak

ECHO ===============================================================
ECHO Testing #6 
ECHO Protocol              : UDP
ECHO Number of Blocks Sent : 2000 bytes
ECHO Number of Blocks      : 25000 
ECHO ===============================================================

CALL %1 -UDP -a %2 -p %3 -s 2000 -n 25000
timeout /t 5 /nobreak

ECHO ===============================================================
ECHO Testing #7 
ECHO Protocol              : UDP
ECHO Number of Blocks Sent : 5000 bytes
ECHO Number of Blocks      : 10000 
ECHO ===============================================================

CALL %1 -UDP -a %2 -p %3 -s 5000 -n 10000
timeout /t 5 /nobreak

ECHO ===============================================================
ECHO Testing #8 
ECHO Protocol              : UDP
ECHO Number of Blocks Sent : 10000 bytes
ECHO Number of Blocks      : 5000 
ECHO ===============================================================

CALL %1 -UDP -a %2 -p %3 -s 10000 -n 5000


ECHO ==  All tests are done!  ==