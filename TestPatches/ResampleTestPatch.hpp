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


#ifndef __ResampleTestPatch_hpp__
#define __ResampleTestPatch_hpp__

#include "StompBox.h"

class ResampleTestPatch : public Patch {
public:
  Upsample upsample;
  Downsample downsample;
  FloatArray up;
  FloatArray temp;
  int factor;
  ResampleTestPatch(){
    registerParameter(PARAMETER_A, "do_resample");
    registerParameter(PARAMETER_B, "gain");
    registerParameter(PARAMETER_C, "master");
    factor=4;
    int blockSize=getBlockSize();
    // upsample.init();
    // downsample.init();
    up=FloatArray::create(factor*blockSize);
  }
  ~ResampleTestPatch(){
    FloatArray::destroy(up);
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray samples=buffer.getSamples(0);
    if(getParameterValue(PARAMETER_A)<0.5){
      // debugMessage("oversample",downsample.getCoefficients()[3]);
      return;
    }
    // debugMessage("oversample",downsample.getCoefficients()[0],downsample.getCoefficients()[1],downsample.getCoefficients()[2]);
    upsample.up(samples,up);
    
    // debugMessage("up max", up.getMaxValue());
    // debugMessage("up ", downsample.getCoefficients(1)[1], downsample.getCoefficients(1)[2], downsample.getCoefficients(1)[4]);
    // for(int n=0; n<buffer.getSize(); n++){
      // for(int k=0; k<factor; k++){
        // up[n*factor+k]=samples[n];
      // }
    // }
    downsample.down(up, samples);
    for(int n=0; n<buffer.getSize(); n++){
      samples[n]=up[n*factor]/factor;
    }
    samples.clip();
    samples.multiply(getParameterValue(PARAMETER_B));
  }
};

#endif // __ResampleTestPatch_hpp__
