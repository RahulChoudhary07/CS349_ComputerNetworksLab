Compiling:

gcc server.c -o server
gcc client.c -o client

encode.c and decode.c dont need to be compiled as they only have functions which are finally imported in server and client files.

Running:

Start the server which will take its port number as argument. 
E.g. ./server 5000
Start the client which will take server ip address and server port number as arguments. The server is bind to all available local interface ips. (Eg: 127.0.0.1)
E.g. ./client 127.0.0.1 5000


Properties

-> Both client and server work on TCP sockets.
-> Server should run before client as client will connect to the server as soon as it runs.
-> Server waits for TCP connection from client. 
-> Server starts a new child thread ( using fork() ) for every client so that multiple clients can be handled concurrently.


WorkFlow
-> After client-server connection setup, the client asks user to give input which is then encoded using Base64 encoding (max input lenght allowed is 1000).
-> The client sends the encoded message to the server(Type 1 msg).
-> After receiving the msg, server prints both encoded and decoded message and sends encoded "ACK" to the client(Type 2 msg).
-> Server and client remain in the loop to send any number of messages which the client wishes.
-> When user doesnt wish to send more messages, client sends a type 3 msg to server and connections is closed.
