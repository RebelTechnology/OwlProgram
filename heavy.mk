HEAVYFILE    ?= $(HEAVY).pd
HEAVYNAME    ?= owl
HEAVYSRC     ?= $(BUILDROOT)/HeavySource
HEAVYDIR     ?= $(BUILD)/Heavy
HEAVYARGS    ?= -g owl -n $(HEAVYNAME) -p $(HEAVYDIR) -o $(BUILD)
HVCC         ?= hvcc
HVCC_OWL     ?= $(BUILDROOT)/Tools/build_send_receive_constants.py

$(HEAVYDIR)/_main.pd: $(PATCHSOURCE)/$(HEAVYFILE)
	@mkdir -p $(HEAVYDIR)
	@cp -f $(PATCHSOURCE)/*.pd $(HEAVYDIR)
	@cp -f $< $@

$(BUILD)/Source/Heavy_owl.h: $(HEAVYDIR)/_main.pd
	$(HVCC) $(HEAVYDIR)/_main.pd  $(HEAVYARGS)
	@mv -f $(BUILD)/owl/source/* $(BUILD)/Source

heavy: $(BUILD)/Source/Heavy_owl.h
