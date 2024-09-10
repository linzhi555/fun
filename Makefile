all:
	mkdir -p ./build
	cmake -B ./build
	cmake --build ./build
clean:
	cmake --build ./build --target clean
run:all
	./build/fun
test:
	echo "hello"

install:all
	mv ./build/fun   ~/.local/bin/

