#ifndef __ComplexFloatArrayTestPatch_hpp__
#define __ComplexFloatArrayTestPatch_hpp__

#include "StompBox.h"

class ComplexFloatArrayTestPatch : public Patch {
private:
  bool success;
  int passed;
  int failed;
  bool assert(bool condition,const char * message){
    assert(condition, message, -1);
  }
  bool assert(bool condition,const char * message, int n){
    if(condition==false && success==true){ //only hit this the first time, so only the first failed assertion gets displayed
      success=false;
      if(n>=0)
        debugMessage(message, n);
      else
        debugMessage(message);
      ASSERT(condition, message); //this actually the program to exit and the LED to turn red
      //but if you comment out the line above you can get nice acoustic feedback instead
      failed++;
    } else {
      passed++;
    }
    return condition;
  }
public:
  ComplexFloatArrayTestPatch(){
    success=true;
    passed=0;
    failed=0;
    int sz=100;
    ComplexFloat data[sz];
    ComplexFloat backupData[sz];
    for(int n=0; n<sz; n++){ //initialize array
      float value= n<sz/2 ? n : sz-n; 
      data[n].re=backupData[n].re=value;
      data[n].im=backupData[n].im=-value;
    }
    ComplexFloatArray cfa(data,sz);
    ComplexFloatArray backupcf(backupData,sz);
    ComplexFloat tempcData[sz]; 
    ComplexFloatArray tempc(tempcData,sz);
    ComplexFloat tempc2Data[sz]; 
    ComplexFloatArray tempc2(tempc2Data,sz);
    float tempfData[sz]; 
    FloatArray tempf(tempfData,sz);
        
    assert(cfa.getSize()==sz,"ComplexFloatArray.getSize()");
    for(int n=0; n<sz; n++){ 
      //checking cast and indexing operators
      assert(cfa[n].re==data[n].re,"ComplexFloat& operator []");
      assert(cfa[n].im==data[n].im,"ComplexFloat& operator []");
      assert((ComplexFloat*)cfa==data,"(ComplexFloat*)cfa==data");
      assert((float*)cfa==(float *)data,"(ComplexFloat*)cfa==data");
      assert(&cfa[n]==&data[n],"cfa[n]==data[n]");
      
      //check that indexing works properly overlaying struct and float
      float *f=(float*)data;
      assert(data[n].re==f[2*n],"data[n].re==f[2*n]");
      assert(data[n].im==f[2*n+1],"data[n].im==f[2*n+1]");
      //checking scalar complex math functions
      assert(cfa.mag(n)==sqrtf(f[2*n]*f[2*n]+f[2*n+1]*f[2*n+1]),"mag");
      assert(cfa.mag2(n)==f[2*n]*f[2*n]+f[2*n+1]*f[2*n+1],"mag2");
    }
    cfa.getMagnitudeValues(tempf);
    for(int n=0; n<sz; n++){
      assert(tempf[n]==sqrtf(cfa[n].re*cfa[n].re+cfa[n].im*cfa[n].im),"getMagnitudeValues", n);
    }
    cfa.getMagnitudeSquaredValues(tempf);
    for(int n=0; n<sz; n++){
      assert(tempf[n]==cfa[n].re*cfa[n].re+cfa[n].im*cfa[n].im,"getMagnitudeSquaredValues");
    }
    cfa.getComplexConjugateValues(tempc);
    for(int n=0; n<sz; n++){
      assert(tempc[n].re==cfa[n].re,"getComplexConjugateValues-real");
      assert(tempc[n].im==-cfa[n].im,"getComplexConjugateValues-imag");
    }
    cfa.complexByComplexMultiplication(tempc, tempc2);
    for(int n=0; n<sz; n++){
      assert(tempc2[n].re==cfa[n].re*tempc[n].re - cfa[n].im*tempc[n].im,"complexByComplexMultiplication-real");
      assert(tempc2[n].im==cfa[n].re*tempc[n].im + cfa[n].im*tempc[n].re,"complexByComplexMultiplication-imag");
    }
    cfa.complexByRealMultiplication(tempf, tempc2);
    for(int n=0; n<sz; n++){
      assert(tempc2[n].re==cfa[n].re*tempf[n],"complexByRealMultiplication-real");
      assert(tempc2[n].im==cfa[n].im*tempf[n],"complexByRealMultiplication-imag");
    }
    
  //ComplexDotProduct
    ComplexFloat tempcf;
    for(int n=0; n<sz; n++){
      tempcData[n].re=backupData[n].re=n*2;
      tempcData[n].im=backupData[n].im=-(n*2+1);
    }
    cfa.complexDotProduct(tempc, tempcf);
    float realResult=0;
    float imagResult=0;
    for(int n=0; n<sz; n++) {    
      realResult += cfa[n].re*tempc[n].re - cfa[n].im*tempc[n].im;
      imagResult += cfa[n].re*tempc[n].im + cfa[n].im*tempc[n].re;
    }
    assert(realResult==tempcf.re,"duct-re");
    assert(imagResult==tempcf.im,"duct-im");

    float maxMagVal=-1;
    int maxMagInd=-1;
    for(int n=0; n<sz; n++){
      if(cfa.mag(n)>maxMagVal){
        maxMagVal=cfa.mag(n);
        maxMagInd=n;
      }
    }
    assert(maxMagVal==cfa.getMaxMagnitudeValue(),"getMaxMagnitudeValue()");
    assert(maxMagInd==cfa.getMaxMagnitudeIndex(),"getMaxMagnitudeIndex()");
    
    cfa.getRealValues(tempf);
    for(int n=0; n<sz; n++){
      assert(tempf[n]==cfa[n].re,"getRealValues");
    }
    cfa.getImaginaryValues(tempf);
    for(int n=0; n<sz; n++){
      assert(tempf[n]==cfa[n].im,"getImaginaryValues");
    }
/*
    ComplexFloatArray subarray(int offset, int length);
*/
    if(success==true){
      debugMessage("Tests passed",passed);
    }
  };
  void processAudio(AudioBuffer &buffer){
    float *sig=buffer.getSamples(0);
    if(success==false){
      for(int n=0; n<getBlockSize(); n++){
        sig[n]+=0.05*rand()/(float)RAND_MAX;
      }
    } else {
      static float phase=0;
      float inc=2*M_PI/200.0f;
      for(int n=0; n<getBlockSize(); n++){
        sig[n]+=0.2*sinf(phase);
        phase+=inc;
        phase= phase>2*M_PI ? phase-2*M_PI : phase;
      }
    }
  }
};

#endif // __ComplexFloatArrayTestPatch_hpp__
