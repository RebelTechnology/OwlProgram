#ifndef __FloatArrayTestPatch_hpp__
#define __FloatArrayTestPatch_hpp__

#include "TestPatch.hpp"

/*
 * Status: The first few tests have been converted to the new TestPatch format.
 *         Most of the tests still need to be converted to use CHECK(), CHECK_EQUAL() 
 *         or CHECK_CLOSE() as defined in TestPatch.hpp
 */

#define TESTCHECK(x, y) TEST(y); CHECK(x)
#define TESTCHECK_CLOSE(a, b, y) TEST(y); CHECK_CLOSE(a, b, DEFAULT_TOLERANCE)

class FloatArrayTestPatch : public TestPatch {
public:
  FloatArrayTestPatch(){
    size_t size=606;
    float data[size];
    float backupData[size];
    for(size_t n=0; n<size; n++){
      data[n]=rand()/(float)RAND_MAX *2 -1;
      backupData[n] = data[n];
    }
    FloatArray fa(data, size);
    FloatArray tempFa1=FloatArray::create(size);
    FloatArray tempFa2=FloatArray::create(size);
    FloatArray tempFa3=FloatArray::create(size);

    {
      TEST("sizeof");
      CHECK(sizeof(FloatArray) == sizeof(float*)+sizeof(size_t));
    }

    {
      TEST("constructor");
      CHECK(size == fa.getSize());
      CHECK(data == fa.getData());
    }

    {
      //test create method, called above
      TEST("create method");
      CHECK(size == tempFa1.getSize());
      CHECK(tempFa1.getData() != NULL);
      for(size_t n=0; n<size; n++){ //try to access each element 
	tempFa1[n]=0; //this should segfault if memory was not allocated properly
      }
    }

    {
      TEST("operator overrides");
      CHECK(fa.getSize()==size);
      CHECK(&fa[0]==data);
      CHECK((float*)fa==data);
    }
    
    //test equals
    {
      tempFa1.setAll(1);
      tempFa2.setAll(0);
      TESTCHECK(!tempFa1.equals(tempFa2), "not equals, all values are different");
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      TESTCHECK(tempFa1.equals(tempFa2), "equals");
      tempFa2[size-1]-=1;
      TESTCHECK(!tempFa1.equals(tempFa2), "not equals, one value is different");
      FloatArray temp=FloatArray::create(size-1);
      temp.copyFrom(tempFa2);
      TESTCHECK(!temp.equals(tempFa2), "not equals, size is different");
      FloatArray::destroy(temp);
    }
    
    //test setAll
    {
      float value=1.5;
      tempFa1.setAll(value);
      for(size_t n=0; n<size; n++){
        TESTCHECK(tempFa1[n]==value, "setAll()");
      }
    }
    
    //test copyTo
    fa.copyTo(tempFa1);
    for(size_t n=0; n<size; n++){
      TESTCHECK(tempFa1[n]==fa[n], "copyTo()");
    }
    
    //test copyFrom
    tempFa1.copyFrom(fa);
    for(size_t n=0; n<size; n++){
      TESTCHECK(tempFa1[n]==fa[n], "copyFrom()");
    }
    
    //test rectify
    fa.rectify(tempFa1);
    for(size_t n=0; n<size; n++){
      TESTCHECK(tempFa1[n]==abs(fa[n]),"rectify(tempFa1)");
    }
    tempFa1.copyFrom(fa);
    tempFa1.rectify(); //in place
    for(size_t n=0; n<size; n++){
      TESTCHECK(tempFa1[n]==abs(fa[n]),"rectify() in-place");
    }
    
    // test reverse
    fa.reverse(tempFa1);
    for(size_t n=0; n<size; n++){
      TESTCHECK(tempFa1[n]==fa[size-1-n], "reverse(tempFa1)");
    }
    tempFa1.copyFrom(fa);
    tempFa1.reverse(); //in place
    for(size_t n=0; n<size; n++){
      TESTCHECK(tempFa1[n]==fa[size-1-n], "reverse() in-place");
    }
    
    //test clip
    {
      tempFa1.copyFrom(fa);
      float clip=0.5;
      tempFa1.clip(clip);
      for(size_t n=0; n<size; n++){
        TESTCHECK(abs(tempFa1[n])<=clip, "clip to value");
      }
      tempFa1.copyFrom(fa);
      tempFa1.multiply(3);
      tempFa1.clip();
      for(size_t n=0; n<size; n++){
        TESTCHECK(abs(tempFa1[n])<=1, "clip to 1");
      }
	  tempFa1.copyFrom(fa);
      tempFa1.multiply(3);
      float min=0.1;
      float max=0.5;
      tempFa1.clip(min, max);
      for(size_t n=0; n<size; n++){
        TESTCHECK(abs(tempFa1[n])<=max && abs(tempFa1[n])>=min, "clip, asymmetric");
      }
    }
    //test negate
    tempFa1.copyFrom(fa);
    tempFa1.negate(); //in-place
    for(size_t n=0; n<size; n++){
      TESTCHECK(tempFa1[n]==-fa[n], "negate() inplace");
    }
    tempFa1.setAll(0);
    fa.negate(tempFa1); //negate and copy
    for(size_t n=0; n<size; n++){
      TESTCHECK(tempFa1[n]==-fa[n], "negate(destination)");
    }
    
    //test add
    {
      //test add(operand2)
      tempFa1.noise();
      tempFa2.copyFrom(fa);
      tempFa3.copyFrom(tempFa1);
      tempFa2.add(tempFa1);
      for(size_t n=0; n<size; n++){
        TESTCHECK(tempFa1[n]+fa[n]==tempFa2[n], "add(operand2) sum is correct");
        TESTCHECK(tempFa1[n]==tempFa3[n], "add(operand2) operand2 not modified");
      }
      //test add(operand2, destination)
      tempFa1.noise();
      tempFa2.setAll(0);
      tempFa3.copyFrom(tempFa1);
      fa.add(tempFa1,tempFa2);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(fa[n], backupData[n], "add(operand2, destination) source not modified");
        TESTCHECK_CLOSE(tempFa1[n], tempFa3[n], "add(operand2, destination) operand2 not modified");
        TESTCHECK_CLOSE(tempFa2[n], tempFa1[n]+fa[n], "add(operand2, destination) sum is correct");
      }
      //test fa.add(fa, fa)
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      tempFa1.add(tempFa1,tempFa1);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(tempFa1[n], tempFa2[n]+tempFa2[n], "fa.add(fa, fa) sum is correct");
      }
      //test add(scalar)
      float scalar=0.4;
      tempFa1.copyFrom(tempFa3);
      tempFa1.add(scalar);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(tempFa1[n], tempFa3[n]+scalar, "add(scalar)");
      }
    }    
    
    //test subtract
    {
      //test subtract(operand2)
      tempFa1.noise();
      tempFa2.copyFrom(fa);
      tempFa3.copyFrom(tempFa1);
      tempFa2.subtract(tempFa1);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(fa[n]-tempFa1[n], tempFa2[n], "subtract(operand2) subtraction is correct");
        TESTCHECK_CLOSE(tempFa1[n], tempFa3[n], "subtract(operand2) operand2 not modified");
      }
      //test subtract(operand2, destination)
      tempFa1.noise();
      tempFa2.setAll(0);
      tempFa3.copyFrom(tempFa1);
      fa.subtract(tempFa1,tempFa2);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(fa[n], backupData[n], "subtract(operand2, destination) source not modified");
        TESTCHECK_CLOSE(tempFa1[n], tempFa3[n], "subtract(operand2, destination) operand2 not modified");
        TESTCHECK_CLOSE(tempFa2[n], fa[n]-tempFa1[n], "subtract(operand2, destination) subtraction is correct");
      }
      //test fa.subtract(fa, fa)
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      tempFa1.subtract(tempFa1,tempFa1);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(tempFa1[n], tempFa2[n]-tempFa2[n], "fa.subtract(fa, fa) subtraction is correct");
      }
      //test subtract(scalar)
      float scalar=0.4;
      tempFa1.copyFrom(tempFa3);
      tempFa1.subtract(scalar);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(tempFa1[n], tempFa3[n]-scalar, "subtract(scalar)");
      }
    }
    
    //test multiply
    {
      //test multiply(operand2)
      tempFa1.noise();
      tempFa2.copyFrom(fa);
      tempFa3.copyFrom(tempFa1);
      tempFa2.multiply(tempFa1);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(fa[n]*tempFa1[n], tempFa2[n], "multiply(operand2) multiplication is correct");
        TESTCHECK_CLOSE(tempFa1[n], tempFa3[n], "multiply(operand2) operand2 not modified");
      }
      //test multiply(operand2, destination)
      tempFa1.noise();
      tempFa2.setAll(0);
      tempFa3.copyFrom(tempFa1);
      fa.multiply(tempFa1,tempFa2);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(fa[n], backupData[n], "multiply(operand2, destination) source not modified");
        TESTCHECK_CLOSE(tempFa1[n], tempFa3[n], "multiply(operand2, destination) operand2 not modified");
        TESTCHECK_CLOSE(tempFa2[n], fa[n]*tempFa1[n], "multiply(operand2, destination) multiplication is correct");
      }
      //test fa.multiply(fa, fa)
      tempFa1.noise();
      tempFa2.copyFrom(tempFa1);
      tempFa1.multiply(tempFa1,tempFa1);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(tempFa1[n], tempFa2[n]*tempFa2[n], "fa.multiply(fa, fa) multiplication is correct");
      }
      //test multiply(scalar)
      float scalar=0.4;
      tempFa1.copyFrom(tempFa3);
      tempFa1.multiply(scalar);
      for(size_t n=0; n<size; n++){
        TESTCHECK_CLOSE(tempFa1[n], tempFa3[n]*scalar, "multiply(scalar)");
      }
    }
    
    //test convolve
	if(0)
    { 
      tempFa1.noise();
      size_t size2=123;
      FloatArray operand2=FloatArray::create(size2);
      operand2.noise();
      FloatArray operand2Backup=FloatArray::create(size2);
      operand2Backup.copyFrom(operand2);
      FloatArray destination=FloatArray::create(size+size2-1);
      FloatArray destinationReference=FloatArray::create(size+size2-1);
      fa.convolve(operand2, destination);
      for(size_t n=0; n<size; n++){
        TESTCHECK(fa[n]==backupData[n], "convolve() source modified"); //check that the source array has not been modified
        TESTCHECK(operand2[n]==operand2Backup[n], "convolve() operand2 modified"); 
      }
      for (size_t n=0; n<size+size2-1; n++){
        size_t n1=n;
        destinationReference[n] =0;
        for(size_t k=0; k<size2; k++){
            if(n1>=0 && n1<size)
                destinationReference[n]+=fa[n1]*operand2[k];
            n1--;
        }
        TESTCHECK_CLOSE(destinationReference[n],destination[n],"convolve() result");
      }
      //test partial convolve with offset and samples 
      size_t offset=20;
      int samples=50;
      destination.setAll(-1);
      fa.convolve(operand2, destination, offset, samples);
      for(size_t n=0; n<size; n++){
        TESTCHECK(fa[n]==backupData[n], "partial convolve() source modified"); //check that the source array has not been modified
        TESTCHECK(operand2[n]==operand2Backup[n], "partial convolve() operand2 modified"); //check that the source array has not been modified
      }
      for(size_t n=offset; n<offset+samples; n++){
        //TODO: I suspect a bug in arm_conv_partial_f32
        //as the line below should be  destinationReference[n]==destination[n-offset]
        //instead destination[n] is left unchanged for n<offset
        TESTCHECK(destinationReference[n]==destination[n], "partial convolution result");
      }
      //test correlate
      operand2Backup.copyFrom(operand2);
      fa.correlate(operand2, destination);
      for(size_t n=0; n<size; n++){
        TESTCHECK(fa[n]==backupData[n], " convolve() source modified"); //check that the source array has not been modified
        TESTCHECK(operand2[n]==operand2Backup[n], "correlate() operand2 modified"); //check that the source array has not been modified
      }
      //correlation is the same as a convolution where one of the signals is flipped in time
      //so we flip in time operand2
      operand2.reverse();
      //and convolve it with fa to obtain the correlation
      fa.convolve(operand2, destinationReference);
      for(size_t n=0; n<size+size2-1; n++){
        TESTCHECK_CLOSE(destinationReference[n],destination[n], "correlate()"); //TODO: find out why this requires a large tolerance
      }
      //now redo correlation, but pre-initialize output to 0 and call convolveInitialized() isntead
      destination.setAll(0);
      fa.correlateInitialized(operand2, destination);
      fa.correlate(operand2, destinationReference);
      for(size_t n=0; n<size+size2-1; n++){
        TESTCHECK(destinationReference[n]==destination[n], "correlateInitialized()");
      }
      FloatArray::destroy(operand2);
      FloatArray::destroy(operand2Backup);
      FloatArray::destroy(destination);
      FloatArray::destroy(destinationReference);
    }
    // test insert
    {
      size_t offset=size/3;
      size_t samples=size/3;
      float init=0.1;
      tempFa1.setAll(init);
      tempFa1.insert(fa, offset, samples);
      for(size_t n=0; n<size; n++){
        if(n<offset || n>=offset+samples){
          TESTCHECK(tempFa1[n]==init, "insert() init");
        } else {
          TESTCHECK(tempFa1[n]==fa[n-offset], "insert() inserted values");
        }
      }
    }
    {
      float init=0.2;
      tempFa1.setAll(init);
      size_t sourceOffset=20;
      size_t destinationOffset=30;
      size_t samples=40;
      tempFa1.insert(fa, sourceOffset, destinationOffset, samples);
      for(size_t n=0; n<size; n++){
        if(n<destinationOffset || n>=destinationOffset+samples){
          TESTCHECK(tempFa1[n]==init, "insert() sourceOffset init");
        } else {
          TESTCHECK(tempFa1[n]==fa[n-destinationOffset+sourceOffset], "insert() sourceOffset inserted values");
        }
      }
    }
    {
      //test move
      size_t fromIndex=size/5;
      size_t toIndex=fromIndex*2;
      size_t length=size/6;
      tempFa1.copyFrom(fa);
      tempFa1.move(fromIndex, toIndex, length);
      for(size_t n=0; n<size; n++){
        if(n<toIndex||n>=toIndex+length){
          TESTCHECK(tempFa1[n]==fa[n], "move() non moved values");
        } else {
          TESTCHECK(tempFa1[n]==fa[fromIndex+ n-toIndex], "move() moved values");
        }
      }
    }
    {
      //test subArray
      size_t offset=size/5;
      size_t length=size/6;
      FloatArray sub=fa.subArray(offset, length);
      for(size_t n=0; n<sub.getSize(); n++){
        TESTCHECK(sub[n]==fa[n+offset], "subArray()");
      }
    }
    
    //statistics
    {
    //test min
      float refValue=fa[0];
      int refIndex=0;
      for(size_t n=1; n<size; n++){ //find reference min value and index
        if(fa[n]<refValue){
          refValue=fa[n];
          refIndex=n;
        }
      }
      float value;
      int index;
      fa.getMin(&value, &index);
      TESTCHECK(value==refValue,"getMin() value");
      TESTCHECK(index==refIndex,"getMin() index");
      TESTCHECK(fa.getMinValue()==refValue,"getMinValue()");
      TESTCHECK(fa.getMinIndex()==refIndex,"getMinIndex()");

      //test max
      refValue=fa[0];
      refIndex=0;
      for(size_t n=1; n<size; n++){ //find reference max value and index
        if(fa[n]>refValue){
          refValue=fa[n];
          refIndex=n;
        }
      }
      fa.getMax(&value, &index);
      TESTCHECK(value==refValue,"getMax() value");
      TESTCHECK(index==refIndex,"getMax() index");
      TESTCHECK(fa.getMaxValue()==refValue,"getMaxValue()");
      TESTCHECK(fa.getMaxIndex()==refIndex,"getMaxIndex()");
    }      

    //test rms
    {
      float rms=0;
      for(size_t n=0; n<size; n++){
        rms+=fa[n]*fa[n];
      } 
      rms=sqrt(rms/size);
      TESTCHECK_CLOSE(fa.getRms(), rms, "getRms()");
    }      

    //test power
    {
      float power=0;
      for(size_t n=0; n<size; n++){
        power+=fa[n]*fa[n];
      } 
      TESTCHECK_CLOSE(power, fa.getPower(), "getPower()");
    }    
    
    //quantities needed for mean, variance, standardDeviation
    {
      float sumOfSquares=0;
      float sum=0;
      for(size_t n=0; n<size; n++){
        sum+=fa[n];
        sumOfSquares+=fa[n]*fa[n];
      }
      //test mean
      float mean=sum/size;
      TESTCHECK_CLOSE(mean, fa.getMean(),"mean()");
      //test variance
      //variance is The average of the squared differences from the Mean."
      float var=0;
      for(size_t n=0; n<size; n++){
        float diff=fa[n]-mean;
        var+=diff*diff;
      }
      var=var/(size-1);
      TESTCHECK_CLOSE(var, fa.getVariance(), "getVariance()");
      //test standardDeviation
      float std = sqrtf(var);
      TESTCHECK_CLOSE(std, fa.getStandardDeviation(), "getStandardDeviation()");
    }
    
    //test noise using statistics
    tempFa1.noise();
    TESTCHECK(abs(tempFa1.getMean())<0.5, "noise mean");
    TESTCHECK(tempFa1.getVariance()<0.5, "noise variance");
    // TESTCHECK(tempFa1.getStandardDeviation()<1, "noise standard deviation");
    TESTCHECK(tempFa1.getRms()>0 && tempFa1.getRms()<1, "noise rms");
    TESTCHECK(tempFa1.getPower()>0 && tempFa1.getPower()<size, "noise power");
    
    // FloatArray subarray(int offset, int length);
    // float getDb();

    //check that the original data were not changed by any of the methods
    for(size_t n=0; n<size; n++){
      TESTCHECK(data[n]==backupData[n],"original data");
    }
    FloatArray::destroy(tempFa1);
    FloatArray::destroy(tempFa2);
    FloatArray::destroy(tempFa3);
  }
};

#endif // __FloatArrayTestPatch_hpp__
