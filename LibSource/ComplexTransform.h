#ifndef __COMPLEX_TRANSFORM_H__
#define __COMPLEX_TRANSFORM_H__

#include "basicmaths.h"
#include "SignalProcessor.h"
#include "FloatMatrix.h"

template <size_t matrix_order>
class AbstractMatrix {
public:
    AbstractMatrix() = default;
    AbstractMatrix(FloatMatrix matrix) {
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

/**
 * This template creates classes that can manage a single transformation matrix
 **/
template <size_t matrix_order, typename Operation>
class TransformationMatrix : public AbstractMatrix<matrix_order>,
                             public ComplexSignalProcessor {
public:
    using AbstractMatrix<matrix_order>::AbstractMatrix;
    using AbstractMatrix<matrix_order>::matrix;
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
            ComplexFloat result { input.re * matrix[0][0] + input.im * matrix[0][1],
                input.re * matrix[1][0] + input.im * matrix[1][1] };
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
        return new Operation(matrix);
    }
    static void destroy(Operation* transform) {
        FloatMatrix::destroy(transform->matrix);
        delete transform;
    }
};

template <size_t matrix_order>
class Rotation2D
    : public TransformationMatrix<matrix_order, Rotation2D<matrix_order>> {
public:
    using TransformationMatrix<matrix_order, Rotation2D<matrix_order>>::TransformationMatrix;
    void rotate(float angle) {
        FloatMatrix matrix =
            static_cast<AbstractMatrix<matrix_order>*>(this)->getMatrix();
        matrix[0][0] = cos(angle);
        matrix[1][0] = sin(angle);
    }
    void rotate(ComplexFloat angle) {
        FloatMatrix matrix =
            static_cast<AbstractMatrix<matrix_order>*>(this)->getMatrix();
        matrix[0][0] = angle.re;
        matrix[1][0] = angle.im;
    }
};

using LinearRotation2D = Rotation2D<2>;
using AffineRotation2D = Rotation2D<3>;

template <size_t matrix_order>
class Reflection2D
    : public TransformationMatrix<matrix_order, Reflection2D<matrix_order>> {
public:
    using TransformationMatrix<matrix_order, Reflection2D<matrix_order>>::TransformationMatrix;
    void reflect(float angle) {
        FloatMatrix matrix =
            static_cast<AbstractMatrix<matrix_order>*>(this)->getMatrix();
        matrix[0][0] = cos(angle * 2);
        matrix[0][1] = sin(angle * 2);
    }
};

using LinearReflection2D = Reflection2D<2>;
using AffineReflection2D = Reflection2D<3>;

template <size_t matrix_order>
class Scale2D : public TransformationMatrix<matrix_order, Scale2D<matrix_order>> {
public:
    using TransformationMatrix<matrix_order, Scale2D<matrix_order>>::TransformationMatrix;
    void scale(ComplexFloat vector) {
        FloatMatrix matrix =
            static_cast<AbstractMatrix<matrix_order>*>(this)->getMatrix();
        matrix[0][0] = vector.re;
        matrix[1][1] = vector.im;
    }
    void scale(float factor) {
        FloatMatrix matrix =
            static_cast<AbstractMatrix<matrix_order>*>(this)->getMatrix();
        matrix[0][0] = factor;
        matrix[1][1] = factor;
    }
};

using LinearScale2D = Scale2D<2>;
using AffineScale2D = Scale2D<3>;

template <size_t matrix_order>
class Stretch2D
    : public TransformationMatrix<matrix_order, Stretch2D<matrix_order>> {
public:
    using TransformationMatrix<matrix_order, Stretch2D<matrix_order>>::TransformationMatrix;
    void stretch(ComplexFloat vector) {
        stretch(vector.re, vector.im);
    }
    void stretch(float re, float im) {
        FloatMatrix matrix =
            static_cast<AbstractMatrix<matrix_order>*>(this)->getMatrix();
        matrix[0][0] = re;
        matrix[1][1] = im;
    }
};

using LinearStretch2D = Stretch2D<2>;
using AffineStretch2D = Stretch2D<3>;

template <size_t matrix_order>
class Squeeze2D
    : public TransformationMatrix<matrix_order, Squeeze2D<matrix_order>> {
public:
    using TransformationMatrix<matrix_order, Squeeze2D<matrix_order>>::TransformationMatrix;
    void squeeze(float scale) {
        FloatMatrix matrix =
            static_cast<AbstractMatrix<matrix_order>*>(this)->getMatrix();
        matrix[0][0] = scale;
        matrix[1][1] = 1.f / scale;
    }
};

using LinearSqueeze2D = Squeeze2D<2>;
using AffineSqueeze2D = Squeeze2D<3>;

template <size_t matrix_order>
class Shear2D : public TransformationMatrix<matrix_order, Shear2D<matrix_order>> {
public:
    using TransformationMatrix<matrix_order, Shear2D<matrix_order>>::TransformationMatrix;
    void shearX(float scale) {
        FloatMatrix matrix =
            static_cast<AbstractMatrix<matrix_order>*>(this)->getMatrix();
        matrix[0][1] = scale;
    }
    void shearY(float scale) {
        FloatMatrix matrix =
            static_cast<AbstractMatrix<matrix_order>*>(this)->getMatrix();
        matrix[1][0] = scale;
    }
};

using LinearShear2D = Shear2D<2>;
using AffineShear2D = Shear2D<3>;

template <size_t matrix_order>
class Translation2D
    : public TransformationMatrix<matrix_order, Translation2D<matrix_order>> {
public:
    using TransformationMatrix<matrix_order, Translation2D<matrix_order>>::TransformationMatrix;
    // template <std::size_t size = MatrixSource::matrix_order>
    typename std::enable_if<matrix_order == 3>::type translate(ComplexFloat vector) {
        translate(vector.re, vector.im);
    }
    typename std::enable_if<matrix_order == 3>::type translate(float re, float im) {
        FloatMatrix matrix =
            static_cast<AbstractMatrix<matrix_order>*>(this)->getMatrix();
        matrix[0][2] = re;
        matrix[1][2] = im;
    }
};
/**
 * Unlike other operations, translation can't be performed with matrix order below 3
 **/
using AffineTranslation2D = Translation2D<3>;

/**
 * Composite transform can apply a sequence of transformation matrices. Block based version
 * calculates final transformation matrix just once per blocck rather than for every sample.
 **/
template <size_t matrix_order>
class CompositeTransform
    : public TransformationMatrix<matrix_order, CompositeTransform<matrix_order>> {
private:
    using MatrixType = AbstractMatrix<matrix_order>;

public:
    using TransformationMatrix<matrix_order, CompositeTransform<matrix_order>>::process;

    CompositeTransform() = default;
    CompositeTransform(FloatMatrix matrix, size_t num_transforms, MatrixType** transforms)
        : TransformationMatrix<matrix_order, CompositeTransform<matrix_order>>(matrix)
        , transforms(transforms)
        , num_transforms(num_transforms) {
    }

    ComplexFloat process(ComplexFloat input) {
        computeMatrix();
        return TransformationMatrix<matrix_order, CompositeTransform<matrix_order>>::process(input);
    }

    void process(ComplexFloatArray input, ComplexFloatArray output) {
        computeMatrix();
        TransformationMatrix<matrix_order, CompositeTransform<matrix_order>>::process(input, output);
    }
    void computeMatrix() {
        FloatMatrix m = this->matrix;
        m.copyFrom(transforms[0]->getMatrix());
        for (size_t i = 1; i < num_transforms; i++) {
            m.multiply(transforms[i]->getMatrix());
        }
    }

    template <typename... Transforms>
    static CompositeTransform* create(Transforms*... transform_ptrs) {
        FloatMatrix matrix = FloatMatrix::create(matrix_order, matrix_order);
        MatrixType** transforms = new MatrixType*[sizeof...(Transforms)];
        setTransform(transforms, 0, transform_ptrs...);
        return new CompositeTransform(matrix, sizeof...(Transforms), transforms);
    }
    static void destroy(CompositeTransform* transform) {
        FloatMatrix::destroy(transform->matrix);
        delete[] transform->transforms;
        delete transform;
    }

protected:
    MatrixType** transforms;
    size_t num_transforms;

    template <typename... Transforms>
    static void setTransform(MatrixType** transforms, size_t position,
        MatrixType* transform, Transforms*... transform_ptrs) {
        transforms[position] = transform;
        setTransform(transforms, position + 1, transform_ptrs...);
    }
    static void setTransform(MatrixType** transforms, size_t position) {
    }
};

/**
 * This class calculates transformation matrix once per audio block just like the CompositeTemplate.
 * In addition to that it calculates second matrix with delta values used to smoothly interpolate
 * between matrix states at audio rate. This adds one matrix addition per sample
 **/
template <size_t matrix_order>
class InterpolatedCompositeTransform
    : public TransformationMatrix<matrix_order, InterpolatedCompositeTransform<matrix_order>> {
private:
    using MatrixType = AbstractMatrix<matrix_order>;
    using MatrixType::matrix;

public:
    using TransformationMatrix<matrix_order, InterpolatedCompositeTransform<matrix_order>>::process;

    InterpolatedCompositeTransform(FloatMatrix matrix, FloatMatrix prev_matrix,
        FloatMatrix delta_matrix, size_t num_transforms, MatrixType** transforms)
        : TransformationMatrix<matrix_order, InterpolatedCompositeTransform<matrix_order>>(
              matrix)
        , transforms(transforms)
        , num_transforms(num_transforms)
        , prev_matrix(prev_matrix)
        , delta_matrix(delta_matrix) {
    }
    void process(ComplexFloatArray input, ComplexFloatArray output) {
        computeMatrix();
        float mult = 1.f / input.getSize();
        for (size_t i = 0; i < matrix_order; i++) {
            for (size_t j = 0; j < matrix_order; j++) {
                delta_matrix[i][j] = (matrix[i][j] - prev_matrix[i][j]) * mult;
            }
        }
        matrix.copyFrom(prev_matrix);
        for (size_t i = 0; i < input.getSize(); i++) {
            matrix.add(delta_matrix);
            output[i] = this->process(input[i]);
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
    template <typename... Transforms>
    static InterpolatedCompositeTransform* create(Transforms*... transform_ptrs) {
        FloatMatrix matrix = FloatMatrix::create(matrix_order, matrix_order);
        FloatMatrix prev_matrix = FloatMatrix::create(matrix_order, matrix_order);
        FloatMatrix delta_matrix = FloatMatrix::create(matrix_order, matrix_order);
        MatrixType** transforms = new MatrixType*[sizeof...(Transforms)];
        setTransform(transforms, 0, transform_ptrs...);
        return new InterpolatedCompositeTransform(matrix, prev_matrix,
            delta_matrix, sizeof...(Transforms), transforms);
    }
    static void destroy(InterpolatedCompositeTransform* transform) {
        FloatMatrix::destroy(transform->matrix);
        FloatMatrix::destroy(transform->prev_matrix);
        FloatMatrix::destroy(transform->delta_matrix);
        delete[] transform->transforms;
        delete transform;
    }

protected:
    MatrixType** transforms;
    size_t num_transforms;
    FloatMatrix prev_matrix, delta_matrix;

    template <typename... Transforms>
    static void setTransform(MatrixType** transforms, size_t position,
        MatrixType* transform, Transforms*... transform_ptrs) {
        transforms[position] = transform;
        setTransform(transforms, position + 1, transform_ptrs...);
    }

    static void setTransform(MatrixType** transforms, size_t position) {
    }
};
#endif
