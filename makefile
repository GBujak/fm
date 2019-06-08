make: 
	gcc src/*.c -lncursesw -o fm -Wall

clean:
	rm fm