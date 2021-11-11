def generate_id():
	#read the file
	#read file line by line, and print the content of line
	
	file1=open("counter.txt","r")
	lines=file1.readlines()
	num='1'
	for line in lines:
		num=line

	file1.close()
	
	#write into the file
	#covert string to int to +1 and then again convert it back to string
	
	file2=open("counter.txt","w")
	num=int(num)
	num+=1
	num=str(num)
	
	file2.write(num)
	file2.close()
	return 'id'+num

	
	
