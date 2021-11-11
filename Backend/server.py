import flask,time,id_generator,os,filelock

app=flask.Flask(__name__)

def create_output_fifo(file_name):

    # used to get the result from the cpp_engine
    # fifo will be written to by the cpp_engine and read by get_output()

    os.mkfifo('./FIFOs/'+file_name)


def get_request_id():

    # used to get the request id for the current request
    # makes use of the id_generator module
    # first tries to get a lock in the LockFiles folder so that the id generated is surely unique for each request

    lock_file=filelock.UnixFileLock('./LockFiles/id_generator_lock')
    lock_file.acquire()

    request_id=id_generator.generate_id()

    lock_file.release()
    return request_id


def push_into_fifo(data,request_id):
    
    # writes the hash, hash_type and the request_id into the fifo for hash_cracking_engine
    # writes to the fifo will be atomic because they will always be less than the size of PIPE_BUF

    fifo_write=open('./FIFOs/hash_cracking_engine_fifo','w')
    fifo_write.write(data['hash']+' '+data['hash_type']+' '+request_id+'\n')
    fifo_write.flush()


def get_output(op_file_name):
    fifo_read=open('FIFOs/'+op_file_name,'r')
    result=fifo_read.readline()
    os.remove('FIFOs/'+op_file_name)
    return result

def crack_hash(data):
    request_id=get_request_id()
    create_output_fifo(request_id)  # the file where the ouput of the hash cracking will be stored
                                    # by the c++ engine

    print('server.py: crack_hash(): request id is',request_id)
    push_into_fifo(data,request_id)

    result=get_output(request_id)
    return result

if __name__ == '__main__':
    @app.route('/send_data/',methods=['POST'])
    def send_data():
        req=flask.request.get_json(force=True)
        print('the request is:',req)
        print('processing request, data is sent to hash cracking engine')
        plaintext=crack_hash(req)
        resp=flask.jsonify({'plaintext': plaintext})
        resp.headers['Access-Control-Allow-Origin']='http://localhost:3000'
        resp.headers['Access-Control-Allow-Credentials']='true'
        #resp.headers['Status']='200'
        return resp


app.run(debug=True)

