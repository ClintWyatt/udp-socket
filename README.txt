This program is a udp server and client set up testing the select statement. 2 processes, 
the server and client, must be running in 2 seprate linux command lines for this program to work.

To run the server program, follow the instructions below:

1. Compile the server by typing gcc udpSvr.c
2. To run, type ./a.out with a port number between 1024 and 49151

Example on how to compile and run the server on the machine cse06: 
gcc udpSvr.c
./a.out 15000

To run the client program, the server program must be running first
and you must have a different linux command line window opened. Follow the instructions below:

1. Compile the client by typing gcc udpClient.c
2. To Run, type ./a.out with the machine name the server program is running on and the server port

Example:
gcc udpClient.c
./a.out cse06 15000
