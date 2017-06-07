FAUSTCC ?= faust

faust: $(BUILD)/Source/FaustPatch.hpp

$(BUILD)/Source/FaustPatch.hpp: $(PATCHSOURCE)/$(FAUST).dsp
	@$(FAUSTCC) -I $(PATCHSOURCE) -i -inpl -a FaustCode/owl.cpp -cn GeneratedPatch $< -o $@

$(BUILD)/Source/%Patch.hpp: $(PATCHSOURCE)/%.dsp
	@$(FAUSTCC) -I $(PATCHSOURCE) -i -inpl -a FaustCode/owl.cpp -cn GeneratedPatch $< -o $@

