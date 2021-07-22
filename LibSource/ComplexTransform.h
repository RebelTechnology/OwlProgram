#ifndef __COMPLEX_TRANSFORM_H__
#define __COMPLEX_TRANSFORM_H__

#include "basicmaths.h"
#include "SignalProcessor.h"

template <typename Operation, size_t matrix_dim>
class ComplexTransformTemplate : public ComplexSignalProcessor {
public:
    using ComplexSignalProcessor::process;
    ComplexTransformTemplate() {
        memset((void*)&matrix[0][0], 0, sizeof(float) * matrix_dim * matrix_dim);
    }
    ComplexFloat process(ComplexFloat input) {
        // We can avoid template specialization in C++17 thanks to if constexpr() support
        if constexpr (matrix_dim == 2) {
            return ComplexFloat(
                matrix[0][0] * input.re + matrix[0][1] * input.re,
                matrix[1][0] * input.im + matrix[1][1] * input.im);
        }
        else {
            return ComplexFloat(
                matrix[0][0] * input.re + matrix[0][1] * input.re + matrix[0][2] * input.re,
                matrix[1][0] * input.im + matrix[1][1] * input.im + matrix[1][2] * input.im);
        }
    }
    static Operation* create() {
        return new Operation();
    }
    static void destroy(Operation* transform) {
        delete transform;
    }

protected:
    float matrix[matrix_dim][matrix_dim];
};

class Reflection2D : public ComplexTransformTemplate<Reflection2D, 2> {
public:
    Reflection2D() {
        setAngle(0);
    }
    void setAngle(float angle) {
        matrix[0][0] = cos(angle * 2);
        matrix[0][1] = sin(angle * 2);
        matrix[1][0] = matrix[0][1];
        matrix[1][1] = -matrix[0][0];
    }
};

class Rotation2D : public ComplexTransformTemplate<Rotation2D, 2> {
public:
    void setAngle(float angle) {
        matrix[0][0] = cos(angle);
        matrix[1][0] = sin(angle);
        matrix[0][1] = -matrix[1][0];
        matrix[1][1] = matrix[0][0];
    }
};

class Scale2D : public ComplexTransformTemplate<Scale2D, 2> {
public:
    void setDirection(ComplexFloat vector) {
        matrix[0][0] = vector.re;
        matrix[1][1] = vector.im;
    }
    void setFactor(float factor) {
        matrix[0][0] = factor;
        matrix[1][1] = factor;
    }
};

class StretchX2D : public ComplexTransformTemplate<StretchX2D, 2> {
public:
    StretchX2D() {
        matrix[1][1] = 1;
    }

    void setFactor(float scale) {
        matrix[0][0] = scale;
    }
};

class StretchY2D : public ComplexTransformTemplate<StretchY2D, 2> {
public:
    StretchY2D() {
        matrix[0][0] = 1;
    }

    void setFactor(float scale) {
        matrix[1][1] = scale;
    }
};

class Squeeze2D : public ComplexTransformTemplate<Squeeze2D, 2> {
public:
    void setFactor(float scale) {
        matrix[0][0] = scale;
        matrix[1][1] = 1.f / scale;
    }
};

class ShearX2D : public ComplexTransformTemplate<ShearX2D, 2> {
public:
    ShearX2D() {
        matrix[0][0] = 1;
        matrix[1][1] = 1;
    }

    void setFactor(float scale) {
        matrix[0][1] = scale;
    }
};

class ShearY2D : public ComplexTransformTemplate<ShearY2D, 2> {
public:
    ShearY2D() {
        matrix[0][0] = 1;
        matrix[1][1] = 1;
    }

    void setFactor(float scale) {
        matrix[1][0] = scale;
    }
};

class Translate2D : public ComplexTransformTemplate<Translate2D, 3> {
public:
    Translate2D() {
        matrix[0][0] = 1;
        matrix[1][1] = 1;
        matrix[2][2] = 1;
    }

    void setDirection(ComplexFloat vector) {
        matrix[0][2] = vector.re;
        matrix[1][2] = vector.im;
    }
};

#endif
