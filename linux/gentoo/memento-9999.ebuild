# Copyright 2023 Ripose
# Distributed under the terms of the GNU General Public License v2

EAPI=8

inherit git-r3 cmake xdg

DESCRIPTION="An mpv-based video player for studying Japanese"
HOMEPAGE="https://ripose-jp.github.io/Memento/"
EGIT_REPO_URI="https://github.com/ripose-jp/Memento.git"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~arm64"

DEPEND="
    dev-libs/libzip
    app-text/mecab
    app-dicts/mecab-ipadic
    dev-db/sqlite
    media-video/mpv:=[libmpv]
    dev-libs/json-c
    >=dev-qt/qtcore-5.15.0:5
    >=dev-qt/qtopengl-5.15.0:5
    >=dev-qt/qtgui-5.15.0:5
    >=dev-qt/qtx11extras-5.15.0:5
    >=dev-qt/qtsvg-5.15.0:5
    >=dev-qt/qtnetwork-5.15.0:5
    >=dev-qt/qtdbus-5.15.0:5
    >=dev-qt/qtconcurrent-5.15.0:5"
RDEPEND="
    ${DEPEND}
    media-fonts/noto-cjk"
BDEPEND="
    >=dev-util/cmake-3.15.0"

src_configure()
{
    local mycmakeargs=(
        "-DBUILD_SHARED_LIBS=OFF"
    )
    cmake_src_configure
}
