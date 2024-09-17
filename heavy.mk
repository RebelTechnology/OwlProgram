HEAVYFILE    ?= $(HEAVY).pd
HEAVYNAME    ?= owl
HEAVYDIR     ?= $(BUILD)/Heavy
HEAVYARGS    ?= -g owl -n $(HEAVYNAME) -p $(HEAVYDIR) -o $(BUILD)
HVCC         ?= hvcc

$(HEAVYDIR)/_main.pd: $(PATCHSOURCE)/$(HEAVYFILE)
	@mkdir -p $(HEAVYDIR)
	@cp -f $(PATCHSOURCE)/*.pd $(HEAVYDIR)
	@cp -f $< $@

$(BUILD)/Source/Heavy_owl.h: $(HEAVYDIR)/_main.pd
	$(HVCC) $(HEAVYDIR)/_main.pd  $(HEAVYARGS)

heavy: $(BUILD)/Source/Heavy_owl.h
