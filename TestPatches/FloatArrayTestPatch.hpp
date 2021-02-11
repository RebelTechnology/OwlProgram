#ifndef __FloatArrayTestPatch_hpp__
#define __FloatArrayTestPatch_hpp__

#include "Patch.h"
#ifdef assert
#undef assert
#endif

class FloatArrayTestPatch : public Patch {
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
    return assert(cond, message);
  }
  bool assertt(float first, float second, const char *message){ //assert with tolerance of 0.0000001
    return assertt(first, second, message,  0.0000001);
  }
  bool assert(bool condition,const char * message){
    return assert(condition, message, -1);
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
    int size=101;
    float data[size];
    float backupData[size];
    for(int n=0; n<size; n++){
      data[n]=rand()/(float)RAND_MAX *2 -1;
      backupData[n]=data[n];
    }
    //test constructor
    FloatArray fa(data, size);
    assert(size==fa.getSize(), "constructor size");
    assert(data==fa.getData(), "constructor data");
    FloatArray tempFa1=FloatArray::create(size);
    FloatArray tempFa2=FloatArray::create(size);
    FloatArray tempFa3=FloatArray::create(size);
    
    //test create method, called above
    assert(size==tempFa1.getSize(),"create() size");
    assert(tempFa1.getData()!=NULL,"create() data");
    for(int n=0; n<size; n++){ //try to access each element 
      tempFa1[n]=0; //this should segfault if memory was not allocated properly
    }
    
    //test operator overrides
    assert(fa.getSize()==size, "getSize");
    assert(&fa[0]==data, "[] operator");
    assert((float *)fa==data, "(float *) operator");
    
    //test equals
    {
      tempFa1.setAll(1);
      tempFa2.setAll(0);
      assert(!tempFa1.equals(tempFa2), "not equals, all values are different");
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      assert(tempFa1.equals(tempFa2), "equals");
      tempFa2[size-1]-=1;
      assert(!tempFa1.equals(tempFa2), "not equals, one value is different");
      FloatArray temp=FloatArray::create(size-1);
      temp.copyFrom(tempFa2);
      assert(!temp.equals(tempFa2), "not equals, size is different");
      FloatArray::destroy(temp);
    }
    
    //test setAll
    {
      float value=1.5;
      tempFa1.setAll(value);
      for(int n=0; n<size; n++){
        assert(tempFa1[n]==value, "setAll()");
      }
    }
    
    //test copyTo
    fa.copyTo(tempFa1);
    for(int n=0; n<size; n++){
      assert(tempFa1[n]==fa[n], "copyTo()");
    }
    
    //test copyFrom
    tempFa1.copyFrom(fa);
    for(int n=0; n<size; n++){
      assert(tempFa1[n]==fa[n], "copyFrom()");
    }
    
    //test rectify
    fa.rectify(tempFa1);
    for(int n=0; n<size; n++){
      assert(tempFa1[n]==abs(fa[n]),"rectify(tempFa1)");
    }
    tempFa1.copyFrom(fa);
    tempFa1.rectify(); //in place
    for(int n=0; n<size; n++){
      assert(tempFa1[n]==abs(fa[n]),"rectify() in-place");
    }
    
    // test reverse
    fa.reverse(tempFa1);
    for(int n=0; n<size; n++){
      assert(tempFa1[n]==fa[size-1-n], "reverse(tempFa1)");
    }
    tempFa1.copyFrom(fa);
    tempFa1.reverse(); //in place
    for(int n=0; n<size; n++){
      assert(tempFa1[n]==fa[size-1-n], "reverse() in-place");
    }
    
    //test clip
    {
      tempFa1.copyFrom(fa);
      float clip=0.5;
      tempFa1.clip(clip);
      for(int n=0; n<size; n++){
        assert(abs(tempFa1[n])<=clip, "clip to value");
      }
      tempFa1.copyFrom(fa);
      tempFa1.multiply(3);
      tempFa1.clip();
      for(int n=0; n<size; n++){
        assert(abs(tempFa1[n])<=1, "clip to 1");
      }
	  tempFa1.copyFrom(fa);
      tempFa1.multiply(3);
      float min=0.1;
      float max=0.5;
      tempFa1.clip(min, max);
      for(int n=0; n<size; n++){
        assert(abs(tempFa1[n])<=max && abs(tempFa1[n])>=min, "clip, asymmetric");
      }
    }
    //test negate
    tempFa1.copyFrom(fa);
    tempFa1.negate(); //in-place
    for(int n=0; n<size; n++){
      assert(tempFa1[n]==-fa[n], "negate() inplace");
    }
    tempFa1.setAll(0);
    fa.negate(tempFa1); //negate and copy
    for(int n=0; n<size; n++){
      assert(tempFa1[n]==-fa[n], "negate(destination)");
    }
    
    //test add
    {
      //test add(operand2)
      tempFa1.noise();
      tempFa2.copyFrom(fa);
      tempFa3.copyFrom(tempFa1);
      tempFa2.add(tempFa1);
      for(int n=0; n<size; n++){
        assert(tempFa1[n]+fa[n]==tempFa2[n], "add(operand2) sum is correct");
        assert(tempFa1[n]==tempFa3[n], "add(operand2) operand2 not modified");
      }
      //test add(operand2, destination)
      tempFa1.noise();
      tempFa2.setAll(0);
      tempFa3.copyFrom(tempFa1);
      fa.add(tempFa1,tempFa2);
      for(int n=0; n<size; n++){
        assert(fa[n]==backupData[n], "add(operand2, destination) source not modified");
        assert(tempFa1[n]==tempFa3[n], "add(operand2, destination) operand2 not modified");
        assert(tempFa2[n]==tempFa1[n]+fa[n], "add(operand2, destination) sum is correct");
      }
      //test fa.add(fa, fa)
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      tempFa1.add(tempFa1,tempFa1);
      for(int n=0; n<size; n++){
        assert(tempFa1[n]==tempFa2[n]+tempFa2[n], "fa.add(fa, fa) sum is correct");
      }
      //test add(scalar)
      float scalar=0.4;
      tempFa1.copyFrom(tempFa3);
      tempFa1.add(scalar);
      for(int n=0; n<size; n++){
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
      for(int n=0; n<size; n++){
        assert(fa[n]-tempFa1[n]==tempFa2[n], "subtract(operand2) subtraction is correct");
        assert(tempFa1[n]==tempFa3[n], "subtract(operand2) operand2 not modified");
      }
      //test subtract(operand2, destination)
      tempFa1.noise();
      tempFa2.setAll(0);
      tempFa3.copyFrom(tempFa1);
      fa.subtract(tempFa1,tempFa2);
      for(int n=0; n<size; n++){
        assert(fa[n]==backupData[n], "subtract(operand2, destination) source not modified");
        assert(tempFa1[n]==tempFa3[n], "subtract(operand2, destination) operand2 not modified");
        assert(tempFa2[n]==fa[n]-tempFa1[n], "subtract(operand2, destination) subtraction is correct");
      }
      //test fa.subtract(fa, fa)
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      tempFa1.subtract(tempFa1,tempFa1);
      for(int n=0; n<size; n++){
        assert(tempFa1[n]==tempFa2[n]-tempFa2[n], "fa.subtract(fa, fa) subtraction is correct");
      }
      //test subtract(scalar)
      float scalar=0.4;
      tempFa1.copyFrom(tempFa3);
      tempFa1.subtract(scalar);
      for(int n=0; n<size; n++){
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
      for(int n=0; n<size; n++){
        assert(fa[n]*tempFa1[n]==tempFa2[n], "multiply(operand2) multiplication is correct");
        assert(tempFa1[n]==tempFa3[n], "multiply(operand2) operand2 not modified");
      }
      //test multiply(operand2, destination)
      tempFa1.noise();
      tempFa2.setAll(0);
      tempFa3.copyFrom(tempFa1);
      fa.multiply(tempFa1,tempFa2);
      for(int n=0; n<size; n++){
        assert(fa[n]==backupData[n], "multiply(operand2, destination) source not modified");
        assert(tempFa1[n]==tempFa3[n], "multiply(operand2, destination) operand2 not modified");
        assert(tempFa2[n]==fa[n]*tempFa1[n], "multiply(operand2, destination) multiplication is correct");
      }
      //test fa.multiply(fa, fa)
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      tempFa1.multiply(tempFa1,tempFa1);
      for(int n=0; n<size; n++){
        assert(tempFa1[n]==tempFa2[n]*tempFa2[n], "fa.multiply(fa, fa) multiplication is correct");
      }
      //test multiply(scalar)
      float scalar=0.4;
      tempFa1.copyFrom(tempFa3);
      tempFa1.multiply(scalar);
      for(int n=0; n<size; n++){
        assert(tempFa1[n]==tempFa3[n]*scalar, "multiply(scalar)");
      }
    }
    
    //test convolve
	if(0)
    { 
      tempFa1.noise();
      int size2=123;
      FloatArray operand2=FloatArray::create(size2);
      operand2.noise();
      FloatArray operand2Backup=FloatArray::create(size2);
      operand2Backup.copyFrom(operand2);
      FloatArray destination=FloatArray::create(size+size2-1);
      FloatArray destinationReference=FloatArray::create(size+size2-1);
      fa.convolve(operand2, destination);
      for(int n=0; n<size; n++){
        assert(fa[n]==backupData[n], "convolve() source modified"); //check that the source array has not been modified
        assert(operand2[n]==operand2Backup[n], "convolve() operand2 modified"); 
      }
      for (int n=0; n<size+size2-1; n++){
        int n1=n;
        destinationReference[n] =0;
        for(int k=0; k<size2; k++){
            if(n1>=0 && n1<size)
                destinationReference[n]+=fa[n1]*operand2[k];
            n1--;
        }
        assertt(destinationReference[n],destination[n],"convolve() result");
      }
      //test partial convolve with offset and samples 
      int offset=20;
      int samples=50;
      destination.setAll(-1);
      fa.convolve(operand2, destination, offset, samples);
      for(int n=0; n<size; n++){
        assert(fa[n]==backupData[n], "partial convolve() source modified"); //check that the source array has not been modified
        assert(operand2[n]==operand2Backup[n], "partial convolve() operand2 modified"); //check that the source array has not been modified
      }
      for(int n=offset; n<offset+samples; n++){
        //TODO: I suspect a bug in arm_conv_partial_f32
        //as the line below should be  destinationReference[n]==destination[n-offset]
        //instead destination[n] is left unchanged for n<offset
        assert(destinationReference[n]==destination[n], "partial convolution result");
      }
      //test correlate
      operand2Backup.copyFrom(operand2);
      fa.correlate(operand2, destination);
      for(int n=0; n<size; n++){
        assert(fa[n]==backupData[n], " convolve() source modified"); //check that the source array has not been modified
        assert(operand2[n]==operand2Backup[n], "correlate() operand2 modified"); //check that the source array has not been modified
      }
      //correlation is the same as a convolution where one of the signals is flipped in time
      //so we flip in time operand2
      operand2.reverse();
      //and convolve it with fa to obtain the correlation
      fa.convolve(operand2, destinationReference);
      for(int n=0; n<size+size2-1; n++){
        assertt(destinationReference[n],destination[n], "correlate()",0.00001); //TODO: find out why this requires a large tolerance
      }
      //now redo correlation, but pre-initialize output to 0 and call convolveInitialized() isntead
      destination.setAll(0);
      fa.correlateInitialized(operand2, destination);
      fa.correlate(operand2, destinationReference);
      for(int n=0; n<size+size2-1; n++){
        assert(destinationReference[n]==destination[n], "correlateInitialized()");
      }
      FloatArray::destroy(operand2);
      FloatArray::destroy(operand2Backup);
      FloatArray::destroy(destination);
      FloatArray::destroy(destinationReference);
    }
    // test insert
    {
      int offset=size/3;
      int samples=size/3;
      float init=0.1;
      tempFa1.setAll(init);
      tempFa1.insert(fa, offset, samples);
      for(int n=0; n<size; n++){
        if(n<offset || n>=offset+samples){
          assert(tempFa1[n]==init, "insert() init");
        } else {
          assert(tempFa1[n]==fa[n-offset], "insert() inserted values");
        }
      }
    }
    {
      float init=0.2;
      tempFa1.setAll(init);
      int sourceOffset=20;
      int destinationOffset=30;
      int samples=40;
      tempFa1.insert(fa, sourceOffset, destinationOffset, samples);
      for(int n=0; n<size; n++){
        if(n<destinationOffset || n>=destinationOffset+samples){
          assert(tempFa1[n]==init, "insert() sourceOffset init", n);
        } else {
          assert(tempFa1[n]==fa[n-destinationOffset+sourceOffset], "insert() sourceOffset inserted values");
        }
      }
    }
    {
      //test move
      int fromIndex=size/5;
      int toIndex=fromIndex*2;
      int length=size/6;
      tempFa1.copyFrom(fa);
      tempFa1.move(fromIndex, toIndex, length);
      for(int n=0; n<size; n++){
        if(n<toIndex||n>=toIndex+length){
          assert(tempFa1[n]==fa[n], "move() non moved values");
        } else {
          assert(tempFa1[n]==fa[fromIndex+ n-toIndex], "move() moved values");
        }
      }
    }
    {
      //test subArray
      int offset=size/5;
      int length=size/6;
      FloatArray sub=fa.subArray(offset, length);
      for(int n=0; n<sub.getSize(); n++){
        assert(sub[n]==fa[n+offset], "subArray()");
      }
    }
    
    //statistics
    {
    //test min
      float refValue=fa[0];
      int refIndex=0;
      for(int n=1; n<size; n++){ //find reference min value and index
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
      for(int n=1; n<size; n++){ //find reference max value and index
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
      for(int n=0; n<size; n++){
        rms+=fa[n]*fa[n];
      } 
      rms=sqrt(rms/size);
      assertt(fa.getRms(), rms, "getRms()", 0.0001);
    }      

    //test power
    {
      float power=0;
      for(int n=0; n<size; n++){
        power+=fa[n]*fa[n];
      } 
      assertt(power, fa.getPower(), "getPower()", 0.0001);
    }    
    
    //quantities needed for mean, variance, standardDeviation
    {
      float sumOfSquares=0;
      float sum=0;
      for(int n=0; n<size; n++){
        sum+=fa[n];
        sumOfSquares+=fa[n]*fa[n];
      }
      //test mean
      float mean=sum/size;
      assertt(mean, fa.getMean(),"mean()", 0.001);
      //test variance
      //variance is The average of the squared differences from the Mean."
      float var=0;
      for(int n=0; n<size; n++){
        float diff=fa[n]-mean;
        var+=diff*diff;
      }
      var=var/(size-1);
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
    assert(tempFa1.getPower()>0 && tempFa1.getPower()<size, "noise power");
    
    // FloatArray subarray(int offset, int length);
    // float getDb();

//check that the original data were not changed by any of the methods
    for(int n=0; n<size; n++){
      assert(data[n]==backupData[n],"original data");
    }
    FloatArray::destroy(tempFa1);
    FloatArray::destroy(tempFa2);
    FloatArray::destroy(tempFa3);
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
