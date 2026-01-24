# Maintainer: Ripose <ripose@protonmail.com>
pkgname=memento
pkgver=1.7.1
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
makedepends=('git' 'make' 'cmake' 'gcc')
optdepends=(
    'yt-dlp: streaming support'
    'noto-fonts-cjk: optimal font support'
)
source=("${pkgname}-${pkgver}.tar.gz::https://github.com/ripose-jp/Memento/archive/refs/tags/v${pkgver}.tar.gz")
sha256sums=('0e031fc74dbaf52d94c43d3cb56742594f8ba308af4f95caaa445c7c8ac98418')

prepare() {
    mkdir -p ${srcdir}/build
}

build() {
    cd ${srcdir}/build
    cmake -DCMAKE_INSTALL_PREFIX:PATH=${pkgdir}/usr \
          -DRELEASE_BUILD=ON \
          -DCMAKE_BUILD_TYPE=Release \
          -DSYSTEM_QCORO=ON \
          -DMECAB_SUPPORT=ON \
          "${srcdir}/Memento-${pkgver}"
    cmake --build . -j $(grep -c ^processor /proc/cpuinfo)
}

package() {
    cd ${srcdir}/build
    make install
}
