#!/usr/bin/env bash
for i in demo0 demo1 demo2 demo3; do
  pushd $i
  aclocal
  autoconf
  automake --add-missing
  ./configure
  make
  popd
done
