#!/usr/bin/env bash
for i in demo0 demo1 demo2 demo3; do
  pushd $i
  for j in gtk3 gtk4; do
	  for k in freeglut glfw sdl glarea; do
		  echo "$i/$j-opengl-$k"
		  ./$j-opengl-$k
	  done
  done
  popd
done
