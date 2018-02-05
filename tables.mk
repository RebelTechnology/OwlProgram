BUILDROOT ?= .

POW_PRECISION = 6
LOG_PRECISION = 8

tables: $(BUILDROOT)/LibSource/FastPowTable.h $(BUILDROOT)/LibSource/FastLogTable.h

$(BUILDROOT)/Tools/mkpow: LibSource/fastpow.h LibSource/fastpow.c Tools/MakePowTable.c
	$(CC) $? -o $@ -lm

$(BUILDROOT)/LibSource/FastPowTable.h: $(BUILDROOT)/Tools/mkpow
	$(BUILDROOT)/Tools/mkpow $(POW_PRECISION) > $@

$(BUILDROOT)/Tools/mklog: LibSource/fastlog.h LibSource/fastlog.c Tools/MakeLogTable.c
	$(CC) $? -o $@ -lm

$(BUILDROOT)/LibSource/FastLogTable.h: $(BUILDROOT)/Tools/mklog
	$(BUILDROOT)/Tools/mklog $(LOG_PRECISION) > $@

clean:
	rm -f $(BUILDROOT)/Tools/mkpow $(BUILDROOT)/Tools/mklog $(BUILDROOT)/LibSource/FastPowTable.h $(BUILDROOT)/LibSource/FastLogTable.h
