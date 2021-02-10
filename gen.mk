GENSRC     ?= $(BUILDROOT)/GenSource

.PHONY: .FORCE gen

$(BUILD)/Source/gen.h: .FORCE
	@echo "#include \"$(GEN).h\"" > $@
	@echo "namespace gen = $(GEN);" >> $@

gen: $(BUILD)/Source/gen.h
	@cp $(GENSRC)/* $(BUILD)/Source
