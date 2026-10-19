#!/bin/bash

#  Command line utility to generate an Angelcode BMfont text descriptor and corresponding TGA texture
#
#  usage: <fontfile> [fontsize]
#
#  Dependencies:
#      https://imagemagick.org/
#      https://github.com/vladimirgamalyan/fontbm
    

set -euo pipefail

if [ $# -lt 1 ]; then
	echo "usage: $0 <fontfile> [fontsize]" >&2
	exit 1
fi

font="$1"
size="${2:-24}"
name="$(basename "$font")"
name="${name%.*}"

~/tools/fontbm/fontbm --font-file "$font" --output "$name" --chars 32-255 --font-size "$size" --kerning-pairs basic --monochrome --max-texture-count 1 --padding-up 1 --padding-down 1 --padding-right 1 --padding-left 1

convert "${name}_0.png" "${name}_0.tga"
rm "${name}_0.png"

sed -i '0,/\.png/s/\.png/.tga/' "${name}.fnt"
