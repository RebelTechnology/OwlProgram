#ifndef __FloatArrayTestPatch_hpp__
#define __FloatArrayTestPatch_hpp__

#include "StompBox.h"

class FloatArrayTestPatch : public Patch {
private:
  bool success;
  int passed;
  int failed;
  bool assertt(float first, float second, const char *message){ //assert with tolerance
    bool cond=false;
    float tol=0.0000001;
    if(abs(second-first)<tol){
      cond=true;
    }
    assert(cond, message);
  }
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
  FloatArrayTestPatch(){
    success=true;
    passed=0;
    failed=0;
    int sz=101;
    float data[sz];
    float backupData[sz];
    for(int n=0; n<sz; n++){
      if(n<sz/2*1.5)
        data[n]=n/(sz/2.0f) * 2 - 1; 
      else
        data[n]=(sz-n)/(sz/2.0f) * 2 - 1;
      data[n]=rand()/(float)RAND_MAX *2 -1;
      backupData[n]=data[n];
    }
    FloatArray fa(data, sz);
    float tempData[sz]; 
    FloatArray tempFa(tempData,sz);
    //operator overrides
    assert(fa.getSize()==sz, "getSize");
    assert(&fa[0]==data, "[] operator");
    assert((float *)fa==data, "(float *) operator");

    fa.rectify(tempFa);
    for(int n=0; n<sz; n++){
      assert(tempFa[n]==abs(fa[n]),"rectify(tempFa)");
    }
    //copy fa to tempFa
    for(int n=0; n<sz; n++){
      tempFa[n]=fa[n];
    }
    //so we can use the in-place method
    tempFa.rectify(); //in place
    for(int n=0; n<sz; n++){
      assert(tempFa[n]==abs(fa[n]),"rectify() in-place");
    }
    fa.reverse(tempFa);
    for(int n=0; n<sz; n++){
      assert(tempFa[n]==fa[sz-1-n], "reverse(tempFa)");
    }
    //copy fa to tempFa
    for(int n=0; n<sz; n++){
      tempFa[n]=fa[n];
    }
    //so we can use the in-place method
    tempFa.reverse(); //in place
    for(int n=0; n<sz; n++){
      assert(tempFa[n]==fa[sz-1-n], "reverse() in-place");
    }
    //copy fa to tempFa
    for(int n=0; n<sz; n++){
      tempFa[n]=fa[n];
    }
    //so we can use the in-place method
    float factor=0.5;
    tempFa.scale(factor);
    for(int n=0; n<sz; n++){
      assert(tempFa[n]==fa[n]*factor, "scale() in-place");
    }
    
    //statistics
    //min
    float refValue=fa[0];
    long unsigned int refIndex=0;
    for(int n=1; n<sz; n++){ //find reference min value and index
      if(fa[n]<refValue){
        refValue=fa[n];
        refIndex=n;
      }
    }
    float value;
    long unsigned int index;
    fa.getMin(&value, &index);
    assert(value==refValue,"getMin() value");
    assert(index==refIndex,"getMin() index");
    assert(fa.getMinValue()==refValue,"getMinValue()");
    assert(fa.getMinIndex()==refIndex,"getMinIndex()");
    //max
    refValue=fa[0];
    refIndex=0;
    for(int n=1; n<sz; n++){ //find reference max value and index
      if(fa[n]>refValue){
        refValue=fa[n];
        refIndex=n;
      }
    }
    fa.getMax(&value, &index);
    assert(value==refValue,"getMax() value");
    assert(index==refIndex,"getMax() index");
    assert(fa.getMaxValue()==refValue,"getMaxValue()");
    assert(fa.getMaxIndex()==refIndex,"getMaxIndex()");
    
    //rms
    float rms=0;
    for(int n=0; n<sz; n++){
      rms+=fa[n]*fa[n];
    } 
    rms=sqrt(rms/sz);
    assert(fa.getRms()==rms,"getRms()");
      
    //power
    float power=0;
    for(int n=0; n<sz; n++){
      power+=fa[n]*fa[n];
    } 
    assert(power==fa.getPower(),"getPower()");
    
    //quantities needed for mean, variance, standardDeviation
    float sumOfSquares=0;
    float sum=0;
    for(int n=0; n<sz; n++){
      sum+=fa[n];
      sumOfSquares+=fa[n]*fa[n];
    }
    //mean
    float mean=sum/sz;
    assert(mean==fa.getMean(),"mean()");
    //variance
    //variance is The average of the squared differences from the Mean."
    float var=0;
    for(int n=0; n<sz; n++){
      float diff=fa[n]-mean;
      var+=diff*diff;
    }
    var=var/(sz-1);
    assertt(var, fa.getVariance(), "getVariance()");
    //standardDeviation
    float std = sqrtf(var);
    assertt(std, fa.getStandardDeviation(), "getStandardDeviation()");

    // FloatArray subarray(int offset, int length);
    // float getDb();

//check that the original data were not changed by any of the methods
    for(int n=0; n<sz; n++){
      assert(data[n]==backupData[n],"original data");
    }
    if(success==true){
      debugMessage("Tests passed: ",passed);
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

#endif // __FloatArrayTestPatch_hpp__
