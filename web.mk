LIBSOURCE    = $(BUILDROOT)/LibSource
SOURCE       = $(BUILDROOT)/Source
GENSOURCE    = $(BUILD)/Source
DAISYSP      = $(BUILDROOT)/Libraries/DaisySP/Source

# emscripten
EMCC      ?= emcc
EMAR      ?= emar
EMCCFLAGS += -fno-rtti -fno-exceptions
# EMCCFLAGS += -s ASSERTIONS=1 -Wall
EMCCFLAGS += -I$(SOURCE) -I$(PATCHSOURCE) -I$(LIBSOURCE) -I$(GENSOURCE) -I$(BUILD)
EMCCFLAGS += -I$(BUILD)/Source
EMCCFLAGS += -I$(DAISYSP)
EMCCFLAGS += -I$(DAISYSP)/Control
EMCCFLAGS += -I$(DAISYSP)/Drums
EMCCFLAGS += -I$(DAISYSP)/Dynamics
EMCCFLAGS += -I$(DAISYSP)/Effects
EMCCFLAGS += -I$(DAISYSP)/Filters
EMCCFLAGS += -I$(DAISYSP)/Noise
EMCCFLAGS += -I$(DAISYSP)/PhysicalModeling
EMCCFLAGS += -I$(DAISYSP)/Synthesis
EMCCFLAGS += -I$(DAISYSP)/Utility
EMCCFLAGS +=  -ILibraries -ILibraries/KissFFT -DHV_SIMD_NONE -DDSY_CORE_DSP -DDSY_CUSTOM_DSP
EMCCFLAGS += -Wno-warn-absolute-paths
EMCCFLAGS += -Wno-unknown-warning-option
EMCCFLAGS += --memory-init-file 0 # don't create separate memory init file .mem
EMCCFLAGS += -s EXPORTED_FUNCTIONS="['_WEB_setup','_WEB_setParameter','_WEB_getParameter','_WEB_processBlock','_WEB_getPatchName','_WEB_getParameterName','_WEB_getMessage','_WEB_getStatus','_WEB_getButtons','_WEB_setButton', '_WEB_processMidi', '_malloc']"
EMCCFLAGS += -s "EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap']"
CPP_SRC  = $(SOURCE)/PatchProcessor.cpp $(SOURCE)/message.cpp
CPP_SRC += WebSource/web.cpp
CPP_SRC += Resource.cpp
C_SRC   += $(LIBSOURCE)/basicmaths.c
CPP_SRC += $(LIBSOURCE)/Patch.cpp $(LIBSOURCE)/FloatArray.cpp $(LIBSOURCE)/ComplexFloatArray.cpp $(LIBSOURCE)/FastFourierTransform.cpp $(LIBSOURCE)/Envelope.cpp $(LIBSOURCE)/VoltsPerOctave.cpp $(LIBSOURCE)/Window.cpp $(LIBSOURCE)/WavetableOscillator.cpp $(LIBSOURCE)/PolyBlepOscillator.cpp $(LIBSOURCE)/SmoothValue.cpp $(LIBSOURCE)/PatchParameter.cpp
C_SRC   += $(LIBSOURCE)/fastpow.c $(LIBSOURCE)/fastlog.c
C_SRC   += Libraries/KissFFT/kiss_fft.c
CPP_SRC += MonochromeScreenPatch.cpp ColourScreenPatch.cpp
C_SRC += font.c
ifdef MAXIMILIAN
EMCC_SRC := $(filter-out $(PATCHSOURCE)/$(MAXIMILIAN).cpp, $(EMCC_SRC))
endif
WEBDIR   = $(BUILD)/web

EMCCFLAGS += -s WASM=0 # disables wasm output

CPPFLAGS =
CFLAGS   = $(EMCCFLAGS) -std=gnu11
CXXFLAGS = $(EMCCFLAGS) -std=gnu++14
LDFLAGS  = $(EMCCFLAGS)

EMCC_OBJS = $(addprefix $(WEBDIR)/, $(notdir $(CPP_SRC:.cpp=.o)))
EMCC_OBJS += $(addprefix $(WEBDIR)/, $(notdir $(C_SRC:.c=.o)))

