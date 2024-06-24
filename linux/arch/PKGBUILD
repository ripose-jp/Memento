# Maintainer: Ripose <ripose@protonmail.com>
pkgname=memento
pkgver=1.4.0
pkgrel=0
pkgdesc="An mpv-based video player for studying Japanese."
arch=('x86_64')
url="https://ripose-jp.github.io/Memento/"
license=('GPL2')
depends=(
    'mpv'
    'qt6-base'
    'qt6-svg'
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
sha256sums=('164461e6a9685a7cab8653c9240fb381568618fe4f0d05b422ae3cd204e9c479')

prepare() {
    mkdir -p ${srcdir}/build
}

build() {
    cd ${srcdir}/build
    cmake -DCMAKE_INSTALL_PREFIX:PATH=${pkgdir}/usr \
          -DRELEASE_BUILD=ON \
          -DCMAKE_BUILD_TYPE=Release \
          -DMECAB_SUPPORT=ON \
          "${srcdir}/Memento-${pkgver}"
    cmake --build . -j $(grep -c ^processor /proc/cpuinfo)
}

package() {
    cd ${srcdir}/build
    make install
}
