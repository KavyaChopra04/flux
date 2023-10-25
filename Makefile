all:
	g++ -o a3_basic_2 md5_hash/hashlibpp.h md5_hash/hl_md5.o md5_hash/hl_md5wrapper.o a3_basic_m2.cpp
	time ./a3_basic_m2 > log.txt
	python3 plotGraph.py
