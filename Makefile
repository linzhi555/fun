.PHONY: build clean run test fmt install

build:
	mkdir -p ./build
	cmake -B ./build
	cmake --build ./build
clean:
	cmake --build ./build --target clean
run:build
	./build/fun
test:build
	ctest -V --test-dir ./build
fmt:
	clang-format -i ./src/*.cpp ./src/*.h

install:all
	mv ./build/fun   ~/.local/bin/

