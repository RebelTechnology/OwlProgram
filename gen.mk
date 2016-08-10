GENSRC     ?= $(BUILDROOT)/GenSource
GENDIR     ?= $(BUILD)/Gen
GENARGS    ?= -r $(GENRELEASE)
ifdef GENTOKEN
GENARGS    += -t $(GENTOKEN)
endif

# $(GENDIR)/_main.pd: $(PATCHSOURCE)/$(GENFILE)
# 	@mkdir -p $(GENDIR)
# 	@cp -f $(PATCHSOURCE)/*.pd $(GENDIR)
# 	@cp -f $< $@

# $(BUILD)/Source/Gen_owl.h: $(GENDIR)/_main.pd
# 	@python2.7 ./Tools/Gen/uploader.py $(GENDIR) -g c -n $(GENNAME) -o $(BUILD)/Source $(GENARGS)
# 	@cp $(GENSRC)/HvUtils.h $(GENSRC)/HvMessage.c $(BUILD)/Source

# gen: $(BUILD)/Source/Gen_owl.h
