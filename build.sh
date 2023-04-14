#!/usr/bin/env bash
if test -x "`command -v gmake`"; then
	MAKE=gmake
else
	MAKE=make
fi
for i in demo0 demo1 demo2 demo3; do
  pushd $i
  aclocal
  autoconf
  automake --add-missing
  ./configure
  ${MAKE}
  popd
done
