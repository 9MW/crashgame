#pragma once
#define EIGEN_MATRIXBASE_PLUGIN "MatrixBaseAddons.h"
#include <Eigen/Dense>

inline Scalar& at(uint i) { return this->operator[](i); }
