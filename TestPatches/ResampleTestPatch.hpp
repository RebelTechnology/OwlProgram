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
  Resample resample;
  FloatArray up;
  FloatArray temp;
  int factor;
  int numTaps;
  ResampleTestPatch(){
    registerParameter(PARAMETER_A, "do_resample");
    registerParameter(PARAMETER_B, "gain");
    registerParameter(PARAMETER_C, "master");
    factor=4;
    numTaps=8;
    int blockSize=getBlockSize();
    resample.init(factor, numTaps, blockSize);
    up=FloatArray::create(factor*blockSize);
    temp=FloatArray::create(blockSize);
  }
  ~ResampleTestPatch(){
    FloatArray::destroy(up);
  }
  void processAudio(AudioBuffer &buffer){
    if(getParameterValue(PARAMETER_A)<0.5){
      return;
    }
    FloatArray samples=buffer.getSamples(0);
    resample.up(samples,up);
    // for(int n=0; n<getBlockSize(); n++){ //upsample withour filtering 
      // for(int k=0; k<factor; k++)
        // up[n]=samples[n+k];
    // }
    // resample.down(up, samples);
    for(int n=0; n<getBlockSize(); n++){ //downsample without filtering
        samples[n]=up[n*factor];
    }
    // samples.scale(getParameterValue(PARAMETER_B)*0.00000000000000000001);
    samples.scale(getParameterValue(PARAMETER_B)*0.00001);
    for(int n=0; n<getBlockSize(); n++){
      if(samples[n]>1)
        samples[n]=1;
      if(samples[n]<-1)
        samples[n]=-1;
    }
    samples.scale(getParameterValue(PARAMETER_C));
  }
};

#endif // __ResampleTestPatch_hpp__
