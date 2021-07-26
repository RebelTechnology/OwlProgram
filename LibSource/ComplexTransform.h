#ifndef __COMPLEX_TRANSFORM_H__
#define __COMPLEX_TRANSFORM_H__

#include "basicmaths.h"
#include "SignalProcessor.h"
#include "FloatMatrix.h"

template <size_t matrix_order>
class AbstractTransform {
public:
    AbstractTransform() = default;
    AbstractTransform(FloatMatrix matrix) {
        this->matrix = matrix;
        resetMatrix();
    }
    FloatMatrix getMatrix() {
        return matrix;
    }
    void resetMatrix() {
        matrix.clear();
        for (size_t i = 0; i < matrix_order; i++) {
            matrix[i][i] = 1;
        }
    }
protected:
    FloatMatrix matrix;
};

template <size_t matrix_order, typename Operation>
class ComplexTransformTemplate : public AbstractTransform<matrix_order>,
                                 public ComplexSignalProcessor {
public:
    using AbstractTransform<matrix_order>::AbstractTransform;
    using AbstractTransform<matrix_order>::matrix;
    using ComplexSignalProcessor::process;



    ComplexFloat process(ComplexFloat input) {
        if constexpr (matrix_order == 2) {
            return ComplexFloat(input.re * matrix[0][0] + input.im * matrix[0][1],
                input.re * matrix[1][0] + input.im * matrix[1][1]);
        }
        else if constexpr (matrix_order == 3) {
            return ComplexFloat(
                input.re * matrix[0][0] + input.im * matrix[0][1] + matrix[0][2],
                input.re * matrix[1][0] + input.im * matrix[1][1] + matrix[1][2]);
        }
        else if constexpr (matrix_order > 3) {
            ComplexFloat result {
                input.re * matrix[0][0] + input.im * matrix[0][1],
                input.re * matrix[1][0] + input.im * matrix[1][1]};
            for (size_t i = 2; i < matrix_order; i++) {
                input.re += matrix[0][i];
                input.im += matrix[0][i];
            }
        }
        else {
            return ComplexFloat {};
        }
    }

    static Operation* create() {
        FloatMatrix matrix = FloatMatrix::create(matrix_order, matrix_order);
        Operation* op = new Operation(matrix);
        return op;
    }
    static void destroy(Operation* transform) {
        FloatMatrix::destroy(transform->matrix);
        delete transform;
    }
};

class LinearReflection2D : public ComplexTransformTemplate<2, LinearReflection2D> {
public:
    using ComplexTransformTemplate<2, LinearReflection2D>::ComplexTransformTemplate;
    void setAngle(float angle) {
        matrix[0][0] = cos(angle * 2);
        matrix[1][0] = sin(angle * 2);
        matrix[0][1] = matrix[0][1];
        matrix[1][1] = -matrix[0][0];
    }
};

class AffineReflection2D : public ComplexTransformTemplate<3, AffineReflection2D> {
public:
    using ComplexTransformTemplate<3, AffineReflection2D>::ComplexTransformTemplate;
    void setAngle(float angle) {
        matrix[0][0] = cos(angle * 2);
        matrix[0][1] = sin(angle * 2);
        matrix[1][0] = matrix[0][1];
        matrix[1][1] = -matrix[0][0];
    }
};

class LinearRotation2D : public ComplexTransformTemplate<2, LinearRotation2D> {
public:
    using ComplexTransformTemplate<2, LinearRotation2D>::ComplexTransformTemplate;
    void setAngle(float angle) {
        matrix[0][0] = cos(angle);
        matrix[1][0] = sin(angle);
        matrix[0][1] = -matrix[1][0];
        matrix[1][1] = matrix[0][0];
    }
};

class AffineRotation2D : public ComplexTransformTemplate<3, AffineRotation2D> {
public:
    using ComplexTransformTemplate<3, AffineRotation2D>::ComplexTransformTemplate;
    void setAngle(float angle) {
        matrix[0][0] = cos(angle);
        matrix[1][0] = sin(angle);
        matrix[0][1] = -matrix[1][0];
        matrix[1][1] = matrix[0][0];
    }
};

