#!/usr/bin/env bash
if [[ `uname -s` = "OpenBSD" ]]; then
	export AUTOCONF_VERSION=2.71
	export AUTOMAKE_VERSION=1.16
fi
if test -x "`command -v gmake`"; then
	MAKE=gmake
else
	MAKE=make
fi
aclocal
autoconf
automake --add-missing
./configure
for i in demo0 demo1 demo2; do
	pushd $i
	${MAKE} $1
	popd
done
