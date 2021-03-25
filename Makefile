.PHONY: setup compile debug install clean

all: setup
	cmake --build ./build --config Release

debug: setup
	cmake --build ./build --config Debug

install:
	cmake --build ./build --target install

setup:
	mkdir build; cd build; cmake ..

clean:
	rm -rf build/
