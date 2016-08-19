////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
 
 LICENSE:
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */


/* created by the OWL team 2016 */


////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __OscillatorTestPatch_hpp__
#define __OscillatorTestPatch_hpp__

#include "Patch.h"
#include "SineOscillator.hpp"

class OscillatorTestPatch : public Patch {
public:
  SineOscillator osc;
  SineOscillator lfo;
  SineOscillator fm;
  OscillatorTestPatch():
	osc(), lfo(getBlockSize()), fm() 
  {
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "LFO rate");
    registerParameter(PARAMETER_C, "FM freq");
    registerParameter(PARAMETER_D, "FM width");
  }
  ~OscillatorTestPatch(){
  }
  void processAudio(AudioBuffer &buffer){
    float frequency = getParameterValue(PARAMETER_A) * 500 + 100;
    float parameterB = getParameterValue(PARAMETER_B);
    float fmFreq = getParameterValue(PARAMETER_C) * 1000;
    float fmWidth = getParameterValue(PARAMETER_D) * 1;

    FloatArray fa=buffer.getSamples(0);
    FloatArray fb=buffer.getSamples(1);
    float lfoValue;
    // make the lfo knob inactive when close to 0
    if (parameterB < 0.05){
      lfoValue = 1;
    } else {
      lfo.setPeriod(1 - (parameterB * 0.9 + 0.1));
      lfoValue = lfo.getNextSample();
    }
    // smooth the LFO value
    for(int n = 0; n < fb.getSize(); ++n){
      static float oldLfoValue = 0;
      fb[n] = oldLfoValue * 0.99 + lfoValue * 0.01;
      oldLfoValue = fb[n];
    }
    fm.setFrequency(fmFreq);
    fm.getSamples(fa);
    fa.multiply(fmWidth);
    osc.setFrequency(frequency);
    osc.getSamples(fa, fa); // the carrier oscillator is modulated
    fa.multiply(fb);
    buffer.getSamples(1).copyFrom(fa);
  }
};

#endif // __OscillatorTestPatch_hpp__
