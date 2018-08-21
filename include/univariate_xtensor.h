#ifndef UNIVARIATE_XTENSOR_H
#define UNIVARIATE_XTENSOR_H

#include <xtensor/xtensor.hpp>
#include <xtensor/xfixed.hpp>
#include <xtensor/xnoalias.hpp>

#ifdef USE_LAMBDA_XFUNCTION
template <typename Container, std::size_t N, std::size_t Idx>
struct eval_poly {
  eval_poly(Container const& coeff) : coeff_(coeff) {}

  template <class X>
  auto operator()(X x) const -> decltype(x + x) {
    if constexpr (Idx == N - 1) {
      return X(coeff_[Idx]);
    } else {
      return coeff_[Idx] + x * eval_poly<Container, N, Idx+1>(coeff_)(x);
    }
  }

  Container const& coeff_;
};
#endif // USE_LAMBDA_XFUNCTION

template <int N = -1>
class univariate_xtensor {

public:

  using container_type = std::conditional_t<N == -1, xt::xtensor<double, 1>, xt::xtensor_fixed<double, xt::xshape<static_cast<std::size_t>(N)>>>;

private:

  template <class E, int Idx>
  auto recursive_eval_impl(xt::xexpression<E> const& x) const {
    if constexpr (Idx == N - 1) {
      return coeff_[Idx];
    } else {
      return coeff_[Idx] + x.derived_cast() * recursive_eval_impl<E, Idx+1>(x);
    }
  }

  template <class E>
  xt::xtensor<double, 1> recursive_eval_impl(xt::xexpression<E> const& x, int idx) const {
    if (idx == coeff_.size() - 1) {
      xt::xtensor<double, 1> res = xt::empty<double>(x.derived_cast().shape());
      res.fill(coeff_[idx]);
      return res;
    }
    return coeff_[idx] + x.derived_cast() * recursive_eval_impl(x, idx+1);
  }

public:

  template <typename E>
  univariate_xtensor(xt::xexpression<E> const& coeff) : coeff_(coeff) {}

  template <typename E>
  auto recursive_eval(xt::xexpression<E> const& x) const {
    if constexpr (N == -1) {
      return recursive_eval_impl(x, 0);
    } else {
#ifdef USE_LAMBDA_XFUNCTION
      return make_lambda_xfunction(eval_poly<container_type, N, 0>(coeff_), x.derived_cast());
#else
      return recursive_eval_impl<E, 0>(x);
#endif // USE_LAMBDA_X_FUNCTION
    }
  }

  template <typename E>
  xt::xtensor<double, 1> iterative_eval(xt::xexpression<E> const& x) const {
    xt::xtensor<double, 1> res = xt::empty<double>(x.derived_cast().shape());

    if constexpr (N == -1) {
      res.fill(coeff_[coeff_.size() - 1]);
      for (int idx = coeff_.size() - 2; idx > -1; --idx) {
        xt::noalias(res) = coeff_[idx] + x.derived_cast() * res;
      }
    } else {
      res.fill(coeff_[N - 1]);
      for (int idx = N - 2; idx > -1; --idx) {
        xt::noalias(res) = coeff_[idx] + x.derived_cast() * res;
      }
    }

    return res;
  }

private:

  container_type coeff_;
};

#endif // UNIVARIATE_XTENSOR_H
