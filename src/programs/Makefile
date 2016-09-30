all:
	cc -o gen randomNumberGen.c
	cc -o listener randomNumListener.c
	cc -o block block.c
	cc -o block_s block_s.c
	cc -o tester tester.c
	cc -o testing testing.c
run: all
	./gen &; ./listener
test:
	cc -o tester tester.c
	./tester
test_block: all
	./block_s &; ./block

