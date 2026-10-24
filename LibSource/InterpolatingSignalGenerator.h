#ifndef __InterpolatingSignalGenerator_h__
#define __InterpolatingSignalGenerator_h__

#include "FloatArray.h"
#include "CircularBuffer.h"
#include "Interpolator.h"

/**
 * Signal Generator that generates an arbitrary number of in-between points,
 * interpolated between two (linear, cosine) or more (cubic, hermite) reference
 * points.
 * Can be used e.g. to convert a block rate signal to audio rate.
 */
template<InterpolationMethod im = LINEAR_INTERPOLATION>
class InterpolatingSignalGenerator {
protected:
  CircularFloatBuffer* points;
  float mu = 0;
  float inc = 0;
  static InterpolatingSignalGenerator<im>* allocate(size_t len){
    CircularFloatBuffer* points = CircularFloatBuffer::create(len);
    return new InterpolatingSignalGenerator<im>(points);
  }
public:
  InterpolatingSignalGenerator(CircularFloatBuffer* points): points(points){}
  /** Set period in samples */
  void setPeriod(float period){
    inc = 1/period;
  }
  /** Get period in samples */
  float getPeriod(){
    return 1/inc;
  }
  /** 
   * Update the interpolator with a new reference point.
   * update() must be called once every period.
   */
  void update(float x){
    points->write(x);
    mu = 0;
  }
  /**
   * Reset interpolator to a fixed value.
   * Sets all reference points to `x`.
   */
  void set(float x){
    points->setAll(x);
    mu = 0;
  }
  float generate();
  void generate(FloatArray output);
  static InterpolatingSignalGenerator<im>* create();
  static InterpolatingSignalGenerator<im>* create(size_t period){
    InterpolatingSignalGenerator<im>* obj = create();
    obj->setPeriod(period);
    return obj;
  }
  static void destroy(InterpolatingSignalGenerator<im>* obj){
    CircularFloatBuffer::destroy(obj->points);
    delete obj;
  }
};

template<>
void InterpolatingSignalGenerator<NO_INTERPOLATION>::generate(FloatArray output){
  output.setAll(points->read());
}

template<>
float InterpolatingSignalGenerator<NO_INTERPOLATION>::generate(){
  return points->read();
}

template<>
InterpolatingSignalGenerator<NO_INTERPOLATION>*
InterpolatingSignalGenerator<NO_INTERPOLATION>::create(){
  return allocate(1);
}

template<>
float InterpolatingSignalGenerator<LINEAR_INTERPOLATION>::generate(){
  float pts[2];
  points->setDelay(2);
  points->read(pts, 2);
  float y = Interpolator::linear(pts[0], pts[1], mu);
  mu += inc;
  return y;
}

template<>
void InterpolatingSignalGenerator<LINEAR_INTERPOLATION>::generate(FloatArray output){
  float pts[2];
  points->setDelay(2);
  points->read(pts, 2);
  size_t len = output.getSize();
  float* out = output.getData();
  while(len--){
    *out++ = Interpolator::linear(pts[0], pts[1], mu);
    mu += inc;
  }
}

template<>
InterpolatingSignalGenerator<LINEAR_INTERPOLATION>*
InterpolatingSignalGenerator<LINEAR_INTERPOLATION>::create(){
  return allocate(2);
}

template<>
float InterpolatingSignalGenerator<COSINE_INTERPOLATION>::generate(){
  float pts[2];
  points->setDelay(2);
  points->read(pts, 2);
  float y = Interpolator::cosine(pts[0], pts[1], mu);
  mu += inc;
  return y;
}

template<>
void InterpolatingSignalGenerator<COSINE_INTERPOLATION>::generate(FloatArray output){
  float pts[2];
  points->setDelay(2);
  points->read(pts, 2);
  size_t len = output.getSize();
  float* out = output.getData();
  while(len--){
    *out++ = Interpolator::cosine(pts[0], pts[1], mu);
    mu += inc;
  }
}

