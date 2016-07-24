HEAVYRELEASE ?= r2016.07.05 
# release version, see https://enzienaudio.com/a/releases
HEAVYFILE    ?= $(HEAVY).pd
HEAVYNAME    ?= owl
HEAVYSRC     ?= $(BUILDROOT)/HeavySource
HEAVYDIR     ?= $(BUILD)/Heavy
HEAVYARGS    ?= -r $(HEAVYRELEASE)
ifdef HEAVYTOKEN
HEAVYARGS    += -t $(HEAVYTOKEN)
endif

$(HEAVYDIR)/_main.pd: $(PATCHSOURCE)/$(HEAVYFILE)
	@mkdir -p $(HEAVYDIR)
	@cp -f $(PATCHSOURCE)/*.pd $(HEAVYDIR)
	@cp -f $< $@

$(BUILD)/Source/Heavy_owl.h: $(HEAVYDIR)/_main.pd
	@python2.7 ./Tools/Heavy/uploader.py $(HEAVYDIR) -g c -n $(HEAVYNAME) -o $(BUILD)/Source $(HEAVYARGS)
	@cp $(HEAVYSRC)/HvUtils.h $(HEAVYSRC)/HvMessage.c $(BUILD)/Source

heavy: $(BUILD)/Source/Heavy_owl.h
