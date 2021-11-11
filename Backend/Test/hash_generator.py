#!/usr/bin/python

import os
import hashlib,binascii

root='./dictionary'
# l=list(os.walk(root))
# l=l[0][2]
# input_files=list()
# for i in l:
# 	input_files.append(i)

# print(input_files)		
# print(len(l)," " ,len(input_files))

# for i in range(0,len(input_files)):
IFile='./Dictionary/common_password.txt';
OFile='./HashFiles/ntlm_hash.txt';	
#path=os.getcwd();
fin=open(IFile,"rb");
fout=open(OFile,"a");
lines=fin.readlines()
for line in lines:
	line=line.strip()
	hash_val = binascii.hexlify(hashlib.new('md4', line.decode('UTF-8').encode('utf-16le')).digest())
	(line,hash_val)=(line.decode('UTF-8'),hash_val.decode('UTF-8'))
	print(line,hash_val)
	# exit()
	fout.write(hash_val);
	# print(hash_val)
	fout.write("\n");
fin.close();
fout.close();
