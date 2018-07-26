LIBSOURCE    = $(BUILDROOT)/LibSource
SOURCE       = $(BUILDROOT)/Source
GENSOURCE    = $(BUILD)/Source

# emscripten
EMCC      ?= emcc
EMCCFLAGS += -fno-rtti -fno-exceptions
# EMCCFLAGS += -s ASSERTIONS=1 -Wall
EMCCFLAGS += -Dnullptr=NULL
EMCCFLAGS += -I$(SOURCE) -I$(PATCHSOURCE) -I$(LIBSOURCE) -I$(GENSOURCE) -I$(BUILD)
EMCCFLAGS += -I$(BUILD)/Source
EMCCFLAGS +=  -ILibraries -ILibraries/KissFFT -DHV_SIMD_NONE
EMCCFLAGS += -Wno-warn-absolute-paths
EMCCFLAGS += -Wno-unknown-warning-option
EMCCFLAGS += -Wno-c++11-extensions
EMCCFLAGS += --memory-init-file 0 # don't create separate memory init file .mem
EMCCFLAGS += -s EXPORTED_FUNCTIONS="['_WEB_setup','_WEB_setParameter','_WEB_processBlock','_WEB_getPatchName','_WEB_getParameterName','_WEB_getMessage','_WEB_getStatus','_WEB_getButtons','_WEB_setButtons', '_WEB_processMidi']"
EMCCFLAGS += -s "EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap']"
CPP_SRC  = $(SOURCE)/PatchProgram.cpp $(SOURCE)/PatchProcessor.cpp $(SOURCE)/message.cpp
CPP_SRC += WebSource/web.cpp
C_SRC   += $(LIBSOURCE)/basicmaths.c
CPP_SRC += $(LIBSOURCE)/Patch.cpp $(LIBSOURCE)/FloatArray.cpp $(LIBSOURCE)/ComplexFloatArray.cpp $(LIBSOURCE)/FastFourierTransform.cpp $(LIBSOURCE)/Envelope.cpp $(LIBSOURCE)/VoltsPerOctave.cpp $(LIBSOURCE)/Window.cpp $(LIBSOURCE)/WavetableOscillator.cpp $(LIBSOURCE)/PolyBlepOscillator.cpp $(LIBSOURCE)/SmoothValue.cpp
# C_SRC  += $(LIBSOURCE)/fastpow.c $(LIBSOURCE)/fastlog.c $(LIBSOURCE)/system_tables.cpp
C_SRC   += Libraries/KissFFT/kiss_fft.c
C_SRC   += $(wildcard $(GENSOURCE)/*.c)
CPP_SRC += $(wildcard $(GENSOURCE)/*.cpp)
C_SRC   += $(wildcard $(PATCHSOURCE)/*.c)
CPP_SRC += $(wildcard $(PATCHSOURCE)/*.cpp)
WEBDIR   = $(BUILD)/web

EMCCFLAGS += -s WASM=0

CPPFLAGS =
CFLAGS = $(EMCCFLAGS)
CXXFLAGS = $(EMCCFLAGS) -std=gnu++11
LDFLAGS = $(EMCCFLAGS)

EMCC_OBJS = $(addprefix $(WEBDIR)/, $(notdir $(CPP_SRC:.cpp=.o)))
EMCC_OBJS += $(addprefix $(WEBDIR)/, $(notdir $(C_SRC:.c=.o)))

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
vpath %.cpp WebSource
vpath %.c Libraries/KissFFT

ifeq ($(CONFIG),Debug)
EMCCFLAGS += -s ASSERTIONS=1
endif

# JavaScript minifiers
#CLOSURE = java -jar Tools/node_modules/google-closure-compiler/compiler.jar --language_in=ECMASCRIPT5
UGLIFYJS = Tools/node_modules/uglifyjs/bin/uglifyjs

$(WEBDIR)/$(TARGET).js: $(EMCC_OBJS)
	@$(EMCC) $(LDFLAGS) $(EMCC_OBJS) -o $(WEBDIR)/$(TARGET).js
	@cp WebSource/*.js WebSource/*.html $(WEBDIR)

$(WEBDIR)/%.min.js: $(WEBDIR)/%.js
	@$(UGLIFYJS) -o $@ $<
#	@$(CLOSURE) --js_output_file=$@ $<


# compile and generate dependency info
$(WEBDIR)/%.o: %.c
	@$(EMCC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
	@$(EMCC) -MM -MT"$@" $(CPPFLAGS) $(CFLAGS) $< > $(@:.o=.d)

$(WEBDIR)/%.o: %.cpp
	@$(EMCC) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	@$(EMCC) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) $< > $(@:.o=.d)

web: $(WEBDIR)/$(TARGET).js ;

minify: $(WEBDIR)/$(TARGET).min.js ;
