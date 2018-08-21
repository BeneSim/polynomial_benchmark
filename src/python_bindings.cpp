#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>

#include <univariate_xtensor.h>
#include <univariate_eigen.h>

#define FORCE_IMPORT_ARRAY
#include <xtensor-python/pytensor.hpp>


namespace py = pybind11;

using py_tensor = xt::pytensor<double,1,xt::layout_type::row_major>;

PYBIND11_MODULE(py_polynomial, m) {
  xt::import_numpy();

  py::class_<univariate_xtensor<-1>>(m, "univariate_xtensor")
    .def(py::init([](xt::xexpression<py_tensor> const& coeff) {
          return std::make_unique<univariate_xtensor<-1>>(coeff);
        }))
    .def("iterative_eval", &univariate_xtensor<-1>::iterative_eval<py_tensor>)
    // Fails to deduce return type without the lambda
    .def("recursive_eval", [](univariate_xtensor<-1> const& u, py_tensor const& x) {
        return u.recursive_eval(x);
      });

  py::class_<univariate_xtensor<5>>(m, "univariate_xtensor_5")
    .def(py::init([](xt::xexpression<py_tensor> const& coeff) {
                    return std::make_unique<univariate_xtensor<5>>(coeff);
                  }))
    .def("iterative_eval", &univariate_xtensor<5>::iterative_eval<py_tensor>)
    .def("recursive_eval", [](univariate_xtensor<5> const& u, py_tensor const& x) {
                             return py_tensor(u.recursive_eval(x));
                           });

  py::class_<univariate_xtensor<10>>(m, "univariate_xtensor_10")
    .def(py::init([](xt::xexpression<py_tensor> const& coeff) {
          return std::make_unique<univariate_xtensor<10>>(coeff);
        }))
    .def("iterative_eval", &univariate_xtensor<10>::iterative_eval<py_tensor>)
    .def("recursive_eval", [](univariate_xtensor<10> const& u, py_tensor const& x) {
        return py_tensor(u.recursive_eval(x));
      });

  py::class_<univariate_xtensor<15>>(m, "univariate_xtensor_15")
    .def(py::init([](xt::xexpression<py_tensor> const& coeff) {
                    return std::make_unique<univariate_xtensor<15>>(coeff);
                  }))
    .def("iterative_eval", &univariate_xtensor<15>::iterative_eval<py_tensor>)
    .def("recursive_eval", [](univariate_xtensor<15> const& u, py_tensor const& x) {
                             return py_tensor(u.recursive_eval(x));
                           });

  py::class_<univariate_xtensor<20>>(m, "univariate_xtensor_20")
    .def(py::init([](xt::xexpression<py_tensor> const& coeff) {
          return std::make_unique<univariate_xtensor<20>>(coeff);
        }))
    .def("iterative_eval", &univariate_xtensor<20>::iterative_eval<py_tensor>)
    .def("recursive_eval", [](univariate_xtensor<20> const& u, py_tensor const& x) {
        return py_tensor(u.recursive_eval(x));
      });

  py::class_<univariate_eigen<Eigen::Dynamic>>(m, "univariate_eigen")
    .def(py::init<univariate_eigen<Eigen::Dynamic>::container_type const&>())
    .def("iterative_eval", &univariate_eigen<Eigen::Dynamic>::iterative_eval)
    .def("recursive_eval", &univariate_eigen<Eigen::Dynamic>::recursive_eval);

  py::class_<univariate_eigen<5>>(m, "univariate_eigen_5")
    .def(py::init<univariate_eigen<5>::container_type const&>())
    .def("iterative_eval", &univariate_eigen<5>::iterative_eval)
    .def("recursive_eval", &univariate_eigen<5>::recursive_eval);

  py::class_<univariate_eigen<10>>(m, "univariate_eigen_10")
    .def(py::init<univariate_eigen<10>::container_type const&>())
    .def("iterative_eval", &univariate_eigen<10>::iterative_eval)
    .def("recursive_eval", &univariate_eigen<10>::recursive_eval);

  py::class_<univariate_eigen<15>>(m, "univariate_eigen_15")
    .def(py::init<univariate_eigen<15>::container_type const&>())
    .def("iterative_eval", &univariate_eigen<15>::iterative_eval)
    .def("recursive_eval", &univariate_eigen<15>::recursive_eval);

  py::class_<univariate_eigen<20>>(m, "univariate_eigen_20")
    .def(py::init<univariate_eigen<20>::container_type const&>())
    .def("iterative_eval", &univariate_eigen<20>::iterative_eval)
    .def("recursive_eval", &univariate_eigen<20>::recursive_eval);
}
