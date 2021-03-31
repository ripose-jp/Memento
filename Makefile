.PHONY: setup compile debug install clean

release: setup
	cd build; cmake -DCMAKE_BUILD_TYPE=Release ..
	cmake --build ./build

debug: setup
	cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..
	cmake --build ./build

install:
	cmake --build ./build --target install

setup:
	mkdir build

clean:
	rm -rf build/
