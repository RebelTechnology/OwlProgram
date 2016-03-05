HEAVYFILE   ?= $(HEAVY).pd
HEAVYNAME   ?= owl
HEAVYSRC    ?= $(BUILDROOT)/HeavySource
HEAVYDIR    ?= $(BUILD)/Heavy
ifdef HEAVYTOKEN
HEAVYARGS   = -t $(HEAVYTOKEN)
endif

$(HEAVYDIR)/_main.pd: $(PATCHSOURCE)/$(HEAVYFILE)
	@mkdir -p $(HEAVYDIR)
	@cp -f $(PATCHSOURCE)/*.pd $(HEAVYDIR)
	@cp -f $< $@

$(BUILD)/Source/Heavy_owl.h: $(HEAVYDIR)/_main.pd
	@python2.7 ./Tools/Heavy/uploader.py $(HEAVYDIR) -g c -n $(HEAVYNAME) -o $(BUILD)/Source $(HEAVYARGS)
	@cp $(HEAVYSRC)/HvUtils.h $(BUILD)/Source

heavy: $(BUILD)/Source/Heavy_owl.h
