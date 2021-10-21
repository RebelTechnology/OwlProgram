#ifndef QUADRATURE_SINE_OSCILLATOR_H
#define QUADRATURE_SINE_OSCILLATOR_H

#include "ComplexOscillator.h"

/**
 * Oscillator outputs complex numbers on unit cycle. This means that axis are
 * 90 degrees out of phase at any point of time and oscillator's magnitude is
 * always equal to 1.0
 **/
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
    void generate(ComplexFloatArray output, FloatArray fm) override {
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

/**
 * An oscillator similar to QuadratureSineOscillator class that also includes
 * feedback control. Feedback value is a scalar value, meaning that both real and
 * imaginary axis get the same amount of feeback
 **/
class FeedbackQuadratureSineOscillator : public ComplexOscillatorTemplate<FeedbackQuadratureSineOscillator> {
public:
    static constexpr float begin_phase = 0;
    static constexpr float end_phase = 2 * M_PI;
    void setFeedback(float feedback) {
        this->feedback = feedback;
    }
    float getFeedback() const {
        return feedback;
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
    void generate(ComplexFloatArray output, FloatArray fm) override {
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


/**
 * An oscillator similar to QuadratureSineOscillator class that also includes
 * feedback control. Feedback value is a complex number, this allows controlling
 * feedback direction.
 **/
class ComplexFeedbackQuadratureSineOscillator : public ComplexOscillatorTemplate<ComplexFeedbackQuadratureSineOscillator> {
public:
    static constexpr float begin_phase = 0;
    static constexpr float end_phase = 2 * M_PI;
    void setFeedback(ComplexFloat feedback) {
        this->feedback = feedback;
    }
    ComplexFloat getFeedback() const {
        return feedback;
    }
    ComplexFloat getSample() {
        last_sample = ComplexFloat(
            cosf(phase + last_sample.re * feedback.re), sinf(phase + last_sample.im * feedback.im));
        return last_sample;
    }
    void generate(ComplexFloatArray output) override {
        size_t len = output.getSize();
        for (size_t i = 0; i < len; ++i) {
            output[i].re = cosf(phase + last_sample.re * feedback.re);
            output[i].im = sinf(phase + last_sample.im * feedback.im);
            last_sample = output[i];
            phase += incr; // allow phase to overrun
        }
        phase = fmodf(phase, end_phase);
    }
    void generate(ComplexFloatArray output, FloatArray fm) override {
        size_t len = output.getSize();
        for (size_t i = 0; i < len; ++i) {
            output[i].re = cosf(phase + last_sample.re * feedback.re);
            output[i].im = sinf(phase + last_sample.im * feedback.im);
            last_sample = output[i];
            phase += incr * (1 + fm[i]);
            // allow phase to overrun
        }
        phase = fmodf(phase, end_phase);
    }
    using ComplexOscillatorTemplate<ComplexFeedbackQuadratureSineOscillator>::generate;
protected:
    ComplexFloat feedback = 0;
    ComplexFloat last_sample = 0;
};

#endif /* QUADRATURE_SINE_OSCILLATOR_H */
