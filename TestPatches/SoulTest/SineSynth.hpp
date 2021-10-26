//      _____ _____ _____ __
//     |   __|     |  |  |  |        Auto-generated C++
//     |__   |  |  |  |  |  |__      SOUL Version 0.9.0
//     |_____|_____|_____|_____|     https://soul.dev
//

#include <array>
#include <functional>
#include <cmath>
#include <cstddef>
#include <limits>
#include <cstring>

#ifndef SOUL_CPP_ASSERT
 #define SOUL_CPP_ASSERT(x)
#endif

// If you need to provide custom implementations of the instrinsics that soul uses,
// you can set this macro to provide your own namespace containing them.
#ifndef SOUL_INTRINSICS
 #define SOUL_INTRINSICS std
#endif

//==============================================================================
// Generated from graph 'SineSynth', source file: SineSynth.soul
//
class SineSynth
{
public:
    SineSynth() = default;
    ~SineSynth() = default;

    //==============================================================================
    template <typename Type, int32_t size> struct Vector;
    template <typename Type, int32_t size> struct FixedArray;
    template <typename Type> struct DynamicArray;

    static constexpr uint32_t maxBlockSize  = 1024;
    static constexpr uint32_t latency       = 0;

    template <typename Item>
    struct span
    {
        Item* start = nullptr;
        size_t numItems = 0;

        constexpr size_t size() const               { return numItems; }
        constexpr bool empty() const                { return numItems == 0; }
        constexpr Item* begin() const               { return start; }
        constexpr Item* end() const                 { return start + numItems; }
        const Item& operator[] (size_t index) const { SOUL_CPP_ASSERT (index < numItems); return start[index]; }
    };

    struct _RenderStats;
    struct _SparseStreamStatus;
    struct _Event_in_f32_1;
    struct _Event_in_struct_Message_1;
    struct _Stream_out_f32_1024;
    struct soul__midi__MPEParser___State;
    struct soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator__VoiceInfo;
    struct soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State;
    struct SineOsc___State;
    struct soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___State;
    struct soul__gain__DynamicGain___for__root__Voice_attenuator___State;
    struct Voice___State;
    struct soul__gain__DynamicGain___for__root__SineSynth_gainProcessor___State;
    struct soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___State;
    struct _State;
    struct soul__midi__Message;
    struct soul__note_events__NoteOn;
    struct soul__note_events__NoteOff;
    struct soul__note_events__PitchBend;
    struct soul__note_events__Pressure;
    struct soul__note_events__Slide;
    struct soul__note_events__Control;
    struct soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___IO;
    struct soul__gain__DynamicGain___for__root__SineSynth_gainProcessor___IO;
    struct soul__gain__DynamicGain___for__root__Voice_attenuator___IO;
    struct soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___IO;
    struct SineOsc___IO;
    struct Voice___IO;
    struct StringLiteral;

    //==============================================================================
    // The following methods provide basic initialisation and control for the processor

    void init (double newSampleRate, int sessionID)
    {
        memset (reinterpret_cast<void*> (std::addressof (state)), 0, sizeof (state));
        sampleRate = newSampleRate;
        _initialise (state, sessionID);
        initialisedState = state;
    }

    void reset() noexcept
    {
        state = initialisedState;
    }

    uint32_t getNumXRuns() noexcept
    {
        return static_cast<uint32_t> (_get_num_xruns (state));
    }

    //==============================================================================
    // These classes and functions provide a high-level rendering method that
    // presents the processor as a set of standard audio and MIDI channels.

    static constexpr uint32_t numAudioInputChannels  = 0;
    static constexpr uint32_t numAudioOutputChannels = 1;

    struct MIDIMessage
    {
        uint32_t frameIndex = 0;
        uint8_t byte0 = 0, byte1 = 0, byte2 = 0;
    };

    struct MIDIMessageArray
    {
        const MIDIMessage* messages = nullptr;
        uint32_t numMessages = 0;
    };

    template <typename FloatType = float>
    struct RenderContext
    {
        std::array<const FloatType*, 0> inputChannels;
        std::array<FloatType*, 1> outputChannels;
        MIDIMessageArray  incomingMIDI;
        uint32_t          numFrames = 0;
    };

    //==============================================================================
    template <typename FloatType>
    void render (RenderContext<FloatType> context)
    {
        uint32_t startFrame = 0, startMIDIIndex = 0;

        while (startFrame < context.numFrames)
        {
            auto framesRemaining = context.numFrames - startFrame;
            auto numFramesToDo = framesRemaining < maxBlockSize ? framesRemaining : maxBlockSize;
            auto endMIDIIndex = startMIDIIndex;

            while (endMIDIIndex < context.incomingMIDI.numMessages)
            {
                auto eventTime = context.incomingMIDI.messages[endMIDIIndex].frameIndex;

                if (eventTime > startFrame)
                {
                    auto framesUntilEvent = eventTime - startFrame;

                    if (framesUntilEvent < numFramesToDo)
                        numFramesToDo = framesUntilEvent;

                    break;
                }

                ++endMIDIIndex;
            }

            prepare (numFramesToDo);

            while (startMIDIIndex < endMIDIIndex)
            {
                auto midi = context.incomingMIDI.messages[startMIDIIndex++];
                auto packed = (static_cast<uint32_t> (midi.byte0) << 16) | (static_cast<uint32_t> (midi.byte1) << 8) | static_cast<uint32_t> (midi.byte2);
                _addInputEvent_midiIn_struct_Message (state, { static_cast<int32_t> (packed) });
            }

            advance();

            copyFromInterleaved (&context.outputChannels[0], startFrame, _getOutputFrameArrayRef_audioOut (state).elements, numFramesToDo);
            startFrame += numFramesToDo;
        }
    }

    //==============================================================================
    // The following methods provide low-level access for read/write to all the
    // endpoints directly, and to run the prepare/advance loop.

    void prepare (uint32_t numFramesToBeRendered)
    {
        SOUL_CPP_ASSERT (numFramesToBeRendered != 0);
        framesToAdvance = numFramesToBeRendered;
        _prepare (state, static_cast<int32_t> (numFramesToBeRendered));
    }

    void advance()
    {
        SOUL_CPP_ASSERT (framesToAdvance != 0); // you must call prepare() before advance()!
        auto framesRemaining = framesToAdvance;

        while (framesRemaining > 0)
        {
            auto framesThisCall = framesRemaining < maxBlockSize ? framesRemaining : maxBlockSize;

            run (state, static_cast<int32_t> (framesThisCall));

            totalFramesElapsed += framesThisCall;
            framesRemaining -= framesThisCall;
        }
    }

    void addInputEvent_volume (float eventValue)
    {
        _addInputEvent_volume_f32 (state, eventValue);
    }

    void addInputEvent_midiIn (const soul__midi__Message& eventValue)
    {
        _addInputEvent_midiIn_struct_Message (state, eventValue);
    }

    DynamicArray<const float> getOutputStreamFrames_audioOut()
    {
        return { &(_getOutputFrameArrayRef_audioOut (state).elements[0]), static_cast<int32_t> (framesToAdvance) };
    }

    //==============================================================================
    // The following methods provide a fixed interface for finding out about
    // the input/output endpoints that this processor provides.

    using EndpointID = const char*;
    enum class EndpointType     { value, stream, event };

    struct EndpointDetails
    {
        const char* name;
        EndpointID endpointID;
        EndpointType endpointType;
        const char* frameType;
        uint32_t numAudioChannels;
        const char* annotation;
    };

    std::array<EndpointDetails, 2> getInputEndpoints() const
    {
        return
        {
            EndpointDetails { "volume", "in:volume", EndpointType::event, "float32",                     0, "{ \"name\": \"Volume\", \"min\": -40, \"max\": 0, \"init\": -6, \"step\": 1, \"unit\": \"dB\" }" },
            EndpointDetails { "midiIn", "in:midiIn", EndpointType::event, "Message { int32 midiBytes }", 0, ""                                                                                                }
        };
    }

    std::array<EndpointDetails, 1> getOutputEndpoints() const
    {
        return
        {
            EndpointDetails { "audioOut", "out:audioOut", EndpointType::stream, "float32", 1, "" }
        };
    }

    //==============================================================================
    // The following methods provide help in dealing with the processor's endpoints
    // in a format suitable for traditional audio plugin channels and parameters.

    struct ParameterProperties
    {
        const char* UID;
        const char* name;
        const char* unit;
        float minValue, maxValue, step, initialValue;
        bool isAutomatable, isBoolean, isHidden;
        const char* group;
        const char* textValues;
    };

    struct Parameter
    {
        ParameterProperties properties;
        float currentValue;
        std::function<void(float)> applyValue;

        void setValue (float f)
        {
            currentValue = snapToLegalValue (f);
            applyValue (f);
        }

        float getValue() const
        {
            return currentValue;
        }

