all:
	cc -o gen randomNumberGen.c
	cc -o listener randomNumListener.c
run: all
	./gen &; ./listener
test:
	cc -o tester tester.c
	./tester

