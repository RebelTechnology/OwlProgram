LIBSOURCE    = $(BUILDROOT)/LibSource
SOURCE       = $(BUILDROOT)/Source
GENSOURCE    = $(BUILD)/Source

# emscripten
EMCC      ?= emcc
EMCCFLAGS ?= -fno-rtti -fno-exceptions # -std=c++11
EMCCFLAGS += -I$(SOURCE) -I$(PATCHSOURCE) -I$(LIBSOURCE) -I$(GENSOURCE) -I$(BUILD)
EMCCFLAGS += -I$(BUILD)/HeavySource
EMCCFLAGS +=  -ILibraries -ILibraries/KissFFT -DHV_SIMD_NONE
EMCCFLAGS += -Wno-warn-absolute-paths
EMCCFLAGS += -Wno-unknown-warning-option
EMCCFLAGS += -Wno-c++11-extensions
EMCCFLAGS += --memory-init-file 0 # don't create separate memory init file .mem
EMCCFLAGS += -s EXPORTED_FUNCTIONS="['_WEB_setup','_WEB_setParameter','_WEB_processBlock','_WEB_getPatchName','_WEB_getParameterName','_WEB_getMessage','_WEB_getStatus','_WEB_getButtons','_WEB_setButtons']"""
EMCC_SRC   = $(SOURCE)/PatchProgram.cpp $(SOURCE)/PatchProcessor.cpp $(SOURCE)/message.cpp
EMCC_SRC  += WebSource/web.cpp
EMCC_SRC  += $(LIBSOURCE)/basicmaths.c $(LIBSOURCE)/StompBox.cpp $(LIBSOURCE)/FloatArray.cpp $(LIBSOURCE)/ComplexFloatArray.cpp $(LIBSOURCE)/FastFourierTransform.cpp $(LIBSOURCE)/Envelope.cpp $(LIBSOURCE)/VoltsPerOctave.cpp $(LIBSOURCE)/WavetableOscillator.cpp $(LIBSOURCE)/PolyBlepOscillator.cpp
EMCC_SRC  += $(PATCH_CPP_SRC) $(PATCH_C_SRC)
EMCC_SRC  += Libraries/KissFFT/kiss_fft.c
EMCC_SRC  += $(wildcard $(GENSOURCE)/*.c)
EMCC_SRC  += $(wildcard $(GENSOURCE)/*.cpp)
EMCC_SRC  += $(wildcard $(PATCHSOURCE)/*.c)
EMCC_SRC  += $(wildcard $(PATCHSOURCE)/*.cpp)
WEBDIR     = $(BUILD)/web

# JavaScript minifiers
#CLOSURE = java -jar Tools/node_modules/google-closure-compiler/compiler.jar --language_in=ECMASCRIPT5
UGLIFYJS = Tools/node_modules/uglifyjs/bin/uglifyjs

online:
	@echo "$(ONLINE_INCLUDES)" > $(BUILD)/patch.h
	@echo "$(ONLINE_REGISTER)" > $(BUILD)/patch.cpp
	@echo '.string "OnlineCompiler"' > $(BUILD)/progname.s
	@$(MAKE) $(BUILD)/$(TARGET).syx
	@cp $(BUILD)/$(TARGET).syx $(BUILD)/online.syx

$(WEBDIR)/$(TARGET).js: $(EMCC_SRC)
	@mkdir -p $(WEBDIR)
	@$(EMCC) $(EMCCFLAGS) $(EMCC_SRC) -o $(WEBDIR)/$(TARGET).js
	@cp WebSource/*.js WebSource/*.html $(WEBDIR)

$(WEBDIR)/%.min.js: $(WEBDIR)/%.js
	@$(UGLIFYJS) -o $@ $<
#	$(CLOSURE) --js_output_file=$@ $<

web: $(WEBDIR)/$(TARGET).js
minify: $(WEBDIR)/$(TARGET).min.js
