# NOTE: Tweak the code to make it work correctly
# import the socket library
import socket

# create a socket object
s = socket.socket()
print ("Socket successfully created")

# reserve a port on your computer
# it can be anything as long it is not a reserved port
port = 7000

# Next bind to the port
s.bind(('', port))
print ("socket binded to %s" %(port))

# put the socket into listening mode
s.listen(5)
print ("socket is listening")

# Establish connection with client.
c, addr = s.accept()
print ('Got connection from', addr)

# a forever loop until we interrupt it or an error occurs
while True:
    message = input("Car input: ")    
    c.send(message.encode())

    # Close the connection with the client
    # c.close()