fifo_read=open('buffer','r')

while True:
    data=fifo_read.read(1)
    print('"',data,'"read from buffer')