    private:
        float snapToLegalValue (float v) const
        {
            if (properties.step > 0)
                v = properties.minValue + properties.step * SOUL_INTRINSICS::floor ((v - properties.minValue) / properties.step + 0.5f);

            return v < properties.minValue ? properties.minValue : (v > properties.maxValue ? properties.maxValue : v);
        }
    };

    struct AudioBus
    {
        const char* name;
        uint32_t numChannels;
    };

    static constexpr bool      hasMIDIInput = true;
    static constexpr uint32_t  numParameters = 1;

    static constexpr const std::array<const ParameterProperties, numParameters> parameters =
    {
        ParameterProperties {  "volume",  "Volume",  "dB",  -40.0f,  0.0f,  1.0f,  -6.0f,  true,  false,  false,  "",  ""  }
    };

    static span<const ParameterProperties> getParameterProperties() { return { parameters.data(), numParameters }; }

    static constexpr uint32_t numInputBuses  = 0;
    static constexpr uint32_t numOutputBuses = 1;

    static constexpr std::array<const AudioBus, numOutputBuses> outputBuses = { AudioBus { "audioOut", 1 } };

    static span<const AudioBus> getInputBuses()  { return {}; }
    static span<const AudioBus> getOutputBuses() { return { outputBuses.data(), numOutputBuses }; }

    struct ParameterList
    {
        Parameter* begin()                      { return params; }
        Parameter* end()                        { return params + numParameters; }
        size_t size() const                     { return numParameters; }
        Parameter& operator[] (size_t index)    { SOUL_CPP_ASSERT (index < numParameters); return params[index]; }

        Parameter params[numParameters == 0 ? 1 : numParameters];
    };

    ParameterList createParameterList()
    {
        return
        {
            {
                Parameter {  parameters[0],  -6.0f,  [this] (float v) { addInputEvent_volume (v); }  }
            }
        };
    }

    static constexpr bool hasTimelineEndpoints = false;

    void setTimeSignature (int32_t newNumerator, int32_t newDenominator)
    {
        (void) newNumerator; (void) newDenominator;
    }

    void setTempo (float newBPM)
    {
        (void) newBPM;
    }

    void setTransportState (int32_t newState)
    {
        (void) newState;
    }

    void setPosition (int64_t currentFrame, double currentQuarterNote, double lastBarStartQuarterNote)
    {
        (void) currentFrame; (void) currentQuarterNote; (void) lastBarStartQuarterNote;
    }

    //==============================================================================
    struct ZeroInitialiser
    {
        template <typename Type>   operator Type() const   { return {}; }
        template <typename Index>  ZeroInitialiser operator[] (Index) const { return {}; }
    };

    //==============================================================================
    template <typename Type>
    struct DynamicArray
    {
        using ElementType = Type;
        ElementType* elements = nullptr;
        int32_t numElements = 0;

        constexpr ElementType& operator[] (int i) noexcept                   { return elements[i]; }
        constexpr const ElementType& operator[] (int i) const noexcept       { return elements[i]; }
        constexpr int getElementSizeBytes() const noexcept                   { return sizeof (ElementType); }
    };

    //==============================================================================
    template <typename Type, int32_t size>
    struct FixedArray
    {
        using ElementType = Type;
        ElementType elements[size];
        static constexpr int32_t numElements = size;

        static constexpr FixedArray fromRepeatedValue (ElementType value)
        {
            FixedArray a;

            for (auto& element : a.elements)
                element = value;

            return a;
        }

        static size_t elementOffset (int i) noexcept               { return sizeof (ElementType) * i; }
        ElementType& operator[] (int i) noexcept                   { return elements[i]; }
        const ElementType& operator[] (int i) const noexcept       { return elements[i]; }
        int getElementSizeBytes() const noexcept                   { return sizeof (ElementType); }
        DynamicArray<ElementType> toDynamicArray() const noexcept  { return { const_cast<ElementType*> (&elements[0]), size }; }
        operator ElementType*() const noexcept                     { return const_cast<ElementType*> (&elements[0]); }

        FixedArray& operator= (ZeroInitialiser)
        {
            for (auto& e : elements)
                e = ElementType {};

            return *this;
        }

        template <int start, int end>
        constexpr FixedArray<Type, end - start> slice() const noexcept
        {
            FixedArray<Type, end - start> newSlice;

            for (int i = 0; i < end - start; ++i)
                newSlice.elements[i] = elements[start + i];

            return newSlice;
        }
    };

    //==============================================================================
    template <typename Type, int32_t size>
    struct Vector
    {
        using ElementType = Type;
        ElementType elements[size] = {};
        static constexpr int32_t numElements = size;

        constexpr Vector() = default;
        constexpr Vector (const Vector&) = default;
        constexpr Vector& operator= (const Vector&) = default;

        explicit constexpr Vector (Type value)
        {
            for (auto& element : elements)
                element = value;
        }

        template <typename OtherType>
        constexpr Vector (const Vector<OtherType, size>& other)
        {
            for (int32_t i = 0; i < size; ++i)
                elements[i] = static_cast<Type> (other.elements[i]);
        }

        constexpr Vector (std::initializer_list<Type> i)
        {
            int n = 0;

            for (auto e : i)
                elements[n++] = e;
        }

        static constexpr Vector fromRepeatedValue (Type value)
        {
            return Vector (value);
        }

