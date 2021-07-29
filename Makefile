.PHONY: setup compile debug install clean

release: setup
	cd build; cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
	cmake --build ./build

debug: setup
	cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..
	cmake --build ./build

install:
	cmake --build ./build --target install

setup:
	mkdir -p build

clean:
	rm -rf build/

uninstall:
	rm -rf /usr/local/bin/memento /usr/local/share/applications/memento.desktop /usr/local/share/icons/hicolor/scalable/apps/memento.svg