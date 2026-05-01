
Server program : server.c

Client program : client.c

To run server :

gcc server.c -o server

./server [port-number]

To run client :

Gcc client,c -o client

./client [server’s IP address] [port-number]

Instructions to use the client for sending messages

1.​As soon as you run the client, it asks to enter an id.

2.​Enter a positive integer. This works as the username of the current client.

3.​Now it asks to enter 1 or 2 or 3

4.​enter 1 for private message.It means select this option if you want to message to a particular client. Then it asks for the id
of the client to whom you are sending private messages.
Then you can send as many messages as you want. To end
the private chat enter “exit”.

5.​Enter 2 to send messages to everyone online(broadcast).
Then you can broadcast as many messages as you wish, at
last to end the broadcasting enter “exit”.

6.​Enter 3 to go offline that is to disconnect with the server and
end the client program.

7.​After exiting a particular type of chat the client can select any
of the 3 options 1 or 2 or 3 again

8.​NOTE: client can receive messages from anyone even if you
are in any mode(private ,or broadcast). But sending
messages works on which mode the client is in.
