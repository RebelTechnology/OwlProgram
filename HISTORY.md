* Updated CMSIS-Core to v6.4.0 (CMSIS 6)
* Updated CMSIS-DSP to v1.18.0
* PatchMetadata holds up to 40 parameters (was 20)
* Added Gen output map (GEN_OUTPUT_MAP in GenParameterMap.h) with optional bipolar outputs
* Added genlib spline6_interp
* Faust patches map up to 48 widgets (was 20)
* Added Gen stepped parameters (GEN_SCALE_INT in GenParameterMap.h)
* Added exponential parameters: Faust [scale:log] widgets and GEN_SCALE_LOG in GenParameterMap.h
* Fixed Gen parameter defaults set in gen~ units instead of 0..1
* Added GenParameterMap.h: optional map of gen~ parameter names to OWL parameters
* Fixed native, web and test builds on macOS
* Added LibSource unit tests for DcBlockingFilter, ExponentialDecayEnvelope, IntArray, basic oscillators, NoiseGenerator, VelocityCurve, EnvelopeFollower, SmoothingFilter, DryWetProcessor, StateVariableFilter, VoltsPerOctave, AdsrEnvelope, SimpleValue and TakeoverValue
* Heavy patches are built with mainline hvcc 0.17.3 (hvcc -g owl), replacing the Python 2.7 fork
* Updated toolchain to Arm GNU 13.3.rel1, emscripten 6.0.13 and FAUST 2.88.0 (Docker image and CI)

v23.1
-----

* Fixed missing SmoothValue and StiffValue::normal definitions
* Fixed SmoothValue<int> to use float lambda
* Fixed Dockerfile hvcc build


v23.0
-----

* Added Welch and Sine window types
* Added WavetableOscillator NO_INTERPOLATION option
* Added ScreenPatch::getScreenWidth() and getScreenHeight()
* Fixed Up/Downsampler
* Updated to CMSIS V5
* Added InterpolatingWavetableOscillator
* Added MovingAverage::process(FloatArray) and ::getAverage()
* Added TakeoverValue
* Added EnvelopeFollower
* Added ExponentialMovingAverage and SimpleMovingAverage
* Added KaiserWindow (from the Loris C++ Class Library)
* Added LinearValue, ExponentialValue and LogarithmicValue
* Refactored Smooth and StiffValue
* Added InterpolatingSignalGenerator

v22.2
-----

* Added PLATFORM=OWL1/2/3 build option
* Added NoiseOscillator (whitenoise s+h)
* Added PhaseShiftOscillator template
* Added Biquad allpass filter configuration
* Fixed Antialiased oscillators (Triangle, SquareWave, Ramp)
* Added MorphingOscillator
* AdjustableTapTempo interface changes
* CircularBuffer improvements, added isFull()
* Added InvertedRampOscillator
* Added StateVariableFilter::setAllpass() and processLowMidHighBand()
* Improved realloc implementation
* Block based SineOscillator::generate() with FM
* Added test patches for automatic CI builds
* Automatically add '>' to end of gen~ output parameters
* Updated to use C++17
* Use std::min/max/abs/clamp instead of macros for C++
* Refactored AdsrEnvelope to Linear and Exponential versions
* Updated CMSIS libraries

v21.2
-----

* Added FloatArray::softclip()
* Added clamp(x, lo, hi) macro
* Added TapTempo
* Added DryWetProcessors
* Added FeedbackProcessors
* Added CrossFadingDelayProcessor
* Added AudioBuffer::copyFrom(), copyTo(), multiply() and add()
* Added Sample oscillator
* Added Agnesi curve oscillator
* Added MPE processor
* Added VOSIM oscillator
* Added methods for sample-rate cutoff frequency modulation in StateVariableFilter and BiquadFilter
* Added VelocityCurve, MidiProcessor, Synth and AbstractSynth
* New templates for MIDI voicing: PolyphonicProcessor and MonophonicProcessor
* Added WavFile for reading wav resources and converting to FloatArray
* Changed enum for BUTTON_A, B, C to BUTTON_1, 2, 3 et c.
* Fix FloatMatrix compilation
* FAUST Checkbutton support
* FAUST Soundfile support
* Added CircularBuffer and DelayProcessor classes
* Added SmoothingFilter and DcBlockingFilter classes
* Improved BiquadFilter and StateVariableFilter
* SimpleArray templated base class
* Updated unit tests
* Improvements to ComplexFloat/ComplexShort classes
* Envelope implements SignalProcessor and SignalGenerator
* NoiseOscillator renamed NoiseGenerator
* Oscillator inherits SignalGenerator
* Window inherits SignalProcessor
* Resampler split into Upsampler and Downsampler SignalProcessors
* ShortArray and IntArray conversion methods renamed toFloat() and fromFloat()
* New SignalGenerator and SignalProcessor abstract base classes
* Refactored makefiles
* Fixed 'test' make target
* Allow patches to be compiled and run natively

v21.1
-----

* SOUL integration
* Maximilian integration
* DaisySP integration
* Added FAUST 'key' MIDI parameter (complements 'freq', 'gain', 'gate' and 'bend')
* Added StateVariableFilter DSP class
* Fixed Bus Error regression on legacy hardware
* Automatically zero-initialise all heap allocations

v21.0
-----

* Matches release v21.0 of OpenWare
* Add support for patch resources

v12.0
-----

* extended parameters: up to 24 parameters controlled by MIDI
* incoming events: handle button presses and MIDI note on/off messages in the patch
* outgoing events: send parameter and button changes as MIDI CC and note on/off messages
* parameter defaults, ranges, smoothing, hysteresis, and non-linearity (exponential and logarithmic curves)

v11.0
-----

* new library classes: AdsrEnvelope, VoltsPerOctave
* updates to web audio processing
* sample accurate button states
* full compatibility with OWL v11 firmware
* improvements to FAUST build

v10.0
-----

OwlProgram release, including OWL Library, to coincide with release v10 of the OWL Firmware.


Prehistory
-----------

A wise old OWL sat in an oak.
