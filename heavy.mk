HEAVYRELEASE ?= r2018.03
# release version, see https://enzienaudio.com/a/releases
HEAVYFILE    ?= $(HEAVY).pd
HEAVYNAME    ?= owl
HEAVYSRC     ?= $(BUILDROOT)/HeavySource
HEAVYDIR     ?= $(BUILD)/Heavy
HEAVYARGS    ?= -g c -n $(HEAVYNAME) -p $(HEAVYDIR) -o $(BUILD)
HVCC         ?= Tools/hvcc/hvcc.py

$(HEAVYDIR)/_main.pd: $(PATCHSOURCE)/$(HEAVYFILE)
	@mkdir -p $(HEAVYDIR)
	@cp -f $(PATCHSOURCE)/*.pd $(HEAVYDIR)
	@cp -f $< $@

$(BUILD)/Source/Heavy_owl.h: $(HEAVYDIR)/_main.pd
	@python2.7 $(HVCC) $(HEAVYDIR)/_main.pd  $(HEAVYARGS)
	@mv -f $(BUILD)/c/* $(BUILD)/Source
	@cp -f $(HEAVYSRC)/HvUtils.h $(HEAVYSRC)/HvMessage.c $(BUILD)/Source

heavy: $(BUILD)/Source/Heavy_owl.h
