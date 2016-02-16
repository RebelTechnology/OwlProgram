
faust: $(BUILD)/Source/FaustPatch.hpp

$(BUILD)/Source/FaustPatch.hpp: $(PATCHSOURCE)/$(FAUST).dsp
	faust -I $(PATCHSOURCE) -i -inpl -a owl.cpp -cn GeneratedPatch $< -o $@

$(BUILD)/Source/%Patch.hpp: $(PATCHSOURCE)/%.dsp
	faust -I $(PATCHSOURCE) -i -inpl -a owl.cpp -cn GeneratedPatch $< -o $@

