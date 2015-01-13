FILES = utf8-input-basic.pdf \
        utf8-input-bom.pdf \
        utf16.pdf

%.pdf: %.dot
	dot -Tpdf $< -o $@

all: $(FILES)

view: $(FILES)
	for f in $(FILES); do atril $$f & done

clean:
	rm -f $(FILES)
