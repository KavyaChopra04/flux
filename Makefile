all:
	g++ -o a3_basic md5_hash/hashlibpp.h md5_hash/hl_md5.o md5_hash/hl_md5wrapper.o a3_basic.cpp
	time ./a3_basic 