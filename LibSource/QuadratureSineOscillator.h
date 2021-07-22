#ifndef QUADRATURE_SINE_OSCILLATOR_H
#define QUADRATURE_SINE_OSCILLATOR_H

#include "ComplexOscillator.h"

class QuadratureSineOscillator : public ComplexOscillatorTemplate<QuadratureSineOscillator> {
public:
    static constexpr float begin_phase = 0;
    static constexpr float end_phase = 2 * M_PI;
    ComplexFloat getSample() {
        return ComplexFloat { cosf(phase), sinf(phase) };
    }
    void generate(ComplexFloatArray output) override {
        size_t len = output.getSize();
        for (size_t i = 0; i < len; ++i) {
            output[i].re = cosf(phase);
            output[i].im = sinf(phase);
            phase += incr; // allow phase to overrun
        }
        phase = fmodf(phase, end_phase);
    }
    void generate(ComplexFloatArray output, FloatArray fm) {
        size_t len = output.getSize();
        for (size_t i = 0; i < len; ++i) {
            output[i].re = cosf(phase);
            output[i].im = sinf(phase);
            phase += incr * (1 + fm[i]);
            // allow phase to overrun
        }
        phase = fmodf(phase, end_phase);
    }
    using ComplexOscillatorTemplate<QuadratureSineOscillator>::generate;
};

class FeedbackQuadratureSineOscillator : public ComplexOscillatorTemplate<FeedbackQuadratureSineOscillator> {
public:
    static constexpr float begin_phase = 0;
    static constexpr float end_phase = 2 * M_PI;
    void setFeedback(float feedback) {
        this->feedback = feedback;
    }
    ComplexFloat getSample() {
        last_sample = ComplexFloat(
            cosf(phase + last_sample.re * feedback), sinf(phase + last_sample.im * feedback));
        return last_sample;
    }
    void generate(ComplexFloatArray output) override {
        size_t len = output.getSize();
        for (size_t i = 0; i < len; ++i) {
            output[i].re = cosf(phase + last_sample.re * feedback);
            output[i].im = sinf(phase + last_sample.im * feedback);
            last_sample = output[i];
            phase += incr; // allow phase to overrun
        }
        phase = fmodf(phase, end_phase);
    }
    void generate(ComplexFloatArray output, FloatArray fm) {
        size_t len = output.getSize();
        for (size_t i = 0; i < len; ++i) {
            output[i].re = cosf(phase + last_sample.re * feedback);
            output[i].im = sinf(phase + last_sample.im * feedback);
            last_sample = output[i];
            phase += incr * (1 + fm[i]);
            // allow phase to overrun
        }
        phase = fmodf(phase, end_phase);
    }
    using ComplexOscillatorTemplate<FeedbackQuadratureSineOscillator>::generate;
protected:
    float feedback = 0;
    ComplexFloat last_sample = 0;
};

#endif /* QUADRATURE_SINE_OSCILLATOR_H */
