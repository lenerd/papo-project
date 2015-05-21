.PHONY: all clean clean-all

all: seminarvortrag.pdf

seminarvortrag.pdf: seminarvortrag.tex board.png net.png
	latexmk -pdf seminarvortrag.tex

board.png:
	curl http://www.13thmonkey.org/Artdraw/images/kifu2.png -o board.png

net.png:
	curl http://www.texample.net/media/tikz/examples/PNG/neural-network.png -o net.png

clean:
	latexmk -c
	rm -f *.snm *.nav *.vrb

clean-all: clean
	latexmk -C
	rm -f net.png board.png
