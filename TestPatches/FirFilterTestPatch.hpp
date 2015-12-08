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


#ifndef __FirFilterTestPatch_hpp__
#define __FirFilterTestPatch_hpp__

#include "StompBox.h"
#include "FirFilter.h"
class FirFilterTestPatch : public Patch {
public:
  FirFilter *filter;
  int numTaps=128;
  FloatArray temp;
  FloatArray coefficients;
  FirFilterTestPatch(){
    filter=FirFilter::create(numTaps, getBlockSize());
    coefficients=*(filter->getCoefficients());
    #include "FirInit.h" //initializes coefficients
    temp=FloatArray::create(getBlockSize());
    registerParameter(PARAMETER_A, "Gain");
  }
  ~FirFilterTestPatch(){
    FloatArray::destroy(temp);
  }
  void processAudio(AudioBuffer &buffer){
    float gain=getParameterValue(PARAMETER_A);
    FloatArray fa=buffer.getSamples(0);
    fa.noise();
    filter->processBlock(fa, temp);
    fa.copyFrom(temp);
    debugMessage("max value",fa.getMaxValue());
  }
};

#endif // __FirFilterTestPatch_hpp__
