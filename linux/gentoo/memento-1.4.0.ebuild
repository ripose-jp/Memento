# Copyright 2023 Ripose
# Distributed under the terms of the GNU General Public License v2

EAPI=8

inherit cmake xdg

DESCRIPTION="An mpv-based video player for studying Japanese"
HOMEPAGE="https://ripose-jp.github.io/Memento/"
SRC_URI="
    https://github.com/ripose-jp/Memento/archive/refs/tags/v${PV}.tar.gz
    -> ${P}.tar.gz"
S="${WORKDIR}/Memento-${PV}"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~arm64"
IUSE="mecab"

DEPEND="
    dev-libs/libzip
    mecab? ( app-text/mecab )
    mecab? ( app-dicts/mecab-ipadic )
    dev-db/sqlite
    media-video/mpv:=[libmpv]
    dev-libs/json-c
    >=dev-qt/qtbase-6.7.0:6"
RDEPEND="
    ${DEPEND}
    media-fonts/noto-cjk"
BDEPEND="
    >=dev-build/cmake-3.15.0"

src_configure()
{
    local mycmakeargs=(
        "-DBUILD_SHARED_LIBS=OFF"
        "-DRELEASE_BUILD=ON"
        "-DMECAB_SUPPORT=$(use mecab && echo ON || echo OFF)"
    )
    cmake_src_configure
}
