* Updated toolchain to Arm GNU 13.3.rel1, emscripten 6.0.13 and FAUST 2.88.0 (Docker image and CI)
* Heavy patches are built with mainline hvcc 0.17.3 (hvcc -g owl), replacing the Python 2.7 fork
* Added MidiMessage::at() for poly aftertouch
* Added SmoothValue::normalForSampleRate() to rescale lambda between sample rates
* Fixed TapTempo per-sample clock, WavetableOscillator out-of-bounds read and negative-FM phase wrap
* Added LibSource unit tests for 17 classes
* Added GenParameterMap.h: optional map of gen~ parameter names to OWL parameters, with scale and steps
* Added Gen stepped (GEN_SCALE_INT) and exponential (GEN_SCALE_LOG) parameters
* Added Faust [scale:log] exponential widget response
* Added Gen output map (GEN_OUTPUT_MAP) with optional bipolar outputs
* Added genlib spline6_interp
* Fixed Gen parameter defaults set in gen~ units instead of 0..1
* Fixed Faust patches mapping only the first 20 widgets (now 48)
* Fixed PatchMetadata limited to 20 parameters (now 40)
* Fixed Gen output parameters registered twice with OWL_METADATA and GEN_OUTPUT_MAP
* Fixed web build fast pow/exp/log lookup tables left empty
* Faust floorf, ceilf, sqrtf, rintf and roundf use FPU instructions on Cortex-M7

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
