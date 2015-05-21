.PHONY: all clean

all: seminarvortrag.pdf

seminarvortrag.pdf: seminarvortrag.tex
	latexmk -pdf seminarvortrag.tex

clean:
	latexmk -C
	rm -f *.snm *.nav *.vrb