class LinearScale2D : public ComplexTransformTemplate<2, LinearScale2D> {
public:
    using ComplexTransformTemplate<2, LinearScale2D>::ComplexTransformTemplate;
    void setDirection(ComplexFloat vector) {
        matrix[0][0] = vector.re;
        matrix[1][1] = vector.im;
    }
    void setFactor(float factor) {
        matrix[0][0] = factor;
        matrix[1][1] = factor;
    }
};

class AffineScale2D : public ComplexTransformTemplate<3, AffineScale2D> {
public:
    using ComplexTransformTemplate<3, AffineScale2D>::ComplexTransformTemplate;
    void setDirection(ComplexFloat vector) {
        matrix[0][0] = vector.re;
        matrix[1][1] = vector.im;
    }
    void setFactor(float factor) {
        matrix[0][0] = factor;
        matrix[1][1] = factor;
    }
};

class LinearStretch2D : public ComplexTransformTemplate<2, LinearStretch2D> {
public:
    using ComplexTransformTemplate<2, LinearStretch2D>::ComplexTransformTemplate;
    void setDirection(ComplexFloat vector) {
        matrix[0][0] = vector.re;
        matrix[1][1] = vector.im;
    }
};

class AffineStretch2D : public ComplexTransformTemplate<3, AffineStretch2D> {
public:
    using ComplexTransformTemplate<3, AffineStretch2D>::ComplexTransformTemplate;
    void setDirection(ComplexFloat vector) {
        matrix[0][0] = vector.re;
        matrix[1][1] = vector.im;
    }
};

class LinearSqueeze2D : public ComplexTransformTemplate<2, LinearSqueeze2D> {
public:
    using ComplexTransformTemplate<2, LinearSqueeze2D>::ComplexTransformTemplate;
    void setFactor(float scale) {
        matrix[0][0] = scale;
        matrix[1][1] = 1.f / scale;
    }
};

class LinearShearX2D : public ComplexTransformTemplate<2, LinearShearX2D> {
public:
    using ComplexTransformTemplate<2, LinearShearX2D>::ComplexTransformTemplate;
    void setFactor(float scale) {
        matrix[0][1] = scale;
    }
};

class AffineShearX2D : public ComplexTransformTemplate<3, AffineShearX2D> {
public:
    using ComplexTransformTemplate<3, AffineShearX2D>::ComplexTransformTemplate;
    void setFactor(float scale) {
        matrix[0][1] = scale;
    }
};

class LinearShearY2D : public ComplexTransformTemplate<2, LinearShearY2D> {
public:
    using ComplexTransformTemplate<2, LinearShearY2D>::ComplexTransformTemplate;
    void setFactor(float scale) {
        matrix[1][0] = scale;
    }
};

class AffineShearY2D : public ComplexTransformTemplate<3, AffineShearY2D> {
public:
    using ComplexTransformTemplate<3, AffineShearY2D>::ComplexTransformTemplate;
    void setFactor(float scale) {
        matrix[1][0] = scale;
    }
};

class AffineTranslate2D : public ComplexTransformTemplate<3, AffineTranslate2D> {
public:
    using ComplexTransformTemplate<3, AffineTranslate2D>::ComplexTransformTemplate;
    void setDirection(ComplexFloat vector) {
        matrix[2][0] = vector.re;
        matrix[2][1] = vector.im;
    }
};

