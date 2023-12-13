all_3:
	g++ -o Milestone_3/milestone_3_roundrobin md5_hash/hashlibpp.h md5_hash/hl_md5.o md5_hash/hl_md5wrapper.o Milestone_3/milestone_3_roundrobin.cpp
	time ./Milestone_3/milestone_3_roundrobin > log.txt
all_basic_3:
	g++ -o Milestone_3/milestone_3_bursty md5_hash/hashlibpp.h md5_hash/hl_md5.o md5_hash/hl_md5wrapper.o Milestone_3/milestone_3_bursty.cpp
	time ./Milestone_3/milestone_3_bursty > log.txt
all_2:
	g++ -o Milestone_2/milestone_2_roundrobin md5_hash/hashlibpp.h md5_hash/hl_md5.o md5_hash/hl_md5wrapper.o Milestone_2/milestone_2_roundrobin.cpp
	time ./Milestone_2/milestone_2_roundrobin > log.txt
all_basic_2:
	g++ -o Milestone_2/milestone_2_bursty md5_hash/hashlibpp.h md5_hash/hl_md5.o md5_hash/hl_md5wrapper.o Milestone_2/milestone_2_bursty.cpp
	time ./Milestone_2/milestone_2_bursty > log.txt
reliable:
	g++ -o Milestone_1/milestone_1_normal md5_hash/hashlibpp.h md5_hash/hl_md5.o md5_hash/hl_md5wrapper.o Milestone_1/milestone_1_normal.cpp
	time ./Milestone_1/milestone_1_normal > log.txt