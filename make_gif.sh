cd output

ffmpeg -i ./frame_%03d.png -vf "palettegen=max_colors=48" ./palette.png -y

ffmpeg -i ./frame_%03d.png -i ./palette.png -lavfi "paletteuse=dither=none" -framerate 15 -r 20 ./fractal.gif -y

gifsicle -O3 ./fractal.gif -o ../fractal.gif

