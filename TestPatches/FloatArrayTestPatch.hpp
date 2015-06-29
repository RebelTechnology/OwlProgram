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
      data[n]=rand()/(float)RAND_MAX *2 -1;
      backupData[n]=data[n];
    }
    //test constructor
    FloatArray fa(data, sz);
    assert(sz==fa.getSize(), "constructor size");
    assert(data==fa.getData(), "constructor data");
    FloatArray tempFa1=FloatArray::create(sz);
    FloatArray tempFa2=FloatArray::create(sz);
    FloatArray tempFa3=FloatArray::create(sz);
    
    //test create method, called above
    assert(sz==tempFa1.getSize(),"create() size");
    assert(tempFa1.getData()!=NULL,"create() data");
    for(int n=0; n<sz; n++){ //try to access each element 
      tempFa1[n]=0; //this should segfault if memory was not allocated properly
    }
    
    //test operator overrides
    assert(fa.getSize()==sz, "getSize");
    assert(&fa[0]==data, "[] operator");
    assert((float *)fa==data, "(float *) operator");

    //test setAll
    {
      float value=1.5;
      tempFa1.setAll(value);
      for(int n=0; n<sz; n++){
        assert(tempFa1[n]==value, "setAll()");
      }
    }
    
    //test copyTo
    fa.copyTo(tempFa1);
    for(int n=0; n<sz; n++){
      assert(tempFa1[n]==fa[n], "copyTo()");
    }
    
    //test copyFrom
    tempFa1.copyFrom(fa);
    for(int n=0; n<sz; n++){
      assert(tempFa1[n]==fa[n], "copyFrom()");
    }
    
    //test rectify
    fa.rectify(tempFa1);
    for(int n=0; n<sz; n++){
      assert(tempFa1[n]==abs(fa[n]),"rectify(tempFa1)");
    }
    tempFa1.copyFrom(fa);
    tempFa1.rectify(); //in place
    for(int n=0; n<sz; n++){
      assert(tempFa1[n]==abs(fa[n]),"rectify() in-place");
    }
    
    // test reverse
    fa.reverse(tempFa1);
    for(int n=0; n<sz; n++){
      assert(tempFa1[n]==fa[sz-1-n], "reverse(tempFa1)");
    }
    tempFa1.copyFrom(fa);
    tempFa1.reverse(); //in place
    for(int n=0; n<sz; n++){
      assert(tempFa1[n]==fa[sz-1-n], "reverse() in-place");
    }
    
    //test scale
    {
      tempFa1.copyFrom(fa);
      float factor=0.5;
      tempFa1.scale(factor); //in-place
      for(int n=0; n<sz; n++){
        assert(tempFa1[n]==fa[n]*factor, "scale() in-place");
      }
      tempFa1.setAll(0);
      fa.scale(factor, tempFa1); //scale and copy
      for(int n=0; n<sz; n++){
        assert(tempFa1[n]==fa[n]*factor, "scale() in-place");
      }
    }
    
    //test negate
    tempFa1.copyFrom(fa);
    tempFa1.negate(); //in-place
    for(int n=0; n<sz; n++){
      assert(tempFa1[n]==-fa[n], "negate() inplace");
    }
    tempFa1.setAll(0);
    fa.negate(tempFa1); //negate and copy
    for(int n=0; n<sz; n++){
      assert(tempFa1[n]==-fa[n], "negate(destination)");
    }
    
    //test add
    {
      //test add(operand2)
      tempFa1.noise();
      tempFa2.copyFrom(fa);
      tempFa3.copyFrom(tempFa1);
      tempFa2.add(tempFa1);
      for(int n=0; n<sz; n++){
        assert(tempFa1[n]+fa[n]==tempFa2[n], "add(operand2) sum is correct");
        assert(tempFa1[n]==tempFa3[n], "add(operand2) operand2 not modified");
      }
      //test add(operand2, destination)
      tempFa1.noise();
      tempFa2.setAll(0);
      tempFa3.copyFrom(tempFa1);
      fa.add(tempFa1,tempFa2);
      for(int n=0; n<sz; n++){
        assert(fa[n]==backupData[n], "add(operand2, destination) source not modified");
        assert(tempFa1[n]==tempFa3[n], "add(operand2, destination) operand2 not modified");
        assert(tempFa2[n]==tempFa1[n]+fa[n], "add(operand2, destination) sum is correct");
      }
      //test fa.add(fa, fa)
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      tempFa1.add(tempFa1,tempFa1);
      for(int n=0; n<sz; n++){
        assert(tempFa1[n]==tempFa2[n]+tempFa2[n], "fa.add(fa, fa) sum is correct");
      }
      //test add(scalar)
      float scalar=0.4;
      tempFa1.copyFrom(tempFa3);
      tempFa1.add(scalar);
      for(int n=0; n<sz; n++){
        assert(tempFa1[n]==tempFa3[n]+scalar, "add(scalar)");
      }
    }    
    
    //test subtract
    {
      //test subtract(operand2)
      tempFa1.noise();
      tempFa2.copyFrom(fa);
      tempFa3.copyFrom(tempFa1);
      tempFa2.subtract(tempFa1);
      for(int n=0; n<sz; n++){
        assert(fa[n]-tempFa1[n]==tempFa2[n], "subtract(operand2) subtraction is correct");
        assert(tempFa1[n]==tempFa3[n], "subtract(operand2) operand2 not modified");
      }
      //test subtract(operand2, destination)
      tempFa1.noise();
      tempFa2.setAll(0);
      tempFa3.copyFrom(tempFa1);
      fa.subtract(tempFa1,tempFa2);
      for(int n=0; n<sz; n++){
        assert(fa[n]==backupData[n], "subtract(operand2, destination) source not modified");
        assert(tempFa1[n]==tempFa3[n], "subtract(operand2, destination) operand2 not modified");
        assert(tempFa2[n]==fa[n]-tempFa1[n], "subtract(operand2, destination) subtraction is correct");
      }
      //test fa.subtract(fa, fa)
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      tempFa1.subtract(tempFa1,tempFa1);
      for(int n=0; n<sz; n++){
        assert(tempFa1[n]==tempFa2[n]-tempFa2[n], "fa.subtract(fa, fa) subtraction is correct");
      }
      //test subtract(scalar)
      float scalar=0.4;
      tempFa1.copyFrom(tempFa3);
      tempFa1.subtract(scalar);
      for(int n=0; n<sz; n++){
        assert(tempFa1[n]==tempFa3[n]-scalar, "subtract(scalar)");
      }
    }
    
    //test multiply
    {
      //test multiply(operand2)
      tempFa1.noise();
      tempFa2.copyFrom(fa);
      tempFa3.copyFrom(tempFa1);
      tempFa2.multiply(tempFa1);
      for(int n=0; n<sz; n++){
        assert(fa[n]*tempFa1[n]==tempFa2[n], "multiply(operand2) multiplication is correct");
        assert(tempFa1[n]==tempFa3[n], "multiply(operand2) operand2 not modified");
      }
      //test multiply(operand2, destination)
      tempFa1.noise();
      tempFa2.setAll(0);
      tempFa3.copyFrom(tempFa1);
      fa.multiply(tempFa1,tempFa2);
      for(int n=0; n<sz; n++){
        assert(fa[n]==backupData[n], "multiply(operand2, destination) source not modified");
        assert(tempFa1[n]==tempFa3[n], "multiply(operand2, destination) operand2 not modified");
        assert(tempFa2[n]==fa[n]*tempFa1[n], "multiply(operand2, destination) multiplication is correct");
      }
      //test fa.multiply(fa, fa)
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      tempFa1.multiply(tempFa1,tempFa1);
      for(int n=0; n<sz; n++){
        assert(tempFa1[n]==tempFa2[n]*tempFa2[n], "fa.multiply(fa, fa) multiplication is correct");
      }
      //test multiply(scalar)
      float scalar=0.4;
      tempFa1.copyFrom(tempFa3);
      tempFa1.multiply(scalar);
      for(int n=0; n<sz; n++){
        assert(tempFa1[n]==tempFa3[n]*scalar, "multiply(scalar)");
      }
    }
  // void convolve(FloatArray other, FloatArray destination);
  // void convolve(FloatArray other, FloatArray destination, int offset, int samples);
  // void correlate(FloatArray other, FloatArray destination);
  // void setAll(float value);
  // FloatArray subarray(int offset, int length);
  // void copyTo(float* destination, int length);
  // void copyFrom(float* source, int length);
  // void insert(FloatArray source, int offset, int length);
  // void move(int fromIndex, int toIndex, int length);
    
    //statistics
    {
    //test min
      float refValue=fa[0];
      int refIndex=0;
      for(int n=1; n<sz; n++){ //find reference min value and index
        if(fa[n]<refValue){
          refValue=fa[n];
          refIndex=n;
        }
      }
      float value;
      int index;
      fa.getMin(&value, &index);
      assert(value==refValue,"getMin() value");
      assert(index==refIndex,"getMin() index");
      assert(fa.getMinValue()==refValue,"getMinValue()");
      assert(fa.getMinIndex()==refIndex,"getMinIndex()");

      //test max
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
    }      

    //test rms
    {
      float rms=0;
      for(int n=0; n<sz; n++){
        rms+=fa[n]*fa[n];
      } 
      rms=sqrt(rms/sz);
      assert(fa.getRms()==rms,"getRms()");
    }      

    //test power
    {
      float power=0;
      for(int n=0; n<sz; n++){
        power+=fa[n]*fa[n];
      } 
      assert(power==fa.getPower(),"getPower()");
    }    
    
    //quantities needed for mean, variance, standardDeviation
    {
      float sumOfSquares=0;
      float sum=0;
      for(int n=0; n<sz; n++){
        sum+=fa[n];
        sumOfSquares+=fa[n]*fa[n];
      }
      //test mean
      float mean=sum/sz;
      assert(mean==fa.getMean(),"mean()");
      //test variance
      //variance is The average of the squared differences from the Mean."
      float var=0;
      for(int n=0; n<sz; n++){
        float diff=fa[n]-mean;
        var+=diff*diff;
      }
      var=var/(sz-1);
      assertt(var, fa.getVariance(), "getVariance()");
      //test standardDeviation
      float std = sqrtf(var);
      assertt(std, fa.getStandardDeviation(), "getStandardDeviation()");
    }
    
    //test noise using statistics
    tempFa1.noise();
    assert(abs(tempFa1.getMean())<0.5, "noise mean");
    assert(tempFa1.getVariance()<0.5, "noise variance");
    // assert(tempFa1.getStandardDeviation()<1, "noise standard deviation");
    assert(tempFa1.getRms()>0 && tempFa1.getRms()<1, "noise rms");
    assert(tempFa1.getPower()>0 && tempFa1.getPower()<sz, "noise power");
    
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
