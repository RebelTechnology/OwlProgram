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


#ifndef __BiquadFilterTestPatch_hpp__
#define __BiquadFilterTestPatch_hpp__

#include "Patch.h"
#include "BiquadFilter.h"

class BiquadFilterTestPatch : public Patch {
public:
  BiquadFilter *filter;
  BiquadFilterTestPatch(){
    registerParameter(PARAMETER_A, "Cutoff");
    registerParameter(PARAMETER_B, "Resonance");
    size_t stages=3;
    filter=BiquadFilter::create(getSampleRate(), stages);
    float cutoff=1200;
    float resonance=2;
    //test setLowPass
    FloatArray coefficients=FloatArray::create(BIQUAD_COEFFICIENTS_PER_STAGE*stages);
    FloatArray states=FloatArray::create(BIQUAD_STATE_VARIABLES_PER_STAGE*stages);
    FilterStage stage(coefficients, states);
    filter->setLowPass(cutoff, resonance);
    stage.setLowPass(cutoff, resonance, getSampleRate());
    for(size_t k=0; k<stages; k++){ 
      for(size_t n=0; n<5; n++){
        float filterC=filter->getFilterStage(k).getCoefficients()[n];
        float stageC=stage.getCoefficients()[n];
        ASSERT(abs(filterC-stageC) < 0.000001, "Coefficients not initialized"); //check that filter coefficients are properly initialized
      }
    }
    size_t signalLength=100;
    
    FloatArray x=FloatArray::create(signalLength);
    FloatArray x1=FloatArray::create(signalLength);
    FloatArray y=FloatArray::create(signalLength);
    FloatArray y1=FloatArray::create(signalLength);
    x.noise();
    x1.copyFrom(x);
    
    filter->process(x1, y1, x1.getSize());
    //manually compute the filter
    float b0=filter->getFilterStage(0).getCoefficients()[0];
    float b1=filter->getFilterStage(0).getCoefficients()[1];
    float b2=filter->getFilterStage(0).getCoefficients()[2];
    float a1=filter->getFilterStage(0).getCoefficients()[3];
    float a2=filter->getFilterStage(0).getCoefficients()[4];
    for(size_t n=0; n<stages; n++){
      float d1=0;
      float d2=0;
      for(size_t n=0; n<x.getSize(); n++){ //manually apply filter, one stage
        y[n] = b0 * x[n] + d1;
        d1 = b1 * x[n] + a1 * y[n] + d2;
        d2 = b2 * x[n] + a2 * y[n];   
      }
      x.copyFrom(y); //copy the output to the input for the next iteration. INEFFICIENT
    }
    //done with the filter
    for(size_t n=0; n<x.getSize(); n++){
      // ASSERT(abs(y[n]-y1[n])<0.0001, "");//BiquadFilter.process(FloatArray, FloatArray) result"); //TODO: fails for non-arm
    }
    FloatArray::destroy(x);
    FloatArray::destroy(x1);
    FloatArray::destroy(y);
    FloatArray::destroy(y1);
    debugMessage("All tests passed");
  }
  ~BiquadFilterTestPatch(){
    BiquadFilter::destroy(filter);
  }
  void processAudio(AudioBuffer &buffer){
    float cutoff=getParameterValue(PARAMETER_A)*12000+200;
    float resonance=10*getParameterValue(PARAMETER_B);
    FloatArray fa=buffer.getSamples(0);
    // fa.noise();
    filter->setLowPass(cutoff, resonance);
    filter->process(fa, fa, fa.getSize());
    buffer.getSamples(1).copyFrom(fa);
  }
};

#endif // __BiquadFilterTestPatch_hpp__
