Compiling:

gcc server.c -o server
gcc client.c -o client

encode.c and decode.c dont need to be compiled as they only have functions which are finally imported in server and client files.

Running:

Start the server which will take its port number as argument. 
E.g. ./server 5000
Start the client which will take server ip address and server port number as arguments. (By default server runs on localhost - 127.0.0.1)
E.g. ./client 127.0.0.1 5000


Properties

Both client server work on TCP sockets.
Server file should be running before running client file as client will connect to the server as soon as it runs.
Server will be waiting for TCP connection from client. 
Server starts a new child thread for every client so that multiple clients can be handled concurrently
After successfull connection setup, the client asks users to give input which is then encoded using Base64 encoding (max length 1500).
The client sends the encoded message to the server by writing to the socket. This is type 1 msg
After receiving the msg, server prints both original message and  decoded message and sends encoded "ACK" to the client, which is type 2 msg.
Server and Client remain in the loop to send any number of messages which the client wishes.
If client wants to close the connection, it sends a type 3 msg to server and connections is closed.