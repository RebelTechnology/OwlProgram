HEAVYFILE    ?= $(HEAVY).pd
HEAVYNAME    ?= owl
HEAVYSRC     ?= $(BUILDROOT)/HeavySource
HEAVYDIR     ?= $(BUILD)/Heavy
HEAVYARGS    ?= -g c -n $(HEAVYNAME) -p $(HEAVYDIR) -o $(BUILD)
HVCC         ?= $(BUILDROOT)/Tools/hvcc/hvcc.py
HVCC_OWL     ?= $(BUILDROOT)/Tools/build_send_receive_constants.py

$(HEAVYDIR)/_main.pd: $(PATCHSOURCE)/$(HEAVYFILE)
	@mkdir -p $(HEAVYDIR)
	@cp -f $(PATCHSOURCE)/*.pd $(HEAVYDIR)
	@cp -f $< $@

$(BUILD)/Source/Heavy_owl.h: $(HEAVYDIR)/_main.pd
	@python2.7 $(HVCC) $(HEAVYDIR)/_main.pd  $(HEAVYARGS)
	@mv -f $(BUILD)/c/* $(BUILD)/Source
	@cp -f $(HEAVYSRC)/HvUtils.h $(HEAVYSRC)/HvMessage.c $(BUILD)/Source
	@python2.7 $(HVCC_OWL) $(BUILD)/ir/owl.heavy.ir.json $(BUILD)/Source/Heavy_$(HEAVYNAME)_constants.h

heavy: $(BUILD)/Source/Heavy_owl.h