EMDAISYSP_CPP_SRC = $(wildcard $(DAISYSP)/*/*.cpp)
EMDAISYSP_OBJS = $(addprefix $(WEBDIR)/, $(notdir $(EMDAISYSP_CPP_SRC:.cpp=.o)))

PATCH_C_SRC   = $(wildcard $(GENSOURCE)/*.c)
PATCH_CPP_SRC = $(wildcard $(GENSOURCE)/*.cpp) $(SOURCE)/PatchProgram.cpp
PATCH_C_SRC   += $(wildcard $(PATCHSOURCE)/*.c)
PATCH_CPP_SRC += $(wildcard $(PATCHSOURCE)/*.cpp)

PATCH_OBJS = $(addprefix $(WEBDIR)/, $(notdir $(PATCH_CPP_SRC:.cpp=.o)))
PATCH_OBJS += $(addprefix $(WEBDIR)/, $(notdir $(PATCH_C_SRC:.c=.o)))


# Set up search path
vpath %.cpp $(SOURCE)
vpath %.c $(SOURCE)
vpath %.s $(SOURCE)
vpath %.cpp $(LIBSOURCE)
vpath %.c $(LIBSOURCE)
vpath %.s $(LIBSOURCE)
vpath %.cpp $(PATCHSOURCE)
vpath %.c $(PATCHSOURCE)
vpath %.s $(PATCHSOURCE)
vpath %.cpp $(GENSOURCE)
vpath %.c $(GENSOURCE)
vpath %.s $(GENSOURCE)
vpath %.cpp $(DAISYSP)
vpath %.cpp $(DAISYSP)/Control
vpath %.cpp $(DAISYSP)/Drums
vpath %.cpp $(DAISYSP)/Dynamics
vpath %.cpp $(DAISYSP)/Effects
vpath %.cpp $(DAISYSP)/Filters
vpath %.cpp $(DAISYSP)/Noise
vpath %.cpp $(DAISYSP)/PhysicalModeling
vpath %.cpp $(DAISYSP)/Synthesis
vpath %.cpp $(DAISYSP)/Utility
vpath %.cpp WebSource
vpath %.c Libraries/KissFFT

ifeq ($(CONFIG),Debug)
EMCCFLAGS += -s ASSERTIONS=1
endif

PHONY: libs web minify

# JavaScript minifiers
#CLOSURE = java -jar Tools/node_modules/google-closure-compiler/compiler.jar --language_in=ECMASCRIPT5
UGLIFYJS = Tools/node_modules/uglifyjs/bin/uglifyjs

$(WEBDIR)/$(TARGET).js: $(PATCH_OBJS)
	$(EMCC) $(LDFLAGS) $(PATCH_OBJS) -o $(WEBDIR)/$(TARGET).js Libraries/libowlweb.a -Wl,--whole-archive Libraries/libdaisyspweb.a -Wl,--no-whole-archive
	@cp WebSource/*.js WebSource/*.html $(WEBDIR)

$(WEBDIR)/%.min.js: $(WEBDIR)/%.js
	@$(UGLIFYJS) -o $@ $<
#	@$(CLOSURE) --js_output_file=$@ $<

Libraries/libowlweb.a: $(EMCC_OBJS)
	$(EMAR) rcs $@ $^

Libraries/libdaisyspweb.a: $(EMDAISYSP_OBJS)
	$(EMAR) rcs $@ $^

libs: Libraries/libowlweb.a Libraries/libdaisyspweb.a

# compile and generate dependency info
$(WEBDIR)/%.o: %.c
	@$(EMCC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
	@$(EMCC) -MM -MT"$@" $(CPPFLAGS) $(CFLAGS) $< > $(@:.o=.d)

$(WEBDIR)/%.o: %.cpp
	@$(EMCC) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	@$(EMCC) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) $< > $(@:.o=.d)

web: $(WEBDIR)/$(TARGET).js ;

minify: $(WEBDIR)/$(TARGET).min.js ;
