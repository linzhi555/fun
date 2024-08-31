all:
	cd build && cmake .. && make 

run:all
	./build/fun