        constexpr Vector operator+ (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a + b; }); }
        constexpr Vector operator- (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a - b; }); }
        constexpr Vector operator* (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a * b; }); }
        constexpr Vector operator/ (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a / b; }); }
        constexpr Vector operator% (const Vector& rhs) const                { return apply<Vector> (rhs, [] (Type a, Type b) { return a % b; }); }
        constexpr Vector operator-() const                                  { return apply<Vector> ([] (Type n) { return -n; }); }
        constexpr Vector operator~() const                                  { return apply<Vector> ([] (Type n) { return ~n; }); }
        constexpr Vector operator!() const                                  { return apply<Vector> ([] (Type n) { return ! n; }); }

        Vector& operator= (ZeroInitialiser)
        {
            for (auto& e : elements)
                e = {};

            return *this;
        }

        constexpr Vector<bool, size> operator== (const Vector& rhs) const   { return apply<Vector<bool, size>> (rhs, [] (Type a, Type b) { return a == b; }); }
        constexpr Vector<bool, size> operator!= (const Vector& rhs) const   { return apply<Vector<bool, size>> (rhs, [] (Type a, Type b) { return a != b; }); }

        template <typename ReturnType, typename Op>
        constexpr ReturnType apply (const Vector& rhs, Op&& op) const noexcept
        {
            ReturnType v;

            for (int i = 0; i < size; ++i)
                v.elements[i] = op (elements[i], rhs.elements[i]);

            return v;
        }

        template <typename ReturnType, typename Op>
        constexpr ReturnType apply (Op&& op) const noexcept
        {
            ReturnType v;

            for (int i = 0; i < size; ++i)
                v.elements[i] = op (elements[i]);

            return v;
        }

        template <int start, int end>
        constexpr Vector<Type, end - start> slice() const noexcept
        {
            Vector<Type, end - start> newSlice;

            for (int i = 0; i < end - start; ++i)
                newSlice.elements[i] = elements[start + i];

            return newSlice;
        }

        constexpr const Type& operator[] (int i) const noexcept  { return elements[i]; }
        constexpr Type& operator[] (int i) noexcept              { return elements[i]; }
    };

    //==============================================================================
    struct StringLiteral
    {
        constexpr StringLiteral (int32_t h) noexcept : handle (h) {}
        StringLiteral() = default;
        StringLiteral (const StringLiteral&) = default;
        StringLiteral& operator= (const StringLiteral&) = default;

        const char* toString() const       { return lookupStringLiteral (handle); }
        operator const char*() const       { return lookupStringLiteral (handle); }

        bool operator== (StringLiteral other) const noexcept    { return handle == other.handle; }
        bool operator!= (StringLiteral other) const noexcept    { return handle != other.handle; }

        int32_t handle = 0;
    };


    //==============================================================================
    //==============================================================================
    //
    //    All the code that follows this point should be considered internal.
    //    User code should rarely need to refer to anything beyond this point..
    //
    //==============================================================================
    //==============================================================================

    template <typename Vec>  static Vec _vec_sqrt  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::sqrt (x); }); }
    template <typename Vec>  static Vec _vec_pow   (Vec a, Vec b)  { return a.template apply<Vec> ([&] (typename Vec::ElementType x) { return SOUL_INTRINSICS::pow (x, b); }); }
    template <typename Vec>  static Vec _vec_exp   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::exp (x); }); }
    template <typename Vec>  static Vec _vec_log   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::log (x); }); }
    template <typename Vec>  static Vec _vec_log10 (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::log10 (x); }); }
    template <typename Vec>  static Vec _vec_sin   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::sin (x); }); }
    template <typename Vec>  static Vec _vec_cos   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::cos (x); }); }
    template <typename Vec>  static Vec _vec_tan   (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::tan (x); }); }
    template <typename Vec>  static Vec _vec_sinh  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::sinh (x); }); }
    template <typename Vec>  static Vec _vec_cosh  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::cosh (x); }); }
    template <typename Vec>  static Vec _vec_tanh  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::tanh (x); }); }
    template <typename Vec>  static Vec _vec_asinh (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::asinh (x); }); }
    template <typename Vec>  static Vec _vec_acosh (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::acosh (x); }); }
    template <typename Vec>  static Vec _vec_atanh (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::atanh (x); }); }
    template <typename Vec>  static Vec _vec_asin  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::asin (x); }); }
    template <typename Vec>  static Vec _vec_acos  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::acos (x); }); }
    template <typename Vec>  static Vec _vec_atan  (Vec a)         { return a.template apply<Vec> ([]  (typename Vec::ElementType x) { return SOUL_INTRINSICS::atan (x); }); }
    template <typename Vec>  static Vec _vec_atan2 (Vec a, Vec b)  { return a.template apply<Vec> ([&] (typename Vec::ElementType x) { return SOUL_INTRINSICS::atan2 (x, b); }); }

    static constexpr int32_t _intrin_clamp (int32_t n, int32_t low, int32_t high)  { return n < low ? low : (n > high ? high : n); }
    static constexpr int32_t _intrin_wrap (int32_t n, int32_t range)   { if (range == 0) return 0; auto x = n % range; return x < 0 ? x + range : x; }

    static constexpr float  _nan32 = std::numeric_limits<float>::quiet_NaN();
    static constexpr double _nan64 = std::numeric_limits<double>::quiet_NaN();

    static constexpr float  _inf32 = std::numeric_limits<float>::infinity();
    static constexpr double _inf64 = std::numeric_limits<double>::infinity();

    static constexpr float  _ninf32 = -_inf32;
    static constexpr double _ninf64 = -_inf64;

    template <typename SourceFloatType, typename DestFloatType>
    static inline void copyToInterleaved (DestFloatType* monoDest, const SourceFloatType* const* sourceChannels, uint32_t sourceStartFrame, uint32_t numFrames)
    {
        auto source = *sourceChannels + sourceStartFrame;

        for (uint32_t i = 0; i < numFrames; ++i)
            monoDest[i] = static_cast<DestFloatType> (source[i]);
    }

    template <typename SourceFloatType, typename DestFloatType, int32_t numChannels>
    static inline void copyToInterleaved (Vector<DestFloatType, numChannels>* vectorDest, const SourceFloatType* const* sourceChannels, uint32_t sourceStartFrame, uint32_t numFrames)
    {
        for (uint32_t i = 0; i < numFrames; ++i)
            for (uint32_t chan = 0; chan < static_cast<uint32_t> (numChannels); ++chan)
                vectorDest[i].elements[chan] = static_cast<DestFloatType> (sourceChannels[chan][sourceStartFrame + i]);
    }

    template <typename SourceFloatType, typename DestFloatType>
    static inline void copyFromInterleaved (DestFloatType* const* destChannels, uint32_t destStartFrame, const SourceFloatType* monoSource, uint32_t numFrames)
    {
        auto dest = *destChannels + destStartFrame;

        for (uint32_t i = 0; i < numFrames; ++i)
            dest[i] = static_cast<DestFloatType> (monoSource[i]);
    }

    template <typename SourceFloatType, typename DestFloatType, int32_t numChannels>
    static inline void copyFromInterleaved (DestFloatType* const* destChannels, uint32_t destStartFrame, const Vector<SourceFloatType, numChannels>* vectorSource, uint32_t numFrames)
    {
        for (uint32_t i = 0; i < numFrames; ++i)
            for (uint32_t chan = 0; chan < static_cast<uint32_t> (numChannels); ++chan)
                destChannels[chan][destStartFrame + i] = static_cast<DestFloatType> (vectorSource[i].elements[chan]);
    }

    //==============================================================================
    struct _RenderStats
    {
        int32_t m_underrunCount, m_underrunFrames, m_overrunCount, m_overrunFrames;
    };

    struct _SparseStreamStatus
    {
        int32_t m_activeRamps;
        FixedArray<int32_t, 2> m_rampArray;
    };

    struct _Event_in_f32_1
    {
        int32_t m_numFrames;
    };

    struct _Event_in_struct_Message_1
    {
        int32_t m_numFrames;
    };

    struct _Stream_out_f32_1024
    {
        FixedArray<float, 1024> m_buffer;
    };

    struct soul__midi__MPEParser___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
    };

    struct soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator__VoiceInfo
    {
        bool m_active;
        int32_t m_channel;
        float m_note;
        int32_t m_voiceAge;
        bool m_noteReleased;
    };

    struct soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId, m_nextAllocatedVoiceAge, m_nextUnallocatedVoiceAge;
        FixedArray<soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator__VoiceInfo, 8> m_voiceInfo;
        FixedArray<bool, 16> m_channelSustainActive;
        bool m_masterSustainActive;
    };

    struct SineOsc___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_notePitch, m_bendSemitones, m_phase, m_phaseIncrement;
    };

    struct soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        bool m_active;
        float m_targetLevel, m_attackLevel, m_attackMultiplier, m_level, m_releaseMultiplier;
    };

    struct soul__gain__DynamicGain___for__root__Voice_attenuator___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
    };

    struct Voice___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        SineOsc___State m_osc_state;
        soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___State m_amplitudeEnvelope_state;
        soul__gain__DynamicGain___for__root__Voice_attenuator___State m_attenuator_state;
    };

    struct soul__gain__DynamicGain___for__root__SineSynth_gainProcessor___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
    };

    struct soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId;
        float m_targetGain, m_currentGain, m_increment;
        int32_t m_remainingRampSamples;
    };

    struct _State
    {
        int32_t m__resumePoint, m__frameCount, m__arrayEntry, m__sessionID, m__processorId, m__framesToAdvance;
        _RenderStats m__renderStats;
        _SparseStreamStatus m__sparseStreamStatus;
        _Event_in_f32_1 m__in_volume;
        _Event_in_struct_Message_1 m__in_midiIn;
        _Stream_out_f32_1024 m__out_audioOut;
        soul__midi__MPEParser___State m_MPEParser_state;
        soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State m_voiceAllocator_state;
        FixedArray<Voice___State, 8> m_voices_state;
        soul__gain__DynamicGain___for__root__SineSynth_gainProcessor___State m_gainProcessor_state;
        soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___State m_smoothedGain_state;
    };

    struct soul__midi__Message
    {
        int32_t m_midiBytes;
    };

    struct soul__note_events__NoteOn
    {
        int32_t m_channel;
        float m_note, m_velocity;
    };

    struct soul__note_events__NoteOff
    {
        int32_t m_channel;
        float m_note, m_velocity;
    };

    struct soul__note_events__PitchBend
    {
        int32_t m_channel;
        float m_bendSemitones;
    };

    struct soul__note_events__Pressure
    {
        int32_t m_channel;
        float m_pressure;
    };

    struct soul__note_events__Slide
    {
        int32_t m_channel;
        float m_slide;
    };

    struct soul__note_events__Control
    {
        int32_t m_channel, m_control;
        float m_value;
    };

    struct soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___IO
    {
        float m__out_gain;
    };

    struct soul__gain__DynamicGain___for__root__SineSynth_gainProcessor___IO
    {
        float m__in_in, m__in_gain, m__out_out;
    };

    struct soul__gain__DynamicGain___for__root__Voice_attenuator___IO
    {
        float m__in_in, m__in_gain, m__out_out;
    };

    struct soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___IO
    {
        float m__out_levelOut;
    };

    struct SineOsc___IO
    {
        float m__out_audioOut;
    };

    struct Voice___IO
    {
        float m__out_audioOut;
    };

    //==============================================================================
    #if __clang__
     #pragma clang diagnostic push
     #pragma clang diagnostic ignored "-Wunused-label"
     #pragma clang diagnostic ignored "-Wunused-parameter"
     #pragma clang diagnostic ignored "-Wshadow"
    #elif defined(__GNUC__)
     #pragma GCC diagnostic push
     #pragma GCC diagnostic ignored "-Wunused-label"
     #pragma GCC diagnostic ignored "-Wunused-parameter"
     #pragma GCC diagnostic ignored "-Wshadow"
    #elif defined(_MSC_VER)
     #pragma warning (push)
     #pragma warning (disable: 4100 4102 4458)
    #endif

    //==============================================================================
    int32_t run (_State& _state, int32_t maxFrames) noexcept
    {
        int32_t _2 = {};
        FixedArray<Voice___IO, 8> _3 = {};
        soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___IO _4 = {};
        soul__gain__DynamicGain___for__root__SineSynth_gainProcessor___IO _5 = {};

        _2 = _internal___minInt32 (1024, maxFrames);
        _updateRampingStreams (_state, _2);
        _state.m__frameCount = 0;
        _main_loop_check: { if (! (_state.m__frameCount < _2)) goto _exit; }
        _main_loop_body: { _3 = ZeroInitialiser();
                           Voice__run (_state.m_voices_state[0], _3[0]);
                           Voice__run (_state.m_voices_state[1], _3[1]);
                           Voice__run (_state.m_voices_state[2], _3[2]);
                           Voice__run (_state.m_voices_state[3], _3[3]);
                           Voice__run (_state.m_voices_state[4], _3[4]);
                           Voice__run (_state.m_voices_state[5], _3[5]);
                           Voice__run (_state.m_voices_state[6], _3[6]);
                           Voice__run (_state.m_voices_state[7], _3[7]);
                           _4 = ZeroInitialiser();
                           soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain__run (_state.m_smoothedGain_state, _4);
                           _5 = ZeroInitialiser();
                           _5.m__in_in = ((((((_3[0].m__out_audioOut + _3[1].m__out_audioOut) + _3[2].m__out_audioOut) + _3[3].m__out_audioOut) + _3[4].m__out_audioOut) + _3[5].m__out_audioOut) + _3[6].m__out_audioOut) + _3[7].m__out_audioOut;
                           _5.m__in_gain = _4.m__out_gain;
                           soul__gain__DynamicGain___for__root__SineSynth_gainProcessor__run (_state.m_gainProcessor_state, _5);
                           _writeToStream_struct__Stream_out_f32_1024 (_state.m__out_audioOut, _state.m__frameCount, _5.m__out_out);
                           _state.m__frameCount = _state.m__frameCount + 1;
                           goto _main_loop_check;
        }
        _exit: { _state.m__frameCount = 0;
                 return _2;
        }
    }

    void _initialise (_State& _state, int32_t sessionID) noexcept
    {
        _state.m__sessionID = sessionID;
        _state.m_MPEParser_state.m__arrayEntry = 0;
        _state.m_MPEParser_state.m__sessionID = _state.m__sessionID;
        _state.m_MPEParser_state.m__processorId = 4;
        _state.m_voiceAllocator_state.m__arrayEntry = 0;
        _state.m_voiceAllocator_state.m__sessionID = _state.m__sessionID;
        _state.m_voiceAllocator_state.m__processorId = 5;
        soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___initialise (_state.m_voiceAllocator_state);
        _state.m_voices_state[0].m__arrayEntry = 0;
        _state.m_voices_state[0].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[0].m__processorId = 6;
        Voice___initialise (_state.m_voices_state[0]);
        _state.m_voices_state[1].m__arrayEntry = 1;
        _state.m_voices_state[1].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[1].m__processorId = 7;
        Voice___initialise (_state.m_voices_state[1]);
        _state.m_voices_state[2].m__arrayEntry = 2;
        _state.m_voices_state[2].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[2].m__processorId = 8;
        Voice___initialise (_state.m_voices_state[2]);
        _state.m_voices_state[3].m__arrayEntry = 3;
        _state.m_voices_state[3].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[3].m__processorId = 9;
        Voice___initialise (_state.m_voices_state[3]);
        _state.m_voices_state[4].m__arrayEntry = 4;
        _state.m_voices_state[4].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[4].m__processorId = 10;
        Voice___initialise (_state.m_voices_state[4]);
        _state.m_voices_state[5].m__arrayEntry = 5;
        _state.m_voices_state[5].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[5].m__processorId = 11;
        Voice___initialise (_state.m_voices_state[5]);
        _state.m_voices_state[6].m__arrayEntry = 6;
        _state.m_voices_state[6].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[6].m__processorId = 12;
        Voice___initialise (_state.m_voices_state[6]);
        _state.m_voices_state[7].m__arrayEntry = 7;
        _state.m_voices_state[7].m__sessionID = _state.m__sessionID;
        _state.m_voices_state[7].m__processorId = 13;
        Voice___initialise (_state.m_voices_state[7]);
        _state.m_gainProcessor_state.m__arrayEntry = 0;
        _state.m_gainProcessor_state.m__sessionID = _state.m__sessionID;
        _state.m_gainProcessor_state.m__processorId = 14;
        _state.m_smoothedGain_state.m__arrayEntry = 0;
        _state.m_smoothedGain_state.m__sessionID = _state.m__sessionID;
        _state.m_smoothedGain_state.m__processorId = 15;
        soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___initialise (_state.m_smoothedGain_state);
    }

    void _addInputEvent_volume_f32 (_State& _state, const float& event) noexcept
    {
        soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___volume_f32 (_state.m_smoothedGain_state, event);
    }

    void _addInputEvent_midiIn_struct_Message (_State& _state, const soul__midi__Message& event) noexcept
    {
        soul__midi__MPEParser___parseMIDI_struct_Message (_state.m_MPEParser_state, event);
    }

    FixedArray<float, 1024>& _getOutputFrameArrayRef_audioOut (_State& state) noexcept
    {
        return state.m__out_audioOut.m_buffer;
    }

    void _prepare (_State& state, int32_t frames) noexcept
    {
        state.m__framesToAdvance = frames;
    }

    int32_t _get_num_xruns (_State& state) noexcept
    {
        return state.m__renderStats.m_underrunCount + state.m__renderStats.m_overrunCount;
    }

    //==============================================================================
    bool _updateRampingStream (_State& _state, int32_t streamId, int32_t framesToRender) noexcept
    {
        bool rampComplete = {};

        rampComplete = false;
        return rampComplete;
    }

    void _updateRampingStreams (_State& _state, int32_t framesToRender) noexcept
    {
        bool rampComplete = {};
        int32_t readPos = {}, writePos = {};

        rampComplete = false;
        readPos = 0;
        writePos = 0;
        if (_state.m__sparseStreamStatus.m_activeRamps == 0) goto _exit;
        _loop: { rampComplete = _updateRampingStream (_state, _state.m__sparseStreamStatus.m_rampArray[readPos], framesToRender);
                 if (rampComplete) goto _rampComplete;
        }
        _rampActive: { _state.m__sparseStreamStatus.m_rampArray[writePos] = _state.m__sparseStreamStatus.m_rampArray[readPos];
                       readPos = readPos + 1;
                       writePos = writePos + 1;
                       if (readPos == _state.m__sparseStreamStatus.m_activeRamps) goto _loopExit;
                       goto _loop;
        }
        _rampComplete: { readPos = readPos + 1;
                         if (! (readPos == _state.m__sparseStreamStatus.m_activeRamps)) goto _loop;
        }
        _loopExit: { _state.m__sparseStreamStatus.m_activeRamps = writePos; }
        _exit: {}
    }

    void _writeToStream_struct__Stream_out_f32_1024 (_Stream_out_f32_1024& stream, int32_t writePos, float value) noexcept
    {
        stream.m_buffer[writePos] = value;
    }

    //==============================================================================
    float soul__dBtoGain (float decibels) noexcept
    {
        float _2 = {}, _3 = {}, _4 = {}, _T0 = {};

        if (! (decibels > -100.0f)) goto _ternary_false_0;
        _ternary_true_0: { _2 = SOUL_INTRINSICS::pow (10.0f, decibels * 0.05f);
                           _T0 = _2;
                           goto _ternary_end_0;
        }
        _ternary_false_0: { _3 = 0;
                            _T0 = _3;
        }
        _ternary_end_0: { _4 = _T0;
                          return _4;
        }
    }

    float soul__noteNumberToFrequency_2 (float note) noexcept
    {
        float _2 = {};

        _2 = SOUL_INTRINSICS::pow (2.0f, (note - 69.0f) * 0.083333336f);
        return 440.0f * _2;
    }

    //==============================================================================
    float soul__intrinsics___pow_specialised (float a, float b) noexcept
    {
        return 0;
    }

    float soul__intrinsics___sin_specialised (float n) noexcept
    {
        return 0;
    }

    float soul__intrinsics___abs_specialised (float n) noexcept
    {
        float _2 = {}, _3 = {}, _4 = {}, _T0 = {};

        if (! (n < 0)) goto _ternary_false_0;
        _ternary_true_0: { _2 = -n;
                           _T0 = _2;
                           goto _ternary_end_0;
        }
        _ternary_false_0: { _3 = n;
                            _T0 = _3;
        }
        _ternary_end_0: { _4 = _T0;
                          return _4;
        }
    }

    int32_t soul__intrinsics___max_specialised (int32_t a, int32_t b) noexcept
    {
        int32_t _2 = {}, _3 = {}, _4 = {}, _T0 = {};

        if (! (a > b)) goto _ternary_false_0;
        _ternary_true_0: { _2 = a;
                           _T0 = _2;
                           goto _ternary_end_0;
        }
        _ternary_false_0: { _3 = b;
                            _T0 = _3;
        }
        _ternary_end_0: { _4 = _T0;
                          return _4;
        }
    }

    float soul__intrinsics___addModulo2Pi_specialised (float value, float increment) noexcept
    {
        float _2 = {}, _3 = {}, _4 = {}, _T2 = {};

        value = value + increment;
        if (! (value >= 6.2831855f)) goto _ifnot_0;
        _if_0: { if (! (value >= 12.566371f)) goto _ifnot_1; }
        _if_1: { return SOUL_INTRINSICS::fmod (value, 6.2831855f); }
        _ifnot_1: { return value - 6.2831855f; }
        _ifnot_0: { if (! (value < 0)) goto _ternary_false_2; }
        _ternary_true_2: { _2 = SOUL_INTRINSICS::fmod (value, 6.2831855f) + 6.2831855f;
                           _T2 = _2;
                           goto _ternary_end_2;
        }
        _ternary_false_2: { _3 = value;
                            _T2 = _3;
        }
        _ternary_end_2: { _4 = _T2;
                          return _4;
        }
    }

    //==============================================================================
    int32_t soul__midi__getByte1 (soul__midi__Message m) noexcept
    {
        return (m.m_midiBytes >> 16) & 255;
    }

    int32_t soul__midi__getByte2 (soul__midi__Message m) noexcept
    {
        return (m.m_midiBytes >> 8) & 255;
    }

    int32_t soul__midi__getByte3 (soul__midi__Message m) noexcept
    {
        return m.m_midiBytes & 255;
    }

    //==============================================================================
    static _State& _stateUpCast (soul__midi__MPEParser___State& s)
    {
        auto offset = static_cast<int32_t> (offsetof (_State, m_MPEParser_state));
        return *reinterpret_cast<_State*> (reinterpret_cast<char*> (&s) - offset);
    }

    //==============================================================================
    void soul__midi__MPEParser___parseMIDI_struct_Message (soul__midi__MPEParser___State& _state, soul__midi__Message message) noexcept
    {
        int32_t _2 = {}, _3 = {}, _4 = {};
        int32_t messageByte1 = {}, messageByte2 = {}, messageByte3 = {}, messageType = {}, channel = {};
        soul__note_events__NoteOff _5 = {}, _6 = {};
        float _7 = {}, _8 = {}, _9 = {}, _10 = {}, _11 = {}, _12 = {};
        soul__note_events__NoteOn _13 = {};
        soul__note_events__Slide _14 = {};
        soul__note_events__Control _15 = {};
        soul__note_events__Pressure _16 = {};
        soul__note_events__PitchBend _17 = {};

        _2 = soul__midi__getByte1 (message);
        messageByte1 = static_cast<int32_t> (_2);
        _3 = soul__midi__getByte2 (message);
        messageByte2 = static_cast<int32_t> (_3);
        _4 = soul__midi__getByte3 (message);
        messageByte3 = static_cast<int32_t> (_4);
        messageType = static_cast<int32_t> (static_cast<int32_t> (messageByte1) & 240);
        channel = static_cast<int32_t> (static_cast<int32_t> (messageByte1) & 15);
        if (! (messageType == 128)) goto _ifnot_0;
        _if_0: { _5 = ZeroInitialiser();
                 _5.m_channel = static_cast<int32_t> (channel);
                 _5.m_note = static_cast<float> (messageByte2);
                 _7 = soul__midi__MPEParser__normaliseValue (_state, static_cast<int32_t> (messageByte3));
                 _5.m_velocity = _7;
                 soul__midi__MPEParser___eventOut_struct_NoteOff (_state, _5);
                 goto _ifend_0;
        }
        _ifnot_0: { if (! (messageType == 144)) goto _ifnot_1; }
        _if_1: { if (! (messageByte3 == 0)) goto _ifnot_2; }
        _if_2: { _6 = ZeroInitialiser();
                 _6.m_channel = static_cast<int32_t> (channel);
                 _6.m_note = static_cast<float> (messageByte2);
                 soul__midi__MPEParser___eventOut_struct_NoteOff (_state, _6);
                 goto _ifend_0;
        }
        _ifnot_2: { _13 = ZeroInitialiser();
                    _13.m_channel = static_cast<int32_t> (channel);
                    _13.m_note = static_cast<float> (messageByte2);
                    _8 = soul__midi__MPEParser__normaliseValue (_state, static_cast<int32_t> (messageByte3));
                    _13.m_velocity = _8;
                    soul__midi__MPEParser___eventOut_struct_NoteOn (_state, _13);
                    goto _ifend_0;
        }
        _ifnot_1: { if (! (messageType == 176)) goto _ifnot_3; }
        _if_3: { if (! (messageByte2 == 74)) goto _ifnot_4; }
        _if_4: { _14 = ZeroInitialiser();
                 _14.m_channel = static_cast<int32_t> (channel);
                 _9 = soul__midi__MPEParser__normaliseValue (_state, static_cast<int32_t> (messageByte3));
                 _14.m_slide = _9;
                 goto _ifend_0;
        }
        _ifnot_4: { _15 = ZeroInitialiser();
                    _15.m_channel = static_cast<int32_t> (channel);
                    _15.m_control = static_cast<int32_t> (messageByte2);
                    _10 = soul__midi__MPEParser__normaliseValue (_state, static_cast<int32_t> (messageByte3));
                    _15.m_value = _10;
                    soul__midi__MPEParser___eventOut_struct_Control (_state, _15);
                    goto _ifend_0;
        }
        _ifnot_3: { if (! (messageType == 208)) goto _ifnot_5; }
        _if_5: { _16 = ZeroInitialiser();
                 _16.m_channel = static_cast<int32_t> (channel);
                 _11 = soul__midi__MPEParser__normaliseValue (_state, static_cast<int32_t> (messageByte2));
                 _16.m_pressure = _11;
                 goto _ifend_0;
        }
        _ifnot_5: { if (! (messageType == 224)) goto _ifend_0; }
        _if_6: { _17 = ZeroInitialiser();
                 _17.m_channel = static_cast<int32_t> (channel);
                 _12 = soul__midi__MPEParser__translateBendSemitones (_state, static_cast<int32_t> (messageByte3), static_cast<int32_t> (messageByte2));
                 _17.m_bendSemitones = _12;
                 soul__midi__MPEParser___eventOut_struct_PitchBend (_state, _17);
        }
        _ifend_0: {}
    }

    float soul__midi__MPEParser__normaliseValue (soul__midi__MPEParser___State& _state, int32_t i) noexcept
    {
        return static_cast<float> (i) * 0.007874016f;
    }

    float soul__midi__MPEParser__translateBendSemitones (soul__midi__MPEParser___State& _state, int32_t msb, int32_t lsb) noexcept
    {
        int32_t value = {};

        value = static_cast<int32_t> ((msb * 128) + lsb);
        return static_cast<float> (value - 8192) / 170.66667f;
    }

    void soul__midi__MPEParser___eventOut_struct_NoteOff (soul__midi__MPEParser___State& _state, soul__note_events__NoteOff value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___eventIn_struct_NoteOff (_2.m_voiceAllocator_state, value);
    }

    void soul__midi__MPEParser___eventOut_struct_NoteOn (soul__midi__MPEParser___State& _state, soul__note_events__NoteOn value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___eventIn_struct_NoteOn (_2.m_voiceAllocator_state, value);
    }

    void soul__midi__MPEParser___eventOut_struct_Control (soul__midi__MPEParser___State& _state, soul__note_events__Control value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___eventIn_struct_Control (_2.m_voiceAllocator_state, value);
    }

    void soul__midi__MPEParser___eventOut_struct_PitchBend (soul__midi__MPEParser___State& _state, soul__note_events__PitchBend value) noexcept
    {
        auto& _2 = _stateUpCast (_state);
        soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___eventIn_struct_PitchBend (_2.m_voiceAllocator_state, value);
    }

    //==============================================================================
    void soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___volume_f32 (soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___State& _state, float targetDB) noexcept
    {
        float _2 = {}, _3 = {};
        float maxDelta = {};
        int32_t _4 = {};

        _2 = soul__dBtoGain (targetDB);
        _state.m_targetGain = _2;
        maxDelta = static_cast<float> (static_cast<float> ((1.0 / (sampleRate * 1.0))) / 0.5f);
        _3 = soul__intrinsics___abs_specialised (_state.m_targetGain - _state.m_currentGain);
        _4 = soul__intrinsics___max_specialised (1, static_cast<int32_t> (_3 / static_cast<float> (maxDelta)));
        _state.m_remainingRampSamples = _4;
        _state.m_increment = (_state.m_targetGain - _state.m_currentGain) / static_cast<float> (_state.m_remainingRampSamples);
    }

    void soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain__run (soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___State& _state, soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___IO& _io) noexcept
    {
        float out_value_gain = {};
        int32_t _2 = {}, _3 = {};

        out_value_gain = 0;
        if (! (_state.m_remainingRampSamples != 0)) goto _ifnot_0;
        _if_0: { _2 = _state.m_remainingRampSamples;
                 _3 = _2 - 1;
                 _state.m_remainingRampSamples = _3;
                 if (! (_3 == 0)) goto _ifnot_1;
        }
        _if_1: { _state.m_currentGain = _state.m_targetGain;
                 goto _ifnot_0;
        }
        _ifnot_1: { _state.m_currentGain = _state.m_currentGain + _state.m_increment; }
        _ifnot_0: { out_value_gain = out_value_gain + _state.m_currentGain;
                    _state.m__resumePoint = 1;
                    _io.m__out_gain = out_value_gain;
        }
    }

    void soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___initialise (soul__gain__SmoothedGainParameter___for__root__SineSynth_smoothedGain___State& _state) noexcept
    {
        _state.m_targetGain = 0;
        _state.m_currentGain = 0;
        _state.m_increment = 0;
        _state.m_remainingRampSamples = 0;
    }

    //==============================================================================
    void soul__gain__DynamicGain___for__root__SineSynth_gainProcessor__run (soul__gain__DynamicGain___for__root__SineSynth_gainProcessor___State& _state, soul__gain__DynamicGain___for__root__SineSynth_gainProcessor___IO& _io) noexcept
    {
        float out_value_out = {}, _2 = {}, _3 = {};

        out_value_out = 0;
        _2 = _io.m__in_in;
        _3 = _io.m__in_gain;
        out_value_out = out_value_out + (_2 * _3);
        _state.m__resumePoint = 1;
        _io.m__out_out = out_value_out;
    }

    //==============================================================================
    void soul__gain__DynamicGain___for__root__Voice_attenuator__run (soul__gain__DynamicGain___for__root__Voice_attenuator___State& _state, soul__gain__DynamicGain___for__root__Voice_attenuator___IO& _io) noexcept
    {
        float out_value_out = {}, _2 = {}, _3 = {};

        out_value_out = 0;
        _2 = _io.m__in_in;
        _3 = _io.m__in_gain;
        out_value_out = out_value_out + (_2 * _3);
        _state.m__resumePoint = 1;
        _io.m__out_out = out_value_out;
    }

    //==============================================================================
    void soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___noteIn_struct_NoteOn (soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___State& _state, soul__note_events__NoteOn e) noexcept
    {
        _state.m_active = true;
        _state.m_targetLevel = e.m_velocity;
    }

    void soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___noteIn_struct_NoteOff (soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___State& _state, soul__note_events__NoteOff e) noexcept
    {
        _state.m_active = false;
    }

    void soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope__run (soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___State& _state, soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___IO& _io) noexcept
    {
        float out_value_levelOut = {}, _2 = {}, _3 = {}, _4 = {};
        int32_t _resumePoint = {};
        int32_t attackSamples = {};
        bool _5 = {}, _6 = {}, _7 = {}, _8 = {}, _9 = {}, _10 = {}, _T0 = {}, _T1 = {};

        out_value_levelOut = 0;
        _resumePoint = _state.m__resumePoint;
        if (_resumePoint == 1) goto _loop_1;
        _check_resume_point_2: { if (_resumePoint == 2) goto _resume_point_2; }
        _check_resume_point_3: { if (_resumePoint == 3) goto _loop_3; }
        _check_resume_point_4: { if (_resumePoint == 4) goto _loop_4; }
        _loop_1: { if (! (! _state.m_active)) goto _break_1; }
        _advance_start_0: { _state.m__resumePoint = 1;
                            goto _exit;
        }
        _break_1: { _state.m_level = 0;
                    attackSamples = static_cast<int32_t> (static_cast<int32_t> (static_cast<float> ((sampleRate * 1.0)) * 0.02f));
                    _2 = SOUL_INTRINSICS::pow (2.0f, -1.0f / static_cast<float> (attackSamples));
                    _3 = SOUL_INTRINSICS::pow (_state.m_targetLevel + 2.0f, 1.0f / static_cast<float> (attackSamples));
                    _state.m_attackMultiplier = static_cast<float> (_2 * _3);
                    _state.m_attackLevel = 2.0f;
        }
        _loop_2: { if (! _state.m_active) goto _ternary_false_0; }
        _ternary_true_0: { _5 = _state.m_level < _state.m_targetLevel;
                           _T0 = _5;
                           goto _ternary_end_0;
        }
        _ternary_false_0: { _6 = false;
                            _T0 = _6;
        }
        _ternary_end_0: { _7 = _T0;
                          if (! _7) goto _loop_3;
        }
        _body_2: { _state.m_level = _state.m_attackLevel - 2.0f;
                   out_value_levelOut = out_value_levelOut + _state.m_level;
                   _state.m__resumePoint = 2;
                   goto _exit;
        }
        _resume_point_2: { _state.m_attackLevel = _state.m_attackLevel * static_cast<float> (_state.m_attackMultiplier);
                           goto _loop_2;
        }
        _loop_3: { if (! _state.m_active) goto _break_3; }
        _body_3: { out_value_levelOut = out_value_levelOut + _state.m_level;
                   _state.m__resumePoint = 3;
                   goto _exit;
        }
        _break_3: { _4 = SOUL_INTRINSICS::pow (0.0001f, static_cast<float> ((1.0 / (sampleRate * 1.0))) / 0.1f);
                    _state.m_releaseMultiplier = static_cast<float> (_4);
        }
        _loop_4: { if (! (! _state.m_active)) goto _ternary_false_1; }
        _ternary_true_1: { _8 = _state.m_level > 0.00001f;
                           _T1 = _8;
                           goto _ternary_end_1;
        }
        _ternary_false_1: { _9 = false;
                            _T1 = _9;
        }
        _ternary_end_1: { _10 = _T1;
                          if (! _10) goto _loop_1;
        }
        _body_4: { out_value_levelOut = out_value_levelOut + _state.m_level;
                   _state.m_level = _state.m_level * static_cast<float> (_state.m_releaseMultiplier);
                   _state.m__resumePoint = 4;
        }
        _exit: { _io.m__out_levelOut = out_value_levelOut; }
    }

    void soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___initialise (soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___State& _state) noexcept
    {
        _state.m_active = false;
        _state.m_targetLevel = 0;
    }

    //==============================================================================
    static _State& _stateUpCast (soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State& s)
    {
        auto offset = static_cast<int32_t> (offsetof (_State, m_voiceAllocator_state));
        return *reinterpret_cast<_State*> (reinterpret_cast<char*> (&s) - offset);
    }

    //==============================================================================
    void soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___eventIn_struct_NoteOn (soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State& _state, soul__note_events__NoteOn e) noexcept
    {
        int32_t allocatedVoice = {};
        int32_t allocatedVoiceAge = {}, i = {}, _2 = {}, _3 = {};
        int32_t age = {};
        soul__note_events__NoteOff noteOff = {};

        allocatedVoice = 0;
        allocatedVoiceAge = _state.m_voiceInfo[allocatedVoice].m_voiceAge;
        i = 1;
        _loop_0: { if (! (i < 8)) goto _break_0; }
        _body_0: { age = static_cast<int32_t> (_state.m_voiceInfo[_intrin_wrap (static_cast<int32_t> (i), 8) & 7].m_voiceAge);
                   if (! (age < static_cast<int32_t> (allocatedVoiceAge))) goto _cont_0;
        }
        _if_0: { allocatedVoiceAge = static_cast<int32_t> (age);
                 allocatedVoice = _intrin_wrap (static_cast<int32_t> (i), 8) & 7;
        }
        _cont_0: { _2 = i;
                   i = _2 + 1;
                   goto _loop_0;
        }
        _break_0: { if (! _state.m_voiceInfo[allocatedVoice].m_active) goto _ifnot_1; }
        _if_1: { noteOff = ZeroInitialiser();
                 noteOff.m_channel = _state.m_voiceInfo[allocatedVoice].m_channel;
                 noteOff.m_note = _state.m_voiceInfo[allocatedVoice].m_note;
                 soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___voiceEventOut_struct_NoteOff (_state, static_cast<int32_t> (allocatedVoice) & 7, noteOff);
        }
        _ifnot_1: { soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___voiceEventOut_struct_NoteOn (_state, static_cast<int32_t> (allocatedVoice) & 7, e);
                    _state.m_voiceInfo[allocatedVoice].m_active = true;
                    _state.m_voiceInfo[allocatedVoice].m_noteReleased = false;
                    _state.m_voiceInfo[allocatedVoice].m_channel = e.m_channel;
                    _state.m_voiceInfo[allocatedVoice].m_note = e.m_note;
                    _3 = _state.m_nextAllocatedVoiceAge;
                    _state.m_nextAllocatedVoiceAge = _3 + 1;
                    _state.m_voiceInfo[allocatedVoice].m_voiceAge = _3;
        }
    }

    void soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___eventIn_struct_NoteOff (soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State& _state, soul__note_events__NoteOff e) noexcept
    {
        int32_t voice = {}, _2 = {};
        bool _3 = {}, _4 = {}, pedalDown = {}, _5 = {}, _6 = {}, _7 = {}, _T0 = {}, _T2 = {};
        int32_t counter_0 = {};
        int32_t _8 = {};

        voice = 0;
        if (! _state.m_masterSustainActive) goto _ternary_false_0;
        _ternary_true_0: { _3 = true;
                           _T0 = _3;
                           goto _ternary_end_0;
        }
        _ternary_false_0: { _4 = _state.m_channelSustainActive[_intrin_wrap (static_cast<int32_t> (e.m_channel), 16) & 15];
                            _T0 = _4;
        }
        _ternary_end_0: { pedalDown = _T0;
                          counter_0 = 8;
        }
        _loop_0: { if (! (counter_0 > 0)) goto _break_0; }
        _body_0: { if (! (_state.m_voiceInfo[voice].m_channel == e.m_channel)) goto _ternary_false_2; }
        _ternary_true_2: { _5 = _state.m_voiceInfo[voice].m_note == e.m_note;
                           _T2 = _5;
                           goto _ternary_end_2;
        }
        _ternary_false_2: { _6 = false;
                            _T2 = _6;
        }
        _ternary_end_2: { _7 = _T2;
                          if (! _7) goto _ifnot_1;
        }
        _if_1: { if (! pedalDown) goto _ifnot_3; }
        _if_3: { _state.m_voiceInfo[voice].m_noteReleased = true;
                 goto _ifnot_1;
        }
        _ifnot_3: { _state.m_voiceInfo[voice].m_active = false;
                    _8 = _state.m_nextUnallocatedVoiceAge;
                    _state.m_nextUnallocatedVoiceAge = _8 + 1;
                    _state.m_voiceInfo[voice].m_voiceAge = _8;
                    soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___voiceEventOut_struct_NoteOff (_state, static_cast<int32_t> (voice) & 7, e);
        }
        _ifnot_1: { _2 = voice;
                    voice = (_2 + 1) & 7;
                    counter_0 = counter_0 - 1;
                    goto _loop_0;
        }
        _break_0: {}
    }

    void soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___eventIn_struct_PitchBend (soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State& _state, soul__note_events__PitchBend e) noexcept
    {
        int32_t voice = {}, _2 = {};
        int32_t counter_0 = {};

        voice = 0;
        counter_0 = 8;
        _loop_0: { if (! (counter_0 > 0)) goto _break_0; }
        _body_0: { if (! (_state.m_voiceInfo[voice].m_channel == e.m_channel)) goto _ifnot_0; }
        _if_0: { soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___voiceEventOut_struct_PitchBend (_state, static_cast<int32_t> (voice) & 7, e); }
        _ifnot_0: { _2 = voice;
                    voice = (_2 + 1) & 7;
                    counter_0 = counter_0 - 1;
                    goto _loop_0;
        }
        _break_0: {}
    }

    void soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___eventIn_struct_Control (soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State& _state, soul__note_events__Control c) noexcept
    {
        bool pedalDown = {}, isMasterChannel = {}, _2 = {}, _3 = {}, _4 = {}, _5 = {}, _6 = {}, _7 = {}, _8 = {}, _9 = {}, _10 = {}, _T6 = {}, _T5 = {}, _T4 = {};
        int32_t voice = {}, _11 = {}, voice_2 = {}, _12 = {};
        int32_t counter_0 = {}, counter_1 = {};
        soul__note_events__NoteOff noteOff = {};
        int32_t _13 = {};

        if (! (c.m_control == 64)) goto _ifnot_0;
        _if_0: { pedalDown = c.m_value >= 0.5f;
                 isMasterChannel = c.m_channel == 0;
                 _state.m_channelSustainActive[_intrin_wrap (static_cast<int32_t> (c.m_channel), 16) & 15] = pedalDown;
                 if (! isMasterChannel) goto _ifnot_1;
        }
        _if_1: { _state.m_masterSustainActive = pedalDown; }
        _ifnot_1: { if (! (! pedalDown)) goto _ifnot_2; }
        _if_2: { voice = 0;
                 counter_0 = 8;
        }
        _loop_0: { if (! (counter_0 > 0)) goto _ifnot_2; }
        _body_0: { if (! isMasterChannel) goto _ternary_false_6; }
        _ternary_true_6: { _2 = true;
                           _T6 = _2;
                           goto _ternary_end_6;
        }
        _ternary_false_6: { _3 = _state.m_voiceInfo[voice].m_channel == c.m_channel;
                            _T6 = _3;
        }
        _ternary_end_6: { _4 = _T6;
                          if (! _4) goto _ternary_false_5;
        }
        _ternary_true_5: { _5 = _state.m_voiceInfo[voice].m_active == true;
                           _T5 = _5;
                           goto _ternary_end_5;
        }
        _ternary_false_5: { _6 = false;
                            _T5 = _6;
        }
        _ternary_end_5: { _7 = _T5;
                          if (! _7) goto _ternary_false_4;
        }
        _ternary_true_4: { _8 = _state.m_voiceInfo[voice].m_noteReleased == true;
                           _T4 = _8;
                           goto _ternary_end_4;
        }
        _ternary_false_4: { _9 = false;
                            _T4 = _9;
        }
        _ternary_end_4: { _10 = _T4;
                          if (! _10) goto _ifnot_3;
        }
        _if_3: { noteOff = ZeroInitialiser();
                 noteOff.m_channel = _state.m_voiceInfo[voice].m_channel;
                 noteOff.m_note = _state.m_voiceInfo[voice].m_note;
                 soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___voiceEventOut_struct_NoteOff (_state, static_cast<int32_t> (voice) & 7, noteOff);
                 _state.m_voiceInfo[voice].m_active = false;
                 _13 = _state.m_nextUnallocatedVoiceAge;
                 _state.m_nextUnallocatedVoiceAge = _13 + 1;
                 _state.m_voiceInfo[voice].m_voiceAge = _13;
        }
        _ifnot_3: { _11 = voice;
                    voice = (_11 + 1) & 7;
                    counter_0 = counter_0 - 1;
                    goto _loop_0;
        }
        _ifnot_2: { return; }
        _ifnot_0: { voice_2 = 0;
                    counter_1 = 8;
        }
        _loop_1: { if (! (counter_1 > 0)) goto _break_1; }
        _body_1: { if (! (_state.m_voiceInfo[voice_2].m_channel == c.m_channel)) goto _ifnot_7; }
        _if_7: {}
        _ifnot_7: { _12 = voice_2;
                    voice_2 = (_12 + 1) & 7;
                    counter_1 = counter_1 - 1;
                    goto _loop_1;
        }
        _break_1: {}
    }

    void soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___initialise (soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State& _state) noexcept
    {
        _state.m_nextAllocatedVoiceAge = 1000000000;
        _state.m_nextUnallocatedVoiceAge = 1;
        _state.m_voiceInfo = ZeroInitialiser();
        _state.m_channelSustainActive = ZeroInitialiser();
        _state.m_masterSustainActive = false;
    }

    void soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___voiceEventOut_struct_NoteOff (soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State& _state, int32_t element, soul__note_events__NoteOff value) noexcept
    {
        if (element == 0) goto _element_0;
        _block_1: { if (element == 1) goto _element_1; }
        _block_2: { if (element == 2) goto _element_2; }
        _block_3: { if (element == 3) goto _element_3; }
        _block_4: { if (element == 4) goto _element_4; }
        _block_5: { if (element == 5) goto _element_5; }
        _block_6: { if (element == 6) goto _element_6; }
        _block_7: { if (element == 7) goto _element_7; }
        _block_8: { return; }
        _element_0: { auto& _2 = _stateUpCast (_state);
                      Voice___noteOff_struct_NoteOff (_2.m_voices_state[0], value);
                      return;
        }
        _element_1: { auto& _3 = _stateUpCast (_state);
                      Voice___noteOff_struct_NoteOff (_3.m_voices_state[1], value);
                      return;
        }
        _element_2: { auto& _4 = _stateUpCast (_state);
                      Voice___noteOff_struct_NoteOff (_4.m_voices_state[2], value);
                      return;
        }
        _element_3: { auto& _5 = _stateUpCast (_state);
                      Voice___noteOff_struct_NoteOff (_5.m_voices_state[3], value);
                      return;
        }
        _element_4: { auto& _6 = _stateUpCast (_state);
                      Voice___noteOff_struct_NoteOff (_6.m_voices_state[4], value);
                      return;
        }
        _element_5: { auto& _7 = _stateUpCast (_state);
                      Voice___noteOff_struct_NoteOff (_7.m_voices_state[5], value);
                      return;
        }
        _element_6: { auto& _8 = _stateUpCast (_state);
                      Voice___noteOff_struct_NoteOff (_8.m_voices_state[6], value);
                      return;
        }
        _element_7: { auto& _9 = _stateUpCast (_state);
                      Voice___noteOff_struct_NoteOff (_9.m_voices_state[7], value);
        }
    }

    void soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___voiceEventOut_struct_NoteOn (soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State& _state, int32_t element, soul__note_events__NoteOn value) noexcept
    {
        if (element == 0) goto _element_0;
        _block_1: { if (element == 1) goto _element_1; }
        _block_2: { if (element == 2) goto _element_2; }
        _block_3: { if (element == 3) goto _element_3; }
        _block_4: { if (element == 4) goto _element_4; }
        _block_5: { if (element == 5) goto _element_5; }
        _block_6: { if (element == 6) goto _element_6; }
        _block_7: { if (element == 7) goto _element_7; }
        _block_8: { return; }
        _element_0: { auto& _2 = _stateUpCast (_state);
                      Voice___noteOn_struct_NoteOn (_2.m_voices_state[0], value);
                      return;
        }
        _element_1: { auto& _3 = _stateUpCast (_state);
                      Voice___noteOn_struct_NoteOn (_3.m_voices_state[1], value);
                      return;
        }
        _element_2: { auto& _4 = _stateUpCast (_state);
                      Voice___noteOn_struct_NoteOn (_4.m_voices_state[2], value);
                      return;
        }
        _element_3: { auto& _5 = _stateUpCast (_state);
                      Voice___noteOn_struct_NoteOn (_5.m_voices_state[3], value);
                      return;
        }
        _element_4: { auto& _6 = _stateUpCast (_state);
                      Voice___noteOn_struct_NoteOn (_6.m_voices_state[4], value);
                      return;
        }
        _element_5: { auto& _7 = _stateUpCast (_state);
                      Voice___noteOn_struct_NoteOn (_7.m_voices_state[5], value);
                      return;
        }
        _element_6: { auto& _8 = _stateUpCast (_state);
                      Voice___noteOn_struct_NoteOn (_8.m_voices_state[6], value);
                      return;
        }
        _element_7: { auto& _9 = _stateUpCast (_state);
                      Voice___noteOn_struct_NoteOn (_9.m_voices_state[7], value);
        }
    }

    void soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___voiceEventOut_struct_PitchBend (soul___voice_allocators__Basic___for__root__SineSynth_voiceAllocator___State& _state, int32_t element, soul__note_events__PitchBend value) noexcept
    {
        if (element == 0) goto _element_0;
        _block_1: { if (element == 1) goto _element_1; }
        _block_2: { if (element == 2) goto _element_2; }
        _block_3: { if (element == 3) goto _element_3; }
        _block_4: { if (element == 4) goto _element_4; }
        _block_5: { if (element == 5) goto _element_5; }
        _block_6: { if (element == 6) goto _element_6; }
        _block_7: { if (element == 7) goto _element_7; }
        _block_8: { return; }
        _element_0: { auto& _2 = _stateUpCast (_state);
                      Voice___pitchBend_struct_PitchBend (_2.m_voices_state[0], value);
                      return;
        }
        _element_1: { auto& _3 = _stateUpCast (_state);
                      Voice___pitchBend_struct_PitchBend (_3.m_voices_state[1], value);
                      return;
        }
        _element_2: { auto& _4 = _stateUpCast (_state);
                      Voice___pitchBend_struct_PitchBend (_4.m_voices_state[2], value);
                      return;
        }
        _element_3: { auto& _5 = _stateUpCast (_state);
                      Voice___pitchBend_struct_PitchBend (_5.m_voices_state[3], value);
                      return;
        }
        _element_4: { auto& _6 = _stateUpCast (_state);
                      Voice___pitchBend_struct_PitchBend (_6.m_voices_state[4], value);
                      return;
        }
        _element_5: { auto& _7 = _stateUpCast (_state);
                      Voice___pitchBend_struct_PitchBend (_7.m_voices_state[5], value);
                      return;
        }
        _element_6: { auto& _8 = _stateUpCast (_state);
                      Voice___pitchBend_struct_PitchBend (_8.m_voices_state[6], value);
                      return;
        }
        _element_7: { auto& _9 = _stateUpCast (_state);
                      Voice___pitchBend_struct_PitchBend (_9.m_voices_state[7], value);
        }
    }

    //==============================================================================
    void SineOsc___noteOn_struct_NoteOn (SineOsc___State& _state, soul__note_events__NoteOn e) noexcept
    {
        _state.m_notePitch = e.m_note;
        _state.m_bendSemitones = 0;
        SineOsc__calculatePhaseIncrement (_state);
    }

    void SineOsc___pitchBend_struct_PitchBend (SineOsc___State& _state, soul__note_events__PitchBend e) noexcept
    {
        _state.m_bendSemitones = e.m_bendSemitones;
        SineOsc__calculatePhaseIncrement (_state);
    }

    void SineOsc__calculatePhaseIncrement (SineOsc___State& _state) noexcept
    {
        float _2 = {};
        float noteFrequency = {};

        _2 = soul__noteNumberToFrequency_2 (_state.m_notePitch + _state.m_bendSemitones);
        noteFrequency = static_cast<float> (_2);
        _state.m_phaseIncrement = static_cast<float> ((static_cast<double> (noteFrequency) * 6.283185307179586) * (1.0 / (sampleRate * 1.0)));
    }

    void SineOsc__run (SineOsc___State& _state, SineOsc___IO& _io) noexcept
    {
        float out_value_audioOut = {}, _2 = {}, _3 = {};

        out_value_audioOut = 0;
        _2 = soul__intrinsics___addModulo2Pi_specialised (_state.m_phase, _state.m_phaseIncrement);
        _state.m_phase = _2;
        _3 = SOUL_INTRINSICS::sin (_state.m_phase);
        out_value_audioOut = out_value_audioOut + _3;
        _state.m__resumePoint = 1;
        _io.m__out_audioOut = out_value_audioOut;
    }

    void SineOsc___initialise (SineOsc___State& _state) noexcept
    {
        _state.m_notePitch = 0;
        _state.m_bendSemitones = 0;
        _state.m_phase = 0;
        _state.m_phaseIncrement = 0;
    }

    //==============================================================================
    void Voice___initialise (Voice___State& _state) noexcept
    {
        _state.m_osc_state.m__arrayEntry = 0;
        _state.m_osc_state.m__sessionID = _state.m__sessionID;
        _state.m_osc_state.m__processorId = 1;
        SineOsc___initialise (_state.m_osc_state);
        _state.m_amplitudeEnvelope_state.m__arrayEntry = 0;
        _state.m_amplitudeEnvelope_state.m__sessionID = _state.m__sessionID;
        _state.m_amplitudeEnvelope_state.m__processorId = 2;
        soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___initialise (_state.m_amplitudeEnvelope_state);
        _state.m_attenuator_state.m__arrayEntry = 0;
        _state.m_attenuator_state.m__sessionID = _state.m__sessionID;
        _state.m_attenuator_state.m__processorId = 3;
    }

    void Voice__run (Voice___State& _state, Voice___IO& _io) noexcept
    {
        SineOsc___IO _2 = {};
        soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___IO _3 = {};
        soul__gain__DynamicGain___for__root__Voice_attenuator___IO _4 = {};

        _2 = ZeroInitialiser();
        SineOsc__run (_state.m_osc_state, _2);
        _3 = ZeroInitialiser();
        soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope__run (_state.m_amplitudeEnvelope_state, _3);
        _4 = ZeroInitialiser();
        _4.m__in_in = _2.m__out_audioOut;
        _4.m__in_gain = _3.m__out_levelOut;
        soul__gain__DynamicGain___for__root__Voice_attenuator__run (_state.m_attenuator_state, _4);
        _io.m__out_audioOut = _4.m__out_out;
    }

    void Voice___noteOn_struct_NoteOn (Voice___State& _state, soul__note_events__NoteOn event) noexcept
    {
        SineOsc___noteOn_struct_NoteOn (_state.m_osc_state, event);
        soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___noteIn_struct_NoteOn (_state.m_amplitudeEnvelope_state, event);
    }

    void Voice___noteOff_struct_NoteOff (Voice___State& _state, soul__note_events__NoteOff event) noexcept
    {
        soul__envelope__FixedAttackReleaseEnvelope___for__root__Voice_amplitudeEnvelope___noteIn_struct_NoteOff (_state.m_amplitudeEnvelope_state, event);
    }

    void Voice___pitchBend_struct_PitchBend (Voice___State& _state, soul__note_events__PitchBend event) noexcept
    {
        SineOsc___pitchBend_struct_PitchBend (_state.m_osc_state, event);
    }

    //==============================================================================
    int32_t _internal___minInt32 (int32_t a, int32_t b) noexcept
    {
        if (! (a < b)) goto _moreThan;
        _lessThan: { return a; }
        _moreThan: { return b; }
    }


    #if __clang__
     #pragma clang diagnostic pop
    #elif defined(__GNUC__)
     #pragma GCC diagnostic pop
    #elif defined(_MSC_VER)
     #pragma warning (pop)
    #endif

    //==============================================================================
    // The program contains no string literals, so this function should never be called
    static constexpr const char* lookupStringLiteral (int32_t)  { return {}; }

    //==============================================================================
    _State state = {}, initialisedState;

    double sampleRate = 1.0;
    uint32_t framesToAdvance = 0;
    uint64_t totalFramesElapsed = 0;
};

