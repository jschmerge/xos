%.pdf: %.dot
	dot -Tpdf $< -o $@

all: utf8-input-basic.pdf utf8-input-bom.pdf

clean:
	rm -f *.pdf
