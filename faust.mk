BUILDROOT   ?= .
BUILD       ?= $(BUILDROOT)/Build
PATCHSOURCE ?= $(BUILDROOT)/PatchSource
FAUSTSOURCE ?= $(BUILDROOT)/FaustSource

FAUSTCC ?= faust
FAUSTFLAGS ?= -mem -light -scal -exp10 -mcd 0

faust: $(BUILD)/Source/FaustPatch.hpp

$(BUILD)/Source/FaustPatch.hpp: $(PATCHSOURCE)/$(FAUST).dsp
	@$(FAUSTCC) $(FAUSTFLAGS) -I $(PATCHSOURCE) -i -inpl -a FaustSource/owl.cpp -fm faustmath.h $< $(FAUSTSOURCE)/owl.lib -o $@
	@cp $(FAUSTSOURCE)/*.h $(BUILD)/Source

$(BUILD)/Source/%Patch.hpp: $(PATCHSOURCE)/%.dsp
	@$(FAUSTCC) $(FAUSTFLAGS) -I $(PATCHSOURCE) -i -inpl -a $(FAUSTSOURCE)/owl.cpp -fm faustmath.h $< -o $@

