all:
	g++ -o a3_m2_roundrobin md5_hash/hashlibpp.h md5_hash/hl_md5.o md5_hash/hl_md5wrapper.o a3_m2_roundrobin.cpp
	time ./a3_m2_roundrobin > log.txt
	python3 plotGraph.py
all_basic:
	g++ -o a3_m2_basic md5_hash/hashlibpp.h md5_hash/hl_md5.o md5_hash/hl_md5wrapper.o a3_m2_basic.cpp
	time ./a3_m2_basic > log.txt
