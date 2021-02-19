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


/* created by the OWL team 2013 */


////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __ZeroCrossingPitchDetectorTestPatch_hpp__
#define __ZeroCrossingPitchDetectorTestPatch_hpp__

#include "StompBox.h"
#include "PitchDetector.h"

class ZeroCrossingPitchDetectorTestPatch : public Patch {
public:
  ZeroCrossingPitchDetector zcc;
  ZeroCrossingPitchDetectorTestPatch() : zcc(getSampleRate(), getBlockSize()){
    registerParameter(PARAMETER_A, "Mix");
    registerParameter(PARAMETER_B, "HP cutoff");
    registerParameter(PARAMETER_C, "LP cutoff");
    registerParameter(PARAMETER_D, "gain");
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray fa=buffer.getSamples(0);
    float mix=getParameterValue(PARAMETER_A);
    zcc.setHighPassCutoff(getParameterValue(PARAMETER_B)*15000+150);
    zcc.setLowPassCutoff(getParameterValue(PARAMETER_C)*500+50);
    zcc.process(fa);
    float frequency=zcc.getFrequency();
    float envelope=fa.getRms();
    fa.multiply(1-mix);
    for(size_t n=0;n<fa.getSize(); n++){
      static float phase=0;
      static float pastEnvelope=0;
      phase += 2.0 * M_PI * frequency/getSampleRate();
      if(phase > 2.0 * M_PI)
        phase -= 2.0 * M_PI;
      if(phase > 4.0*M_PI)
        phase=0;
      envelope=0.1*envelope + pastEnvelope*0.9;
      pastEnvelope=envelope;
      fa[n]+=sin(phase)*mix*envelope;
    }
    fa.multiply(getParameterValue(PARAMETER_D)*10);
    fa.copyTo(buffer.getSamples(1));
    debugMessage("frequency/envelope: ", frequency, envelope);
    // float *coeffs=zcc.getFilter()->getFilterStage(0).getCoefficients();
    // debugMessage("coeffs: ", coeffs[3], coeffs[4], coeffs[2] );
  }
};

#endif // __ZeroCrossingPitchDetectorTestPatch_hpp__
