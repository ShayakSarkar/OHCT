import os,time

cpp_engine_fifo_read=open('../FIFOs/hash_cracking_engine_fifo','r')

while True:
    data=cpp_engine_fifo_read.readline()[:-1]
    if data=="":
        # this section is required as the FIFO continues to read ""
        # from the FIFO as soon as the writing end is closed. 
        # Therefore to make the reader block until anything is 
        # written on the other end of the FIFO, we close the reader 
        # and open it again 
        cpp_engine_fifo_read.close()
        cpp_engine_fifo_read=open('../FIFOs/hash_cracking_engine_fifo','r')
        data=cpp_engine_fifo_read.readline()[:-1]

    print('data',data.split())
    [hash_value,hash_type,request_id]=data.split()
    result_fifo_write=open('../FIFOs/'+request_id,'w')
    result_fifo_write.write('result from cpp engine\n')
    result_fifo_write.flush()
    # input('enter a value')
    
    
    