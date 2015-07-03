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
      ASSERT(condition, message); //this actually causes the program to exit and the LED to turn red
      //but if you comment out the line above you can get nice acoustic feedback instead (sinewave for success, noise for error)
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
    int size=100;
    ComplexFloat data[size];
    ComplexFloat backupData[size];
    for(int n=0; n<size; n++){ //initialize array
      float value= n<size/2 ? n : size-n; 
      data[n].re=backupData[n].re=value;
      data[n].im=backupData[n].im=-value;
    }
    ComplexFloatArray cfa(data,size);
    ComplexFloatArray backupcf(backupData,size);
    ComplexFloat tempcData[size]; 
    ComplexFloatArray tempc(tempcData,size);
    ComplexFloat tempc2Data[size]; 
    ComplexFloatArray tempc2(tempc2Data,size);
    float tempfData[size]; 
    FloatArray tempf(tempfData,size);
        
    assert(cfa.getSize()==size,"ComplexFloatArray.getSize()");
    for(int n=0; n<size; n++){ 
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
    for(int n=0; n<size; n++){
      assert(tempf[n]==sqrtf(cfa[n].re*cfa[n].re+cfa[n].im*cfa[n].im),"getMagnitudeValues", n);
    }
    cfa.getMagnitudeSquaredValues(tempf);
    for(int n=0; n<size; n++){
      assert(tempf[n]==cfa[n].re*cfa[n].re+cfa[n].im*cfa[n].im,"getMagnitudeSquaredValues");
    }
    cfa.getComplexConjugateValues(tempc);
    for(int n=0; n<size; n++){
      assert(tempc[n].re==cfa[n].re,"getComplexConjugateValues-real");
      assert(tempc[n].im==-cfa[n].im,"getComplexConjugateValues-imag");
    }
    cfa.complexByComplexMultiplication(tempc, tempc2);
    for(int n=0; n<size; n++){
      assert(tempc2[n].re==cfa[n].re*tempc[n].re - cfa[n].im*tempc[n].im,"complexByComplexMultiplication-real");
      assert(tempc2[n].im==cfa[n].re*tempc[n].im + cfa[n].im*tempc[n].re,"complexByComplexMultiplication-imag");
    }
    cfa.complexByRealMultiplication(tempf, tempc2);
    for(int n=0; n<size; n++){
      assert(tempc2[n].re==cfa[n].re*tempf[n],"complexByRealMultiplication-real");
      assert(tempc2[n].im==cfa[n].im*tempf[n],"complexByRealMultiplication-imag");
    }
    
  //ComplexDotProduct
    ComplexFloat tempcf;
    for(int n=0; n<size; n++){
      tempcData[n].re=backupData[n].re=n*2;
      tempcData[n].im=backupData[n].im=-(n*2+1);
    }
    cfa.complexDotProduct(tempc, tempcf);
    float realResult=0;
    float imagResult=0;
    for(int n=0; n<size; n++) {    
      realResult += cfa[n].re*tempc[n].re - cfa[n].im*tempc[n].im;
      imagResult += cfa[n].re*tempc[n].im + cfa[n].im*tempc[n].re;
    }
    assert(realResult==tempcf.re,"duct-re");
    assert(imagResult==tempcf.im,"duct-im");

    float maxMagVal=-1;
    int maxMagInd=-1;
    for(int n=0; n<size; n++){
      if(cfa.mag(n)>maxMagVal){
        maxMagVal=cfa.mag(n);
        maxMagInd=n;
      }
    }
    assert(maxMagVal==cfa.getMaxMagnitudeValue(),"getMaxMagnitudeValue()");
    assert(maxMagInd==cfa.getMaxMagnitudeIndex(),"getMaxMagnitudeIndex()");
    
    cfa.getRealValues(tempf);
    for(int n=0; n<size; n++){
      assert(tempf[n]==cfa[n].re,"getRealValues");
    }
    cfa.getImaginaryValues(tempf);
    for(int n=0; n<size; n++){
      assert(tempf[n]==cfa[n].im,"getImaginaryValues");
    }
    
    //test setAll
    // test setAll(float)
    {
      for(int n=0; n<size; n++){
        tempc[n].re=1;
        tempc[n].im=1;
      }
      float allValue=0.1;
      tempc.setAll(allValue);
      for(int n=0; n<size; n++){
        assert(tempc[n].re==allValue, "setAll(float) real");
        assert(tempc[n].im==allValue, "setAll(float) imag");
      }
    }
      // test setAll(float, float)
    {
      float allValueRe=0.3;
      float allValueIm=-0.3;
      tempc.setAll(allValueRe, allValueIm);
      for(int n=0; n<size; n++){
        assert(tempc[n].re==allValueRe, "setAll(float, float) real");
        assert(tempc[n].im==allValueIm, "setAll(float, float) imag");
      }
    }
      // test setAll(ComplexFloat)
    {
      ComplexFloat allValue;
      allValue.re=0.4;
      allValue.im=-0.1;
      tempc.setAll(allValue);
      for(int n=0; n<size; n++){
        assert(tempc[n].re==allValue.re, "setAll(ComplexFloat) real");
        assert(tempc[n].im==allValue.im, "setAll(ComplexFloat) imag");
      }
    }
    
    //test copyFrom
    //test copyFrom(FloatArray)
    tempc.setAll(0);
    tempc.copyFrom(tempf);
    for(int n=0; n<size; n++){
      assert(tempf[n]==tempc[n].re, "copyFrom(FloatArray)");
    }
    //test copyFrom(ComplexFloatArray)
    tempc.setAll(0);
    tempc.copyFrom(cfa);
    for(int n=0; n<size; n++){
      assert(cfa[n].re==tempc[n].re, "copyFrom(ComplexFloatArray), real");
      assert(cfa[n].im==tempc[n].im, "copyFrom(ComplexFloatArray), imag");
    }
    //test copyFrom(ComplexFloat *, int)
    tempc.setAll(0);
    tempc.copyFrom(cfa.getData(), cfa.getSize());
    for(int n=0; n<size; n++){
      assert(cfa[n].re==tempc[n].re, "copyFrom(ComplexFloat*, int), real");
      assert(cfa[n].im==tempc[n].im, "copyFrom(ComplexFloat*, int), imag");
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
