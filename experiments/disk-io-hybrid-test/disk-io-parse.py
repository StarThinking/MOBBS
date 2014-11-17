#!/usr/bin/python

import os
import csv


def readFile(dirname):
	cmd = 'cd ' + dirname + ';./analyze.sh'
	return os.popen(cmd).readlines()

def parseLine(line):
	res = []
	tmp = line.split(' ')
	res.append(tmp[0])
	res.append(tmp[4][0:-1])
	res.append(tmp[7])
	tmp2 = float(tmp[7]) / 1024
	res.append(str(tmp2))
	return res

def parseFile(prefix, desFile):
	writer = csv.writer(desFile)

	writer.writerow(['####', '####', prefix, '####', '####'])
	writer.writerow([' ',' ',' ','SSD',' ',' ',' ',' ',' ','HDD',' ',' ',' '])
	writer.writerow([' '] + [' ','RAND',' ',' ','SEQ',' '] * 2)
	writer.writerow(['SIZE'] + ['IOPS','BW_KB','BW_MB'] * 4)

	r1 = readFile('ran-' + prefix + '-ssd')
	r2 = readFile('seq-' + prefix + '-ssd')
	r3 = readFile('ran-' + prefix + '-hdd')
	r4 = readFile('seq-' + prefix + '-hdd')

	for i in range(len(r1)):
		l1 = parseLine(r1[i])
		l2 = parseLine(r2[i])
		l3 = parseLine(r3[i])
		l4 = parseLine(r4[i])
		writer.writerow(l1 + l2[1:] + l3[1:] + l4[1:])


def parseFile2(desFile):
	writer = csv.writer(desFile)

	writer.writerow(['####','####','SSD/HDD','####','####'])
	writer.writerow(['RAND_READ','RAND_WRITE','SEQ_READ','SEQ_WRITE'])

	rr1 = readFile('ran-read-ssd')
	rr2 = readFile('ran-read-hdd')
	
	rw1 = readFile('ran-write-ssd')
	rw2 = readFile('ran-write-hdd')

	sr1 = readFile('seq-read-ssd')
	sr2 = readFile('seq-read-hdd')

	sw1 = readFile('seq-write-ssd')
	sw2 = readFile('seq-write-hdd')

	for i in range(len(rr1)):
		line = []

		rrl1 = parseLine(rr1[i])
		rrl2 = parseLine(rr2[i])
		tmp = float(rrl1[2]) / float(rrl2[2])
		line.append(str(tmp))

		rwl1 = parseLine(rw1[i])
		rwl2 = parseLine(rw2[i])
		tmp = float(rwl1[2]) / float(rwl2[2])
		line.append(str(tmp))

		srl1 = parseLine(sr1[i])
		srl2 = parseLine(sr2[i])
		tmp = float(srl1[2]) / float(srl2[2])
		line.append(str(tmp))

		swl1 = parseLine(sw1[i])
		swl2 = parseLine(sw2[i])
		tmp = float(swl1[2]) / float(swl2[2])
		line.append(str(tmp))

		writer.writerow(line)

### main
f = file('result.csv', 'wb')
parseFile('read', f)
parseFile('write', f)
parseFile2(f)
f.close()




