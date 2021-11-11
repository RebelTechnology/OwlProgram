#ifndef __FFT_PROCESSOR_H__
#define __FFT_PROCESSOR_H__

#include "SignalProcessor.h"
#include "Window.h"

template <class FrequencyDomainProcessor, size_t fft_size, size_t window_size>
class FFTProcessor : public SignalProcessor {
public:
    FFTProcessor(FastFourierTransform* fft, FrequencyDomainProcessor* processor,
        FloatArray timedomain, ComplexFloatArray freqdomain, FloatArray in_overlap,
        FloatArray out_overlap, Window in_win, Window out_win)
        : fft(fft)
        , index(0)
        , timedomain(timedomain)
        , freqdomain(freqdomain)
        , in_overlap(in_overlap)
        , out_overlap(out_overlap)
        , in_win(in_win)
        , out_win(out_win)
        , processor(processor) {
    }
    FrequencyDomainProcessor* getFrequencyDomainProcessor() {
        return processor;
    }
    /**
     * We don't support sample based processing in this class
     **/
    float process(float input) override {
        return 0.0;
    }
    void process(FloatArray input, FloatArray output) override{
        const size_t block_size = input.getSize();

        // Insert twice to prepare contiguous copy of data when we reach buffer end
        in_overlap.insert(input, index, block_size);
        in_overlap.insert(input, (index + window_size) % (window_size * 2), block_size);

        // When end is reach, we rewind to middle of the buffer
        if (index >= window_size * 2 - block_size) {
            index = window_size;
        }
        else {
            index += block_size;
        }

        // Process data only when there's enough input to fill first window
        if (index >= window_size) {
            // FFT
            in_win.apply(in_overlap.getData() + index - window_size, timedomain);
            fft->fft(timedomain, freqdomain);
            ComplexFloatArray bins = freqdomain.subArray(0, fft_size / 2);
            // Time domain processing
            processor->process(bins, bins);
            // IFFT
            fft->ifft(freqdomain, timedomain);
            out_win.apply(timedomain);
            // Overlap accumulation
            const size_t out_index = index - window_size;
            output.copyFrom(timedomain.subArray(0, block_size));
            FloatArray t = out_overlap.subArray(out_index, block_size);
            output.add(t);
            t.setAll(0);
            out_overlap
                .subArray(out_index + block_size, window_size - out_index - block_size)
                .add(timedomain.subArray(
                    block_size, window_size - out_index - block_size));
            out_overlap.subArray(0, out_index)
                .add(timedomain.subArray(window_size - out_index, out_index));
        }
    }
    template <typename... Args>
    static FFTProcessor* create(size_t block_size,
        Window::WindowType in_win_type = Window::HannWindow,
        Window::WindowType out_win_type = Window::HannWindow, Args&&... args) {
        FrequencyDomainProcessor* processor =
            FrequencyDomainProcessor::create(std::forward<Args>(args)...);
        Window in_win = Window::create(in_win_type, window_size);
        Window out_win = Window::create(out_win_type, window_size);

        float norm[block_size] = {};

        // Accumulate shifted multiplications of both windows as norm
        for (size_t i = 0; i < window_size; i++) {
            norm[i % block_size] += in_win[i] * out_win[i];
        }

        // Divide output window by the norm for every block size, compensating amplitude changes
        float* t = out_win.getData();
        for (size_t i = 0; i < window_size / block_size; i++) {
            for (size_t j = 0; j < block_size; j++) {
                *t++ /= norm[j];
            }
        }

        return new FFTProcessor(FastFourierTransform::create(fft_size), processor,
            FloatArray::create(fft_size), ComplexFloatArray::create(fft_size),
            FloatArray::create(window_size * 2),
            FloatArray::create(window_size), in_win, out_win);
    }
    static void destroy(FFTProcessor* fft) {
        FloatArray::destroy(fft->timedomain);
        ComplexFloatArray::destroy(fft->freqdomain);
        FloatArray::destroy(fft->in_overlap);
        FloatArray::destroy(fft->out_overlap);
        Window::destroy(fft->in_win);
        Window::destroy(fft->out_win);
        FrequencyDomainProcessor::destroy(fft->processor);
        FastFourierTransform::destroy(fft->fft);
        delete fft;
    }

protected:
    size_t index;
    FastFourierTransform* fft;
    FloatArray timedomain;
    ComplexFloatArray freqdomain;
    Window in_win, out_win;
    FloatArray in_overlap, out_overlap;
    FrequencyDomainProcessor* processor;
};

#endif
