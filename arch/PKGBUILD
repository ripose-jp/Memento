# Maintainer: Ripose <ripose@protonmail.com>
pkgname=memento
pkgver=0.5.2
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
source=("${pkgname}-${pkgver}-${pkgrel}.tar.gz::https://github.com/ripose-jp/Memento/archive/refs/tags/${pkgver}-beta.tar.gz")
sha256sums=('e9985d07fdf6ee40378a63c8623b2e4e4d81744ed2c6ffc44deeec836f0d0cd7')

prepare() {
    mkdir -p ${srcdir}/build
}

build() {
    cd ${srcdir}/build
    cmake -DCMAKE_INSTALL_PREFIX:PATH=${pkgdir}/usr \
          -DCMAKE_BUILD_TYPE=Release \
          "${srcdir}/Memento-${pkgver}-beta"
    make -j $(grep -c ^processor /proc/cpuinfo)
}

package() {
    cd ${srcdir}/build
    make install
}