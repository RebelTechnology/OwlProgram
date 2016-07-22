BUILDROOT ?= .

C_SRC   = basicmaths.c
C_SRC   += kiss_fft.c
# CPP_SRC = PatchTest.cpp
CPP_SRC += FloatArray.cpp ComplexFloatArray.cpp FastFourierTransform.cpp 
CPP_SRC += ShortArray.cpp
CPP_SRC += Envelope.cpp VoltsPerOctave.cpp Window.cpp
CPP_SRC += WavetableOscillator.cpp PolyBlepOscillator.cpp
CPP_SRC += SmoothValue.cpp # PatchParameter.cpp


BUILD       ?= $(BUILDROOT)/Build

SOURCE       = $(BUILDROOT)/Source
LIBSOURCE    = $(BUILDROOT)/LibSource
GENSOURCE    = $(BUILD)/Source
TESTPATCHES  = $(BUILDROOT)/TestPatches
CPPFLAGS += -I$(SOURCE)
CPPFLAGS += -I$(LIBSOURCE)
CPPFLAGS += -I$(GENSOURCE)
CPPFLAGS += -I$(TESTPATCHES)
CPPFLAGS += -ILibraries -ILibraries/KissFFT

LDLIBS   = -lm
LDFLAGS  = -Wl,--gc-sections

CXXFLAGS = -std=c++11

# object files
OBJS  = $(C_SRC:%.c=$(BUILD)/%.o) $(CPP_SRC:%.cpp=$(BUILD)/%.o)

# Set up search path
vpath %.cpp $(SOURCE)
vpath %.c $(SOURCE)
vpath %.c Libraries/KissFFT/
vpath %.s $(SOURCE)
vpath %.cpp $(LIBSOURCE)
vpath %.c $(LIBSOURCE)
vpath %.s $(LIBSOURCE)
vpath %.cpp $(GENSOURCE)
vpath %.c $(GENSOURCE)
vpath %.s $(GENSOURCE)

test: $(TESTPATCHES)/PatchTest.cpp $(DEPS) $(OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(TESTPATCHES)/PatchTest.cpp -I$(BUILD) $(OBJS) -o $(BUILD)/$@
	$(BUILD)/$@

# compile and generate dependency info
$(BUILD)/%.o: %.c
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@
	@$(CC) -MM -MT"$@" $(CPPFLAGS) $(CFLAGS) $< > $(@:.o=.d)

$(BUILD)/%.o: %.cpp
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
	@$(CXX) -MM -MT"$@" $(CPPFLAGS) $(CXXFLAGS) $< > $(@:.o=.d)

-include $(OBJS:.o=.d) $(SOLO_OBJS:.o=.d) $(MULTI_OBJS:.o=.d)
