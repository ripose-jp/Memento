# Maintainer: Ripose <ripose@protonmail.com>
pkgname=memento
pkgver=1.0.0
pkgrel=0
pkgdesc="An mpv-based video player for studying Japanese."
arch=('x86_64')
url="https://ripose-jp.github.io/Memento/"
license=('GPL2')
depends=(
    'mpv'
    'qt5-base'
    'qt5-svg'
    'qt5-x11extras'
    'sqlite'
    'json-c'
    'libzip'
    'mecab'
    'mecab-ipadic'
)
makedepends=('git' 'make' 'cmake' 'gcc')
optdepends=(
    'youtube-dl: streaming support'
    'mpv-git: additional secondary subtitle support'
    'noto-fonts-cjk: optimal font support'
)
source=("${pkgname}-${pkgver}.tar.gz::https://github.com/ripose-jp/Memento/archive/refs/tags/v${pkgver}.tar.gz")
sha256sums=('7cc9a338899ed4f0a87fb43c1dd25183220570225801f2d1e50083a91359f74c')

prepare() {
    mkdir -p ${srcdir}/build
}

build() {
    cd ${srcdir}/build
    cmake -DCMAKE_INSTALL_PREFIX:PATH=${pkgdir}/usr \
          -DRELEASE_BUILD=ON \
          -DCMAKE_BUILD_TYPE=Release \
          "${srcdir}/Memento-${pkgver}"
    make -j $(grep -c ^processor /proc/cpuinfo)
}

package() {
    cd ${srcdir}/build
    make install
}