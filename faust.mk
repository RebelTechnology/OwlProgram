
faust: $(BUILD)/Source/$(PATCHFILE)

$(BUILD)/Source/%Patch.hpp: $(PATCHSOURCE)/%.dsp
	@faust -I $(PATCHSOURCE) -i -inpl -a owl.cpp -cn $(PATCHNAME)Patch $< -o $@
