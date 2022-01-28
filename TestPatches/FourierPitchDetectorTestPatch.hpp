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


#ifndef __FourierPitchDetectorTestPatch_hpp__
#define __FourierPitchDetectorTestPatch_hpp__

#include "Patch.h"
#include "PitchDetector.h"

class FourierPitchDetectorTestPatch : public Patch {
public:
  FourierPitchDetector fpd;
  FloatArray aux;
  int fftOccurrency;
  FourierPitchDetectorTestPatch() {
    registerParameter(PARAMETER_A, "Mix");
    registerParameter(PARAMETER_B, "HP cutoff");
    registerParameter(PARAMETER_C, "LP cutoff");
    registerParameter(PARAMETER_D, "gain");
    int fftSize=512;
    fpd.init(fftSize, getSampleRate());
    fftOccurrency=fftSize/getBlockSize();
    aux=FloatArray::create(getBlockSize());
  }
  void processAudio(AudioBuffer &buffer){
    static int count=0;
    static int prevRet=0;
    FloatArray fa=buffer.getSamples(0);
    float estimated=0;
    float minFrequency=getParameterValue(PARAMETER_B)*500+50;
    float maxFrequency=getParameterValue(PARAMETER_C)*1000+150;
    
    fpd.setMinFrequency(minFrequency);
    fpd.setMaxFrequency(maxFrequency);
    int ret=fpd.process(fa);
    //if in the previous block an fft took place
    //update the frequency detector
    if(prevRet==1 || fftOccurrency==1){  
      fpd.computeFrequency(); 
    }
    prevRet=ret;

    estimated=fpd.getFrequency();
    ASSERT(ret==((fftOccurrency-1)==(count&(fftOccurrency-1))), "unexpected FFT");
    //synthesize a tone and mix it with the input sound;
    float mix=getParameterValue(PARAMETER_A);
    float envelope=fa.getRms();
    fa.multiply(1-mix);
    for(size_t n=0;n<fa.getSize(); n++){
      static float phase=0;
      static float pastEnvelope=0;
      phase += 2.0 * M_PI * estimated/getSampleRate();
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
    
    count++;
    static float otherTime=0;
    if(ret==1){ //show that the elapsedBlockTime is much larger in blocks with fft
      debugMessage("estimated, elapsedBlockTime, otherTime", estimated, getElapsedBlockTime()*100, otherTime);
    } else  //keep track of the elapsedBlockTime in blocks without fft
      otherTime=getElapsedBlockTime()*100;
  }
};

#endif // __FourierPitchDetectorTestPatch_hpp__