template<>
InterpolatingSignalGenerator<COSINE_INTERPOLATION>*
InterpolatingSignalGenerator<COSINE_INTERPOLATION>::create(){
  return allocate(2);
}

template<>
float InterpolatingSignalGenerator<CUBIC_3P_INTERPOLATION>::generate(){
  float pts[3];
  points->setDelay(3);
  points->read(pts, 3);
  float y = Interpolator::cubic(pts[0], pts[1], pts[2], mu);
  mu += inc;
  return y;
}

template<>
void InterpolatingSignalGenerator<CUBIC_3P_INTERPOLATION>::generate(FloatArray output){
  float pts[3];
  points->setDelay(3);
  points->read(pts, 3);
  size_t len = output.getSize();
  float* out = output.getData();
  while(len--){
    *out++ = Interpolator::cubic(pts[0], pts[1], pts[2], mu);
    mu += inc;
  }
}

template<>
InterpolatingSignalGenerator<CUBIC_3P_INTERPOLATION>*
InterpolatingSignalGenerator<CUBIC_3P_INTERPOLATION>::create(){
  return allocate(3);
}

template<>
float InterpolatingSignalGenerator<CUBIC_4P_INTERPOLATION>::generate(){
  float pts[4];
  points->setDelay(4);
  points->read(pts, 4);
  float y = Interpolator::cubic(pts[0], pts[1], pts[2], pts[3], mu);
  mu += inc;
  return y;
}

template<>
void InterpolatingSignalGenerator<CUBIC_4P_INTERPOLATION>::generate(FloatArray output){
  float pts[4];
  points->setDelay(4);
  points->read(pts, 4);
  size_t len = output.getSize();
  float* out = output.getData();
  while(len--){
    *out++ = Interpolator::cubic(pts[0], pts[1], pts[2], pts[3], mu);
    mu += inc;
  }
}

template<>
InterpolatingSignalGenerator<CUBIC_4P_INTERPOLATION>*
InterpolatingSignalGenerator<CUBIC_4P_INTERPOLATION>::create(){
  return allocate(4);
}

template<>
float InterpolatingSignalGenerator<CUBIC_4P_SMOOTH_INTERPOLATION>::generate(){
  float pts[4];
  points->setDelay(4);
  points->read(pts, 4);
  float y = Interpolator::cubicSmooth(pts[0], pts[1], pts[2], pts[3], mu);
  mu += inc;
  return y;
}

template<>
void InterpolatingSignalGenerator<CUBIC_4P_SMOOTH_INTERPOLATION>::generate(FloatArray output){
  float pts[4];
  points->setDelay(4);
  points->read(pts, 4);
  size_t len = output.getSize();
  float* out = output.getData();
  while(len--){
    *out++ = Interpolator::cubicSmooth(pts[0], pts[1], pts[2], pts[3], mu);
    mu += inc;
  }
}

template<>
InterpolatingSignalGenerator<CUBIC_4P_SMOOTH_INTERPOLATION>*
InterpolatingSignalGenerator<CUBIC_4P_SMOOTH_INTERPOLATION>::create(){
  return allocate(4);
}

template<>
float InterpolatingSignalGenerator<HERMITE_INTERPOLATION>::generate(){
  float pts[4];
  points->setDelay(4);
  points->read(pts, 4);
  float y = Interpolator::hermite(pts[0], pts[1], pts[2], pts[3], mu);
  mu += inc;
  return y;
}

template<>
void InterpolatingSignalGenerator<HERMITE_INTERPOLATION>::generate(FloatArray output){
  float pts[4];
  points->setDelay(4);
  points->read(pts, 4);
  size_t len = output.getSize();
  float* out = output.getData();
  while(len--){
    *out++ = Interpolator::hermite(pts[0], pts[1], pts[2], pts[3], mu);
    mu += inc;
  }
}

template<>
InterpolatingSignalGenerator<HERMITE_INTERPOLATION>*
InterpolatingSignalGenerator<HERMITE_INTERPOLATION>::create(){
  return allocate(4);
}

#endif // __InterpolatingSignalGenerator_h__
