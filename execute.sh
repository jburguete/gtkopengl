#!/usr/bin/env bash
for i in demo0 demo1 demo2; do
	pushd $i
	for j in gtk3 gtk4; do
		for k in glfw sdl glarea; do
			l="$j-opengl-$k"
			if test -x $l; then
				echo "$l"
		  		./$l
			fi
		done
	done
	popd
done
