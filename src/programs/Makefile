all:
	cc -o gen randomNumberGen.c
	cc -o listener randomNumListener.c
	cc -o block block.c
	cc -o block_s block_s.c
	cc -o tester tester.c
	cc -o testing testing.c
	cc -o website-list website-list.c
run: all
	./gen &; ./listener
test: all
	cc -o tester tester.c
	./tester
test_block: all
	./block_s &; ./block
test_multiblock: all
	./block_s &; ./block &; ./block &; ./block
test_web: all
	./website-list
test_itall: all
	./website-list &; ./block_s &; ./block &; ./block &; ./block &; ./gen &; ./listener

