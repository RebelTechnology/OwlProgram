#ifndef __ComplexFloatArrayTestPatch_hpp__
#define __ComplexFloatArrayTestPatch_hpp__

#include "StompBox.h"
#include "ComplexFloatArray.h"

class ComplexFloatArrayTestPatch : public Patch {
private:
  bool success;
  int passed;
  int failed;
  bool assertt(float first, float second, const char *message, float tolerance){ //assert with tolerance
    bool cond=false;
    float tol=tolerance;
    if(abs(second-first)<tol){
      cond=true;
    }
    return assertr(cond, message);
  }
  bool assertt(float first, float second, const char *message){ //assert with tolerance of 0.0000001
    return assertt(first, second, message,  0.0000001);
  }
  bool assertr(bool condition,const char * message){
    return assertr(condition, message, -1);
  }
  bool assertr(bool condition,const char * message, int n){
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
    debugMessage("start");
    passed=0;
    failed=0;
    size_t size=100;
    ComplexFloat data[size];
    ComplexFloat backupData[size];
    for(size_t n=0; n<size; n++){ //initialize array
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
        
    assertr(cfa.getSize()==size,"ComplexFloatArray.getSize()");
    for(size_t n=0; n<size; n++){ 
      //checking cast and indexing operators
      assertr(cfa[n].re==data[n].re,"ComplexFloat& operator []");
      assertr(cfa[n].im==data[n].im,"ComplexFloat& operator []");
      assertr((ComplexFloat*)cfa==data,"(ComplexFloat*)cfa==data");
      assertr((float*)cfa.getData()==(float *)data,"(ComplexFloat*)cfa==data");
      assertr(&cfa[n]==&data[n],"cfa[n]==data[n]");
      
      //check that indexing works properly overlaying struct and float
      float *f=(float*)data;
      assertr(data[n].re==f[2*n],"data[n].re==f[2*n]");
      assertr(data[n].im==f[2*n+1],"data[n].im==f[2*n+1]");
      //checking scalar complex math functions
      assertr(cfa.mag(n)==sqrtf(f[2*n]*f[2*n]+f[2*n+1]*f[2*n+1]),"mag");
      assertr(cfa.mag2(n)==f[2*n]*f[2*n]+f[2*n+1]*f[2*n+1],"mag2");
    }
    
    //test equals(ComplexFloatArray) method
    {
      tempc.copyFrom(cfa);
      assertr(cfa.equals(tempc), ".equals()");
      
    }
    cfa.getMagnitudeValues(tempf);
    for(size_t n=0; n<size; n++){
      assertr(tempf[n]==sqrtf(cfa[n].re*cfa[n].re+cfa[n].im*cfa[n].im),"getMagnitudeValues", n);
    }
    cfa.getMagnitudeSquaredValues(tempf);
    for(size_t n=0; n<size; n++){
      assertr(tempf[n]==cfa[n].re*cfa[n].re+cfa[n].im*cfa[n].im,"getMagnitudeSquaredValues");
    }
    cfa.getComplexConjugateValues(tempc);
    for(size_t n=0; n<size; n++){
      assertr(tempc[n].re==cfa[n].re,"getComplexConjugateValues-real");
      assertr(tempc[n].im==-cfa[n].im,"getComplexConjugateValues-imag");
    }
    cfa.complexByComplexMultiplication(tempc, tempc2);
    for(size_t n=0; n<size; n++){
      assertr(tempc2[n].re==cfa[n].re*tempc[n].re - cfa[n].im*tempc[n].im,"complexByComplexMultiplication-real");
      assertr(tempc2[n].im==cfa[n].re*tempc[n].im + cfa[n].im*tempc[n].re,"complexByComplexMultiplication-imag");
    }
    cfa.complexByRealMultiplication(tempf, tempc2);
    for(size_t n=0; n<size; n++){
      assertr(tempc2[n].re==cfa[n].re*tempf[n],"complexByRealMultiplication-real");
      assertr(tempc2[n].im==cfa[n].im*tempf[n],"complexByRealMultiplication-imag");
    }
    
  //ComplexDotProduct
    ComplexFloat tempcf;
    for(size_t n=0; n<size; n++){
      tempcData[n].re=backupData[n].re=n*2;
      tempcData[n].im=backupData[n].im=-(n*2+1);
    }
    cfa.complexDotProduct(tempc, tempcf);
    float realResult=0;
    float imagResult=0;
    for(size_t n=0; n<size; n++) {    
      realResult += cfa[n].re*tempc[n].re - cfa[n].im*tempc[n].im;
      imagResult += cfa[n].re*tempc[n].im + cfa[n].im*tempc[n].re;
    }
    assertr(realResult==tempcf.re,"duct-re");
    assertr(imagResult==tempcf.im,"duct-im");

    float maxMagVal=-1;
    int maxMagInd=-1;
    for(size_t n=0; n<size; n++){
      if(cfa.mag(n)>maxMagVal){
        maxMagVal=cfa.mag(n);
        maxMagInd=n;
      }
    }
    assertr(maxMagVal==cfa.getMaxMagnitudeValue(),"getMaxMagnitudeValue()");
    assertr(maxMagInd==cfa.getMaxMagnitudeIndex(),"getMaxMagnitudeIndex()");
    
    cfa.getRealValues(tempf);
    for(size_t n=0; n<size; n++){
      assertr(tempf[n]==cfa[n].re,"getRealValues");
    }
    cfa.getImaginaryValues(tempf);
    for(size_t n=0; n<size; n++){
      assertr(tempf[n]==cfa[n].im,"getImaginaryValues");
    }
    
    //test setAll
    // test setAll(float)
    {
      for(size_t n=0; n<size; n++){
        tempc[n].re=1;
        tempc[n].im=1;
      }
      float allValue=0.1;
      tempc.setAll(allValue);
      for(size_t n=0; n<size; n++){
        assertr(tempc[n].re==allValue, "setAll(float) real");
        assertr(tempc[n].im==allValue, "setAll(float) imag");
      }
    }
      // test setAll(float, float)
    {
      float allValueRe=0.3;
      float allValueIm=-0.3;
      tempc.setAll(allValueRe, allValueIm);
      for(size_t n=0; n<size; n++){
        assertr(tempc[n].re==allValueRe, "setAll(float, float) real");
        assertr(tempc[n].im==allValueIm, "setAll(float, float) imag");
      }
    }
      // test setAll(ComplexFloat)
    {
      ComplexFloat allValue;
      allValue.re=0.4;
      allValue.im=-0.1;
      tempc.setAll(allValue);
      for(size_t n=0; n<size; n++){
        assertr(tempc[n].re==allValue.re, "setAll(ComplexFloat) real");
        assertr(tempc[n].im==allValue.im, "setAll(ComplexFloat) imag");
      }
    }
    
    //test copyFrom
    //test copyFrom(FloatArray)
    tempc.setAll(0);
    tempc.copyFrom(tempf);
    for(size_t n=0; n<size; n++){
      assertr(tempf[n]==tempc[n].re, "copyFrom(FloatArray)");
    }
    //test copyFrom(ComplexFloatArray)
    tempc.setAll(0);
    tempc.copyFrom(cfa);
    for(size_t n=0; n<size; n++){
      assertr(cfa[n].re==tempc[n].re, "copyFrom(ComplexFloatArray), real");
      assertr(cfa[n].im==tempc[n].im, "copyFrom(ComplexFloatArray), imag");
    }
    //test copyFrom(ComplexFloat *, int)
    tempc.setAll(0);
    tempc.copyFrom(cfa.getData(), cfa.getSize());
    for(size_t n=0; n<size; n++){
      assertr(cfa[n].re==tempc[n].re, "copyFrom(ComplexFloat*, int), real");
      assertr(cfa[n].im==tempc[n].im, "copyFrom(ComplexFloat*, int), imag");
    }
    
      //test ComplexFloat methods

    {
      ASSERT(tempc.getSize()==size,"tempc");
      //test ComplexFloat.setPolar()
      ComplexFloat i;
      i.re=0;
      i.im=0;
      float magnitude=rand()/(float)RAND_MAX*10;
      float phase=rand()/(float)RAND_MAX*2*M_PI - M_PI;
      i.setPolar(magnitude, phase);
      assertr(i.re==magnitude*cosf(phase),"ComplexFloat setPolar() real");
      assertr(i.im==magnitude*sinf(phase),"ComplexFloat setPolar() imag");
      //test ComplexFloat.getPhase()
      assertt(i.getPhase(),phase, "ComplexFloat getPhase()");
      //test ComplexFloat.getMagnitude()
      // debugMessage("magnitude",i.getMagnitude(),magnitude, phase);
      assertt(i.getMagnitude(),magnitude, "ComplexFloat getMagnitude()", 0.0001);
    }
    {
      //test ComplexFloat.setMagnitude()
      ComplexFloat i;
      float magnitude=rand()/(float)RAND_MAX*10;
      float phase=rand()/(float)RAND_MAX*2*M_PI - M_PI;
      i.setPolar(magnitude,phase);
      float newMagnitude=magnitude+1;
      i.setMagnitude(newMagnitude);
      assertt(i.getMagnitude(), newMagnitude, "ComplexFloat setMagnitude() magnitude", 0.0001);
      assertt(i.getPhase(), phase, "ComplexFloat setMagnitude() phase", 0.0001); //check that the phase has not changed
    }
    {
      //test ComplexFloat.setPhase()
      ComplexFloat i;
      float magnitude=rand()/(float)RAND_MAX*10;
      float phase=rand()/(float)RAND_MAX*2*M_PI - M_PI;
      i.setPolar(magnitude,phase);
      float newPhase=phase+1;
      i.setPhase(newPhase);
      assertt(i.getPhase(), newPhase, "ComplexFloat setPhase() phase", 0.0001);
      assertt(i.getMagnitude(), magnitude, "ComplexFloat setPhase() magnitude", 0.0001); //check that the magnitude has not changed
    }
    {
      // test ComplexFloatArray.setPhase()
      tempc.copyFrom(cfa);
      FloatArray phase=FloatArray::create(size);
      phase.noise(-M_PI, M_PI);
      // setPhase(FloatArray)
      tempc.setPhase(phase);
      for(size_t n=0; n<size; n++){
        if(cfa[n].getMagnitude()<0.001)
          continue; //skip small values of magnitude which would make the phase noisy
        assertt(tempc[n].getPhase(), phase[n], "setPhase() phase", 0.01);
        assertt(tempc[n].getMagnitude(), cfa[n].getMagnitude(), "setPhase() magnitude", 0.01);
      }
      // setPhase(FloatArray, int, int)
      size_t offset=size/3;
      size_t count= size/2;
      tempc.copyFrom(cfa);
      tempc.setPhase(phase, offset, count);
      for(size_t n=0; n<size; n++){
        if(n>=offset && n<offset+count){
          if(cfa[n].getMagnitude()<0.001)
            continue; //skip small values of magnitude which would make the phase noisy
          assertt(tempc[n].getPhase(), phase[n], "setPhase() phase wrong", 0.01);
          assertt(tempc[n].getMagnitude(), cfa[n].getMagnitude(), "setPhase() magnitude wrong", 0.01);
        } else { //check that values outside the range have not changed
          assertr(tempc[n].getPhase()==cfa[n].getPhase(), "setPhase() phase changed", 0);
          assertr(tempc[n].getMagnitude()==cfa[n].getMagnitude(), "setPhase() magnitude changed", 0);
        }
      }
      
      // test ComplexFloatArray.setMagnitude()
      tempc.copyFrom(cfa);
      FloatArray magnitude=FloatArray::create(size);
      magnitude.noise(0, 10);
      // setMagnitude(FloatArray)
      tempc.setMagnitude(magnitude);
      for(size_t n=0; n<size; n++){
        if(cfa[n].getMagnitude()<0.001 || tempc[n].getMagnitude()<0.001)
          continue; //skip small values of magnitude which would make the phase noisy
        assertt(tempc[n].getPhase(), cfa[n].getPhase(), "setMagnitude() phase", 0.01);
        assertt(tempc[n].getMagnitude(), magnitude[n], "setMagnitude() magnitude", 0.01);
      }
      // setMagnitude(FloatArray, int, int)
      offset=size/3.0f;
      count=size/2.0f;
      tempc.copyFrom(cfa);
      tempc.setMagnitude(magnitude, offset, count);
      for(size_t n=0; n<size; n++){
        if(n>=offset && n<offset+count){
          if(cfa[n].getMagnitude()<0.001 || tempc[n].getMagnitude()<0.001)
            continue; //skip small values of magnitude which would make the phase noisy
          assertt(tempc[n].getPhase(), cfa[n].getPhase(), "setMagnitude() phase", 0.01);
          assertt(tempc[n].getMagnitude(), magnitude[n], "setMagnitude() magnitude", 0.01);
        } else { //check that values outside the range have not changed
          if(abs(tempc[n].getPhase()-cfa[n].getPhase())>0){
            debugMessage("error", (float)n, tempc[n].getPhase(), cfa[n].getPhase());
            return;
          }
          assertr(tempc[n].getPhase()==cfa[n].getPhase(), "setMagnitude()t phase");
          assertr(tempc[n].getMagnitude()==cfa[n].getMagnitude(), "setMagnitude() magnitude");
        }
      }
      FloatArray::destroy(phase);
      FloatArray::destroy(magnitude);
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
