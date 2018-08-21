from time import time
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import argparse
import sys
from py_polynomial import (univariate_xtensor,
                           univariate_xtensor_5,
                           univariate_xtensor_10,
                           univariate_xtensor_15,
                           univariate_xtensor_20,
                           univariate_eigen,
                           univariate_eigen_5,
                           univariate_eigen_10,
                           univariate_eigen_15,
                           univariate_eigen_20)

class univariate_numpy:

    def __init__(self, coeff):
        self.coeff = np.copy(coeff[::-1])

    def polyval(self, x):
        return np.polyval(self.coeff, x)

def benchmark(fun, max_time, *args):
    t0 = time()
    fun(*args)
    dt = time() - t0

    n_samples = int(max_time / dt)

    t0 = time()
    for _ in range(n_samples):
        fun(*args)
    dt = time() - t0

    return dt / n_samples, dt, n_samples

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Polynomial benchmark")
    parser.add_argument("--orders", type=int, choices=[5,10,15,20], default=[5,10,15,20], nargs="+")
    parser.add_argument("--libraries", choices=["numpy", "xtensor", "eigen"], default=["numpy", "xtensor", "eigen"] , nargs="+")
    parser.add_argument("--implementations", choices=["dynamic", "fixed"], default=["dynamic", "fixed"], nargs="+")
    parser.add_argument("--methods", choices=["iterative", "recursive"], default=["iterative", "recursive"], nargs="+")
    parser.add_argument("--min-num-x", type=int, default=10)
    parser.add_argument("--max-num-x", type=int, default=100000)
    parser.add_argument("--num-num-x", type=int, default=20)
    parser.add_argument("--max-time", type=float, default=0.3)
    parser.add_argument("--with-xsimd", action="store_true")
    parser.add_argument("--with-march-native", action="store_true")
    parser.add_argument("--xsimd-version")
    parser.add_argument("--xtensor-version")
    parser.add_argument("--xtensor-python-version")
    parser.add_argument("--eigen-version")
    parser.add_argument("--pybind11-version")
    parser.add_argument("--output-prefix", default="")

    args = parser.parse_args()

    impls = []

    if "numpy" in args.libraries:
        impls.append(
            {
                "label": "numpy.polyval",
                "object": univariate_numpy,
                "method": univariate_numpy.polyval
            }
        )

    non_numpy_libraries = [library for library in args.libraries if library != "numpy"]
    for library in non_numpy_libraries:
        if "dynamic" in args.implementations:
            for method in args.methods:
                impls.append(
                    {
                        "label": "{}_{}_dynamic".format(library, method),
                        "object": globals()["univariate_{}".format(library)],
                        "method": getattr(globals()["univariate_{}".format(library)], "{}_eval".format(method))
                    }
                )

        if "fixed" in args.implementations:
            for order in args.orders:
                for method in args.methods:
                    impls.append(
                        {
                            "label": "{}_{}_fixed".format(library, method),
                            "object": globals()["univariate_{}_{}".format(library, order)],
                            "method": getattr(globals()["univariate_{}_{}".format(library, order)], "{}_eval".format(method)),
                            "order": order
                        }
                    )



    versions_string = "python v{}.{}.{}{}{}{}{}{}".format(
        sys.version_info.major,
        sys.version_info.minor,
        sys.version_info.micro,
        ", xsimd v{}".format(args.xsimd_version) if args.xsimd_version is not None and args.with_xsimd else "",
        ", xtensor v{}".format(args.xtensor_version) if args.xtensor_version is not None and "xtensor" in args.libraries else "",
        ", xtensor-python v{}".format(args.xtensor_python_version) if args.xtensor_python_version is not None and "xtensor" in args.libraries else "",
        ", eigen v{}".format(args.eigen_version) if args.eigen_version is not None and "xtensor" in args.libraries else "",
        ", pybind11 v{}".format(args.pybind11_version) if args.pybind11_version is not None else "",
    )

    np.random.seed(0)

    n_xs = np.linspace(args.min_num_x,args.max_num_x,args.num_num_x,dtype=np.int)
    xs = [np.random.rand(n_x) for n_x in n_xs]

    plt.figure(figsize=(10,5))

    for order in args.orders:
        coeff = np.random.rand(order)

        for impl in impls:

            if "order" in impl and impl["order"] != order:
                continue

            obj = impl["object"](coeff)
            mean_dt = np.empty(len(xs))

            for idx, x in enumerate(xs):

                # Make sure the implementations evaluates correctly
                res_ref = np.polyval(coeff[::-1], x)
                res_impl = impl["method"](obj, x)

                if not np.allclose(res_ref, res_impl):
                    raise RuntimeError("Evaluation of {} failed! Solution does not match the reference solution.".format(impl["label"]))

                res = benchmark(impl["method"], args.max_time, obj, x)
                print("order = {:2}, impl = {:25}, num_x = {:6}, avg_dt = {:.2e}, dt = {:.2e}, samples = {:6}".format(order, impl["label"], x.size, *res))
                mean_dt[idx] = res[0]

            plt.plot(n_xs, mean_dt, "x--", label=impl["label"])

        plt.xlabel("number of evaluation points / #")
        plt.ylabel("average wall clock time / s")
        plt.gca().get_yaxis().set_major_formatter(FuncFormatter(lambda x, _: "{:.0e}".format(x)))
        plt.grid()
        plt.legend()
        plt.gcf().suptitle("order = {}{}{}".format(order, ", native" if args.with_march_native else "", ", xsimd" if args.with_xsimd else ""))
        plt.gca().set_title(versions_string, fontdict={"fontsize": 8})

        plt.savefig("{}{}{}{}.png".format(
            args.output_prefix,
            order,
            "_xsimd" if args.with_xsimd and "xtensor" in args.libraries else "",
            "_native" if args.with_march_native else ""
        ))
        plt.clf()





