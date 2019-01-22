@ECHO off
Cls
ECHO:
ECHO == WE ARE TESTING THE SPEED OF TCP/IP AND UDP. PLEASE ENTER THE FOLLOWING INFORMATION ==
ECHO:

SET /p filename=Please enter the executable file path:
SET /p IP=Please enter the IP: 
SET /p port=Please enter the PORT: 
ECHO:
ECHO ======================= WE ARE TESTING TCP ===========================
timeout /t 3 /nobreak
ECHO: 
ECHO:
ECHO SEND #1: 
ECHO Type: TCP, IP:%IP%, Block Size: 1000, Number of Block: 1000
ECHO RECIEVE #1:
CALL %filename% -TCP -a %IP% -p %port% -s 1000 -n 1000
timeout /t 3 /nobreak

ECHO: 
ECHO:
ECHO SEND #2: 
ECHO Type: TCP, IP:%IP%, Block Size: 2000, Number of Block: 1000
ECHO RECIEVE #2:
CALL %filename% -TCP -a %IP% -p %port% -s 2000 -n 1000
timeout /t 3 /nobreak

ECHO: 
ECHO:
ECHO SEND #3: 
ECHO Type: TCP, IP:%IP%, Block Size: 5000, Number of Block: 800
ECHO RECIEVE #3:
CALL %filename% -TCP -a %IP% -p %port% -s 5000 -n 800
timeout /t 3 /nobreak

ECHO: 
ECHO:
ECHO SEND #4: 
ECHO Type: TCP, IP:%IP%, Block Size: 10000, Number of Block: 500
ECHO RECIEVE #5:
CALL %filename% -TCP -a %IP% -p %port% -s 10000 -n 500
timeout /t 3 /nobreak

ECHO: 
ECHO:
ECHO SEND #5: 
ECHO Type: TCP, IP:%IP%, Block Size: 1000, Number of Block: 2000
ECHO RECIEVE #2:
CALL %filename% -TCP -a %IP% -p %port% -s 1000 -n 2000
timeout /t 3 /nobreak



ECHO:
ECHO:

ECHO ======================= WE ARE TESTING UDP ===========================
timeout /t 3 /nobreak
ECHO: 
ECHO:
ECHO SEND #1: 
ECHO Type: UDP, IP:%IP%, Block Size: 1000, Number of Block: 1000
ECHO RECIEVE #1:
CALL %filename% -UDP -a %IP% -p %port% -s 1000 -n 1000
timeout /t 3 /nobreak


ECHO: 
ECHO:
ECHO SEND #2: 
ECHO Type: UDP, IP:%IP%, Block Size: 2000, Number of Block: 1000
ECHO RECIEVE #2:
CALL %filename% -UDP -a %IP% -p %port% -s 2000 -n 1000
timeout /t 3 /nobreak


ECHO: 
ECHO:
ECHO SEND #3: 
ECHO Type: UDP, IP:%IP%, Block Size: 5000, Number of Block: 800
ECHO RECIEVE #3:
CALL %filename% -UDP -a %IP% -p %port% -s 10000 -n 500
timeout /t 3 /nobreak


ECHO: 
ECHO:
ECHO SEND #4: 
ECHO Type: UDP, IP:%IP%, Block Size: 10000, Number of Block: 500
ECHO RECIEVE #4:
CALL %filename% -UDP -a %IP% -p %port% -s 10000 -n 500
timeout /t 3 /nobreak


ECHO: 
ECHO:
ECHO SEND #5: 
ECHO Type: UDP, IP:%IP%, Block Size: 1000, Number of Block: 2000
ECHO RECIEVE #5:
CALL %filename% -TCP -a %IP% -p %port% -s 1000 -n 2000

ECHO =================================================