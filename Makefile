.PHONY: setup compile debug install clean uninstall appimage appbundle appbundle_x86

release: setup
	cd build; \
	cmake -DCMAKE_BUILD_TYPE=Release ${CMAKE_ARGS} ..; \
	cmake --build . -j$(nproc)

debug: setup
	cd build; \
	cmake -DCMAKE_BUILD_TYPE=Debug ${CMAKE_ARGS} ..; \
	cmake --build . -j$(nproc)

install:
	cd build; \
	cmake --build . --target install -j$(nproc)

setup:
	mkdir -p build

clean:
	rm -rf build/

uninstall:
	rm -rf /usr/local/bin/memento \
		   /usr/local/share/applications/memento.desktop \
		   /usr/local/share/icons/hicolor/scalable/apps/memento.svg

appimage: setup
	cd build; \
	cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DAPPIMAGE=ON ${CMAKE_ARGS} ..; \
	cmake --build . -j$(nproc); \
	make install DESTDIR=AppDir; \
	mkdir -p ./AppDir/usr/lib/mecab; \
	mkdir -p ./AppDir/usr/lib/mecab/dic; \
	cp -r ../dic/ipadic ./AppDir/usr/lib/mecab/dic; \
	wget -nc https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage; \
	wget -nc https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage; \
	wget -nc https://github.com/linuxdeploy/linuxdeploy-plugin-appimage/releases/download/continuous/linuxdeploy-plugin-appimage-x86_64.AppImage; \
	chmod +x linuxdeploy-x86_64.AppImage \
			 linuxdeploy-plugin-qt-x86_64.AppImage \
			 linuxdeploy-plugin-appimage-x86_64.AppImage; \
	export VERSION=1.4.1; \
	./linuxdeploy-x86_64.AppImage --appdir AppDir --plugin qt --output appimage

appbundle: setup
	cd build; \
	cmake -DAPPBUNDLE=ON \
	      -DCERT=${CERT_NAME} \
		  -DCMAKE_BUILD_TYPE=Release \
		  ${CMAKE_ARGS} \
		  ..; \
	cmake --build . -j$(nproc);

# This target is for building x86_64 app bundles on arm Macs
appbundle_x86: setup
	cd build; \
	cmake -DAPPBUNDLE=ON \
		  -DMAC_CROSSCOMPILE_X86=ON \
		  -DCERT=${CERT_NAME} \
		  -DCMAKE_BUILD_TYPE=Release \
		  ${CMAKE_ARGS} \
		  ..; \
	cmake --build . -j$(nproc);
