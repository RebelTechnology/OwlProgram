FAUSTCC ?= faust
FAUSTFLAGS ?= -light
FAUSTCODE = FaustCode

faust: $(BUILD)/Source/FaustPatch.hpp

$(BUILD)/Source/FaustPatch.hpp: $(PATCHSOURCE)/$(FAUST).dsp
	@$(FAUSTCC) $(FAUSTFLAGS) -I $(PATCHSOURCE) -i -inpl -mem -a FaustCode/owl.cpp -fm faustmath.h $< $(BUILDROOT)/FaustCode/owl.lib -o $@
	@cp $(FAUSTCODE)/*.h $(BUILD)/Source/

$(BUILD)/Source/%Patch.hpp: $(PATCHSOURCE)/%.dsp
	@$(FAUSTCC) $(FAUSTFLAGS) -I $(PATCHSOURCE) -i -inpl -mem -a FaustCode/owl.cpp -fm faustmath.h $< -o $@

