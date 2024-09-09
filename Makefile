all:
	mkdir -p ./build
	cmake -B ./build
	cmake --build ./build

run:all
	./build/fun
test:
	echo "hello"