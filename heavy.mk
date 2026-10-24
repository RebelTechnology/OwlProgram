# v3 heavy.mk — mainline Wasted-Audio hvcc (python3), c2owl generator.
# Replaces the vendored pingdynasty py2.7 fork path. Differences:
#   - calls `hvcc ... -g owl` (python3) instead of `python2.7 Tools/hvcc/hvcc.py`
#   - c2owl emits the OWL wrapper + constants, so build_send_receive_constants.py
#     is no longer invoked
#   - maps mainline c2owl output names onto the names the OWL build #includes
# hvcc is installed on PATH in Dockerfile.v3 (pipx hvcc==0.17.3).

HEAVYFILE    ?= $(HEAVY).pd
HEAVYNAME    ?= owl
HEAVYSRC     ?= $(BUILDROOT)/HeavySource
HEAVYDIR     ?= $(BUILD)/Heavy
HEAVYARGS    ?= -n $(HEAVYNAME) -g owl -o $(BUILD)
HVCC         ?= hvcc

$(HEAVYDIR)/_main.pd: $(PATCHSOURCE)/$(HEAVYFILE)
	@mkdir -p $(HEAVYDIR)
	@cp -f $(PATCHSOURCE)/*.pd $(HEAVYDIR)
	@cp -f $< $@

$(BUILD)/Source/Heavy_owl.h: $(HEAVYDIR)/_main.pd
	@$(HVCC) $(HEAVYDIR)/_main.pd $(HEAVYARGS)
	@# NB: do NOT copy the fork's HeavySource/HvUtils.h + HvMessage.c over the
	@# generated output — mainline hvcc 0.17.3 emits its own newer HvUtils.h
	@# (376 lines, defining the scalar hv_shl_i/hv_rint_f/... that its HvMath.h
	@# needs). The fork's 352-line HvUtils.h lacks them and breaks the build.
	@# Map mainline c2owl output names onto the OWL build's expected names:
	@#   HeavyOwlConstants.h       -> Heavy_owl_constants.h
	@#   HeavyOWL_$(HEAVYNAME).hpp -> HeavyPatch.hpp
	@cp -f $(BUILD)/Source/HeavyOwlConstants.h $(BUILD)/Source/Heavy_$(HEAVYNAME)_constants.h
	@cp -f $(BUILD)/Source/HeavyOWL_$(HEAVYNAME).hpp $(BUILD)/Source/HeavyPatch.hpp

heavy: $(BUILD)/Source/Heavy_owl.h
