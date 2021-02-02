MAXIMILIANSRC     ?= $(BUILDROOT)/Maximilian

.PHONY: .FORCE

$(BUILD)/Source/maximilian-patch.h: .FORCE
	@echo "#include \"$(MAXIMILIAN).cpp\"" > $@
#	@echo "namespace gen = $(GEN);" >> $@

maximilian: $(BUILD)/Source/maximilian-patch.h
	@cp $(MAXIMILIANSRC)/maximilian.h $(MAXIMILIANSRC)/maximilian.cpp $(MAXIMILIANSRC)/MaximilianPatch.hpp $(BUILD)/Source
