# Maintainer: Ripose <ripose@protonmail.com>
pkgname=memento
pkgver=0.5.1
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
source=("${pkgname}.tar.gz::https://github.com/ripose-jp/Memento/archive/refs/tags/v${pkgver}-beta.tar.gz")
sha256sums=('89e14a5d33dca7194f4bb30f5f760aa271775bf6e1c19cc73b97c59a8fab2ac5')

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