import socket
import sys

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = "spring-boot-complete-1630332816013.azurewebsites.net"
print(host)
client_socket.connect((host, 8080))

while 1:
    data = input( "SEND( TYPE q or Q to Quit):" )
    if (data == 'Q' or data == 'q'): 
        client_socket.send(str.encode(data + '\n') )
    else:
        client_socket.send(str.encode(data + '\n'))
      #  client_socket.close()
        

