#ifndef __DaisySPTestPatch_hpp__
#define __DaisySPTestPatch_hpp__

#include "OpenWareLibrary.h"
#include "daisysp.h"

class DaisySPTestPatch : public Patch {
public:
    daisysp::Oscillator* osc;

    DaisySPTestPatch() {
        osc = new daisysp::Oscillator();
        osc->Init(getSampleRate());
        osc->SetFreq(100);
    }
    ~DaisySPTestPatch() {
        delete osc;
    }
    void processAudio(AudioBuffer& buffer) {
        FloatArray chan = buffer.getSamples(0);
        for (int i = 0; i < chan.getSize(); i++) {
            chan[i] = osc->Process();
        }
    }
};

#endif // __DaisySPTestPatch_hpp__
