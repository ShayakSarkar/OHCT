import time

fifo_write=open('./buffer','w')

while True:
    fifo_write.write('a')
    fifo_write.flush()
    time.sleep(1)
    

