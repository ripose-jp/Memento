# Maintainer: Ripose <ripose@protonmail.com>
pkgname=memento
pkgver=2.1.0
pkgrel=0
pkgdesc="An mpv-based video player for studying Japanese."
arch=('x86_64' 'aarch64')
url="https://ripose-jp.github.io/Memento/"
license=('GPL2')
depends=(
    'json-c'
    'libzip'
    'mecab-git'
    'mecab-ipadic'
    'mpv'
    'qcoro'
    'qt6-base'
    'qt6-svg'
    'sqlite'
)
makedepends=(
    'cmake'
    'gcc'
    'git'
    'make'
    'qt6-tools'
)
optdepends=(
    'kimageformats: support for more image formats in dictionaries'
    'noto-fonts-cjk: optimal font support'
    'yt-dlp: streaming support'
)
source_base="https://github.com/ripose-jp/Memento/archive/refs/tags"
source=("${pkgname}-${pkgver}.tar.gz::${source_base}/v${pkgver}.tar.gz")
sha256sums=('6235b2a805d6539d3af78ecb8189ce3706f1f4b019656bde5b35409a652af775')

prepare() {
    mkdir -p ${srcdir}/build
}

build() {
    cd ${srcdir}/build
    cmake -DCMAKE_INSTALL_PREFIX:PATH=${pkgdir}/usr \
          -DCMAKE_BUILD_TYPE=Release \
          -DMEMENTO_RELEASE_BUILD=ON \
          -DMEMENTO_QAPPLICATION=ON \
          -DMEMENTO_SYSTEM_QCORO=ON \
          -DMEMENTO_MECAB_SUPPORT=ON \
          "${srcdir}/Memento-${pkgver}"
    cmake --build . -j $(grep -c ^processor /proc/cpuinfo)
}

package() {
    cd ${srcdir}/build
    make install
}
