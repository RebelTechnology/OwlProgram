SOULSRC ?= $(BUILDROOT)/SoulSource
SOULCC	?= Tools/soul
SOULINC = $(BUILD)/Source/soul.hpp

vpath %.soul $(PATCHSOURCE)

.PHONY: .FORCE soul

$(SOULINC): .FORCE
	@echo "#include \"$(SOULHPP)\"" > $@
	@echo "#define SOULPATCH $(SOULCLASS)" >> $@

$(BUILD)/Source/$(SOULHPP): $(SOULFILE)
	@$(SOULCC) generate --cpp $< --output=$@

soul: $(SOULINC) $(BUILD)/Source/$(SOULHPP)
	@cp $(SOULSRC)/* $(BUILD)/Source
