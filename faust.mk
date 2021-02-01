FAUSTCC ?= faust
FAUSTFLAGS ?= -light

faust: $(BUILD)/Source/FaustPatch.hpp

$(BUILD)/Source/FaustPatch.hpp: $(PATCHSOURCE)/$(FAUST).dsp
	@$(FAUSTCC) $(FAUSTFLAGS) -I $(PATCHSOURCE) -i -inpl -mem -a FaustCode/owl.cpp -fm faustmath.h $< $(BUILDROOT)/FaustCode/owl.lib -o $@
	@cp FaustCode/owl.h $(PATCHSOURCE)

$(BUILD)/Source/%Patch.hpp: $(PATCHSOURCE)/%.dsp
	@$(FAUSTCC) $(FAUSTFLAGS) -I $(PATCHSOURCE) -i -inpl -mem -a FaustCode/owl.cpp -fm faustmath.h $< -o $@

