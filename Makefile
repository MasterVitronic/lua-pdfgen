LUA     = 5.4
PREFIX  = /usr/local
LIBDIR  = $(PREFIX)/lib/lua/$(LUA)

LUA_CFLAGS  = $(shell pkg-config --cflags lua$(LUA))
CFLAGS  = -fPIC $(LUA_CFLAGS) -I/usr/include/
LIBS    = $(shell pkg-config --libs lua$(LUA))

pdfgen.so: lua-pdfgen.o
	$(CC) -shared $(CFLAGS) -o $@ lua-pdfgen.o pdfgen.c $(LIBS) -lm

install:
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp pdfgen.so $(DESTDIR)$(LIBDIR)

docs: pdfgen.so docs/config.ld
	ldoc -c docs/config.ld -d html -a .

clean:
	rm -rf *.o *.so *.pdf

.PHONY: pdfgen.so
