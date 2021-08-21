.PHONY: setup compile debug install clean uninstall

release: setup
	cd build; \
	cmake -DCMAKE_BUILD_TYPE=Release ..; \
	make -j$(nproc)

debug: setup
	cd build; \
	cmake -DCMAKE_BUILD_TYPE=Debug ..; \
	make -j$(nproc)

install:
	cd build; \
	make install -j$(nproc)

setup:
	mkdir -p build

clean:
	rm -rf build/

uninstall:
	rm -rf /usr/local/bin/memento \
		   /usr/local/share/applications/memento.desktop \
		   /usr/local/share/icons/hicolor/scalable/apps/memento.svg