template <size_t matrix_order>
class CompositeTransform
    : public ComplexTransformTemplate<matrix_order, CompositeTransform<matrix_order>> {
public:
    using BaseTransform = AbstractTransform<matrix_order>;
    using BaseTransform::getMatrix;
    using BaseTransform::matrix;

    CompositeTransform(FloatMatrix matrix, size_t num_transforms, BaseTransform** transforms)
        : ComplexTransformTemplate<matrix_order, CompositeTransform<matrix_order>>(matrix)
        , transforms(transforms)
        , num_transforms(num_transforms) {
    }

    void process(ComplexFloatArray input, ComplexFloatArray output) {
        computeMatrix();
        ComplexTransformTemplate<matrix_order, CompositeTransform<matrix_order>>::process(
            input, output);
    }
    void computeMatrix() {
        FloatMatrix m = this->matrix;
        m.copyFrom(transforms[0]->getMatrix());
        for (size_t i = 1; i < num_transforms; i++) {
            m.multiply(transforms[i]->getMatrix());
        }
    }

    void setTransform(size_t position, BaseTransform* transform) {
        transforms[position] = transform;
    }

    static CompositeTransform* create(size_t transforms) {
        FloatMatrix matrix = FloatMatrix::create(matrix_order, matrix_order);
        BaseTransform** transform_pointers = new BaseTransform*[transforms];
        return new CompositeTransform(matrix, transforms, transform_pointers);
    }

    static void destroy(CompositeTransform* transform) {
        FloatMatrix::destroy(transform->matrix);
        delete[] transform->transforms;
        delete transform;
    }

protected:
    BaseTransform** transforms;
    size_t num_transforms;
};

template <size_t matrix_order>
class InterpolatedCompositeTransform
    : public ComplexTransformTemplate<matrix_order, CompositeTransform<matrix_order>> {
public:
    using BaseTransform = AbstractTransform<matrix_order>;
    using BaseTransform::getMatrix;
    using BaseTransform::matrix;
    using ComplexTransformTemplate<matrix_order, CompositeTransform<matrix_order>>::process;

    InterpolatedCompositeTransform(FloatMatrix matrix, FloatMatrix prev_matrix,
        FloatMatrix delta, size_t num_transforms, BaseTransform** transforms)
        : ComplexTransformTemplate<matrix_order, CompositeTransform<matrix_order>>(matrix)
        , transforms(transforms)
        , num_transforms(num_transforms)
        , prev_matrix(prev_matrix)
        , delta(delta) {
    }

    void process(ComplexFloatArray input, ComplexFloatArray output) {
        computeMatrix();
        float mult = 1.f / input.getSize();
        for (size_t i = 0; i < matrix_order; i++) {
            for (size_t j = 0; j < matrix_order; j++) {
                delta[i][j] = (matrix[i][j] - prev_matrix[i][j]) * mult;
            }
        }

        matrix.copyFrom(prev_matrix);
        for (size_t i = 0; i < input.getSize(); i++) {
            matrix.add(delta);
            output[i] = process(input[i]);
        }
        prev_matrix.copyFrom(matrix);
    }
    void computeMatrix() {
        FloatMatrix m = this->matrix;

        m.copyFrom(transforms[num_transforms - 1]->getMatrix());
        for (int i = num_transforms - 2; i >= 0; i--) {
            m.multiply(transforms[i]->getMatrix());
        }
    }

    void setTransform(size_t position, BaseTransform* transform) {
        transforms[position] = transform;
    }

    static InterpolatedCompositeTransform* create(size_t transforms) {
        FloatMatrix matrix = FloatMatrix::create(matrix_order, matrix_order);
        BaseTransform** transform_pointers = new BaseTransform*[transforms];
        FloatMatrix prev_matrix = FloatMatrix::create(matrix_order, matrix_order);
        FloatMatrix delta = FloatMatrix::create(matrix_order, matrix_order);
        return new InterpolatedCompositeTransform(
            matrix, prev_matrix, delta, transforms, transform_pointers);
    }

    static void destroy(InterpolatedCompositeTransform* transform) {
        FloatMatrix::destroy(transform->matrix);
        FloatMatrix::destroy(transform->prev_matrix);
        FloatMatrix::destroy(transform->delta);
        delete[] transform->transforms;
        delete transform;
    }

protected:
    BaseTransform** transforms;
    size_t num_transforms;
    FloatMatrix prev_matrix, delta;
};

#endif
