# Maintainer: Ripose <ripose@protonmail.com>
pkgname=memento
pkgver=2.0.1
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
sha256sums=('79667450a71f642ccee1027fb496ed5bcb78372844dd89fc9f33b77a907d6cfe')

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
