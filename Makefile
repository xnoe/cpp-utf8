utf8:
	echo

install:
	mkdir -p $(DESTDIR)/usr/include/
	cp -f utf8.h $(DESTDIR)/usr/include/utf8.h
