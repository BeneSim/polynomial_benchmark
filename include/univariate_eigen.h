#ifndef UNIVARIATE_EIGEN_H
#define UNIVARIATE_EIGEN_H

#include <Eigen/Dense>

template <int N = Eigen::Dynamic>
class univariate_eigen {

public:

  using container_type = Eigen::Matrix<double, N, 1>;

private:

  template <int Idx>
  auto recursive_eval_impl(Eigen::Matrix<double, Eigen::Dynamic, 1> const& x) const {
    if constexpr (Idx == N - 1) {
      return coeff_[Idx];
    } else {
      return coeff_[Idx] + x.array() * recursive_eval_impl<Idx+1>(x);
    }
  }

  Eigen::Matrix<double, Eigen::Dynamic, 1> recursive_eval_impl(Eigen::Matrix<double, Eigen::Dynamic, 1> const& x, int idx) const {
    if (idx == coeff_.size() - 1) {
      return Eigen::Matrix<double, Eigen::Dynamic, 1>::Constant(x.size(), coeff_[idx]);
    }
    return coeff_[idx] + x.array() * recursive_eval_impl(x, idx+1).array();
  }

public:

  univariate_eigen(container_type const& coeff) : coeff_(coeff) {}

  Eigen::Matrix<double, Eigen::Dynamic, 1> recursive_eval(Eigen::Matrix<double, Eigen::Dynamic, 1> const& x) const {
    if constexpr (N == Eigen::Dynamic) {
      return recursive_eval_impl(x, 0);
    } else {
      return recursive_eval_impl<0>(x);
    }
  }

  Eigen::Matrix<double, Eigen::Dynamic, 1> iterative_eval(Eigen::Matrix<double, Eigen::Dynamic, 1> const& x) const {

    if constexpr (N == Eigen::Dynamic) {
      Eigen::Matrix<double, Eigen::Dynamic, 1> res = Eigen::Matrix<double, Eigen::Dynamic, 1>::Constant(x.size(), coeff_[coeff_.size() - 1]);
      for (int idx = coeff_.size() - 2; idx > -1; --idx) {
        res.array() = coeff_[idx] + x.array() * res.array();
      }

      return res;
    } else {
      Eigen::Matrix<double, Eigen::Dynamic, 1> res = Eigen::Matrix<double, Eigen::Dynamic, 1>::Constant(x.size(), coeff_[N - 1]);
      for (int idx = N - 2; idx > -1; --idx) {
        res.array() = coeff_[idx] + x.array() * res.array();
      }

      return res;
    }

   }

private:

  container_type coeff_;
};

#endif // UNIVARIATE_EIGEN_H
