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


/* created by the OWL team 2015 */


////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __SuperSawTestPatch_hpp__
#define __SuperSawTestPatch_hpp__

#include "StompBox.h"

class SuperSawTestPatch : public Patch {
public:
  SuperSaw ss;
  SuperSawTestPatch(){
    ss.setSampleRate(getSampleRate());
  }
  void processAudio(AudioBuffer &buffer){
    float gain=getParameterValue(PARAMETER_A);
    float frequency=powf(2,5*getParameterValue(PARAMETER_B))*100;
    float detune=getParameterValue(PARAMETER_C);
    float mix=getParameterValue(PARAMETER_D);
    FloatArray fa=buffer.getSamples(0);
    ss.setMix(mix);
    ss.setFrequency(frequency);
    ss.setDetune(detune);
    ss.getSamples(fa);
    // ss.setNumOscillators(gain*14+1); //just for fun
    fa.scale(gain);
  }
};

#endif // __SuperSawTestPatch_hpp__
