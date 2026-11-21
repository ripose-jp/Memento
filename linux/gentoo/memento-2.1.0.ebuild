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
IUSE="kimageformats mecab widgets"

DEPEND="
	>=dev-qt/qtbase-6.10.0:6
	dev-db/sqlite
	dev-libs/json-c
	dev-libs/libzip
	dev-libs/qcoro
	dev-qt/qtsvg
	mecab? ( app-dicts/mecab-ipadic )
	mecab? ( app-text/mecab )
	media-video/mpv:=[libmpv]
"
RDEPEND="
	${DEPEND}
	kimageformats? ( kde-frameworks/kimageformats )
	media-fonts/noto-cjk
"
BDEPEND="
	>=dev-build/cmake-3.16.0
"

src_configure()
{
	local mycmakeargs=(
		"-DBUILD_SHARED_LIBS=OFF"
		"-DMEMENTO_RELEASE_BUILD=ON"
		"-DMEMENTO_SYSTEM_QCORO=ON"
		"-DMEMENTO_QAPPLICATION=$(use widgets && echo ON || echo OFF)"
		"-DMEMENTO_MECAB_SUPPORT=$(use mecab && echo ON || echo OFF)"
	)
	cmake_src_configure
}
