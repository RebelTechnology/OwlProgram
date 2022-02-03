BUILDROOT   ?= .
BUILD       ?= $(BUILDROOT)/Build
PATCHSOURCE ?= $(BUILDROOT)/PatchSource
FAUSTSOURCE ?= $(BUILDROOT)/FaustSource

FAUSTCC ?= faust
FAUSTFLAGS ?= -light

faust: $(BUILD)/Source/FaustPatch.hpp

$(BUILD)/Source/FaustPatch.hpp: $(PATCHSOURCE)/$(FAUST).dsp
	@$(FAUSTCC) $(FAUSTFLAGS) -I $(PATCHSOURCE) -i -inpl -mem -a FaustSource/owl.cpp -fm faustmath.h $< $(FAUSTSOURCE)/owl.lib -o $@
	@cp $(FAUSTSOURCE)/*.h $(BUILD)/Source

$(BUILD)/Source/%Patch.hpp: $(PATCHSOURCE)/%.dsp
	@$(FAUSTCC) $(FAUSTFLAGS) -I $(PATCHSOURCE) -i -inpl -mem -a $(FAUSTSOURCE)/owl.cpp -fm faustmath.h $< -o $@

