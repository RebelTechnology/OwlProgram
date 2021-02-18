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

#include "Patch.h"
#include "Resample.h"

class ResampleTestPatch : public Patch {
public:
  Resampler resampler;
  FloatArray up;
  FloatArray temp;
  int factor;
  ResampleTestPatch(){
    registerParameter(PARAMETER_A, "do_resample");
    registerParameter(PARAMETER_B, "gain");
    registerParameter(PARAMETER_C, "master");
    factor=4;
    int blockSize=getBlockSize();
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
    resampler.upsample(samples,up);
    
    resampler.downsample(up, samples);
    // samples.multiply(getParameterValue(PARAMETER_B));
  }
};

#endif // __ResampleTestPatch_hpp__
