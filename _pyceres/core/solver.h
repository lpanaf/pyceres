#pragma once

#include "_pyceres/core/callbacks.h"
#include "_pyceres/helpers.h"
#include "_pyceres/logging.h"

#include <ceres/ceres.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;
using ParameterBlockOrdering = ceres::ParameterBlockOrdering;
void BindParameterBlockOrdering(py::module& m) {
  py::class_<ParameterBlockOrdering, std::shared_ptr<ParameterBlockOrdering>>(
      m, "ParameterBlockOrdering")
      .def(py::init<>())
      .def("clear", &ParameterBlockOrdering::Clear)
      .def("reverse", &ParameterBlockOrdering::Reverse)
      .def("num_elements", &ParameterBlockOrdering::NumElements)
      .def("num_groups", &ParameterBlockOrdering::NumGroups)
      .def(
          "add_element_to_group",
          [](ParameterBlockOrdering& self,
             py::array_t<double>& values,
             const int group) {
            py::buffer_info info = values.request();
            return self.AddElementToGroup((double*)info.ptr, group);
          },
          py::arg("values").noconvert(),
          py::arg("group"))
      .def(
          "remove",
          [](ParameterBlockOrdering& self, py::array_t<double>& values) {
            py::buffer_info info = values.request();
            return self.Remove((double*)info.ptr);
          },
          py::arg("values").noconvert())
      .def(
          "group_id",
          [](ParameterBlockOrdering& self, py::array_t<double>& values) {
            py::buffer_info info = values.request();
            return self.GroupId((double*)info.ptr);
          },
          py::arg("values").noconvert())
      .def(
          "is_member",
          [](ParameterBlockOrdering& self, py::array_t<double>& values) {
            py::buffer_info info = values.request();
            return self.IsMember((double*)info.ptr);
          },
          py::arg("values").noconvert())
      .def(
          "group_size",
          [](ParameterBlockOrdering& self, const int group) {
            return self.GroupSize(group);
          },
          py::arg("group"));
}

void BindSolver(py::module& m) {
  using IterSummary = ceres::IterationSummary;
  py::class_<IterSummary, std::shared_ptr<IterSummary>> PyIterSummary(
      m, "IterationSummary");
  PyIterSummary.def(py::init<>())
      .def(py::init<const IterSummary&>())
      .def_readonly("iteration", &IterSummary::iteration)
      .def_readonly("step_is_valid", &IterSummary::step_is_valid)
      .def_readonly("step_is_nonmonotonic", &IterSummary::step_is_nonmonotonic)
      .def_readonly("step_is_successful", &IterSummary::step_is_successful)
      .def_readonly("cost", &IterSummary::cost)
      .def_readonly("cost_change", &IterSummary::cost_change)
      .def_readonly("gradient_max_norm", &IterSummary::gradient_max_norm)
      .def_readonly("gradient_norm", &IterSummary::gradient_norm)
      .def_readonly("step_norm", &IterSummary::step_norm)
      .def_readonly("relative_decrease", &IterSummary::relative_decrease)
      .def_readonly("trust_region_radius", &IterSummary::trust_region_radius)
      .def_readonly("eta", &IterSummary::eta)
      .def_readonly("step_size", &IterSummary::step_size)
      .def_readonly("line_search_function_evaluations",
                    &IterSummary::line_search_function_evaluations)
      .def_readonly("line_search_gradient_evaluations",
                    &IterSummary::line_search_gradient_evaluations)
      .def_readonly("line_search_iterations",
                    &IterSummary::line_search_iterations)
      .def_readonly("linear_solver_iterations",
                    &IterSummary::linear_solver_iterations)
      .def_readonly("iteration_time_in_seconds",
                    &IterSummary::iteration_time_in_seconds)
      .def_readonly("step_solver_time_in_seconds",
                    &IterSummary::step_solver_time_in_seconds)
      .def_readonly("cumulative_time_in_seconds",
                    &IterSummary::cumulative_time_in_seconds);

  py::class_<ceres::IterationCallback,
             PyIterationCallback /* <--- trampoline*/>(m, "IterationCallback")
      .def(py::init<>())
      .def("__call__", &ceres::IterationCallback::operator());

  auto vec_it_cb = py::bind_vector<std::vector<ceres::IterationCallback*>>(
      m, "ListIterationCallback");

  vec_it_cb.def(
      py::init<>([](py::list list) {
        std::vector<ceres::IterationCallback*> callbacks;
        for (auto& handle : list) {
          callbacks.push_back(handle.cast<ceres::IterationCallback*>());
        }
        return callbacks;
      }),
      py::keep_alive<1, 2>());
  py::implicitly_convertible<py::list,
                             std::vector<ceres::IterationCallback*>>();

  using Options = ceres::Solver::Options;
  py::class_<Options, std::shared_ptr<Options>> PyOptions(m, "SolverOptions");
  PyOptions.def(py::init<>())
      .def(py::init<const Options&>())
      .def("IsValid", &Options::IsValid)
      .def_property(
          "callbacks",
          [](const Options& self) { return self.callbacks; },
          py::cpp_function(
              [](Options& self, py::list list) {
                std::vector<ceres::IterationCallback*> callbacks;
                for (auto& handle : list) {
                  self.callbacks.push_back(
                      handle.cast<ceres::IterationCallback*>());
                }
              },
              py::keep_alive<1, 2>()))
      .def_readwrite("minimizer_type", &Options::minimizer_type)
      .def_readwrite("line_search_direction_type",
                     &Options::line_search_direction_type)
      .def_readwrite("line_search_type", &Options::line_search_type)
      .def_readwrite("nonlinear_conjugate_gradient_type",
                     &Options::nonlinear_conjugate_gradient_type)
      .def_readwrite("max_lbfgs_rank", &Options::max_lbfgs_rank)
      .def_readwrite("use_approximate_eigenvalue_bfgs_scaling",
                     &Options::use_approximate_eigenvalue_bfgs_scaling)
      .def_readwrite("line_search_interpolation_type",
                     &Options::line_search_interpolation_type)
      .def_readwrite("min_line_search_step_size",
                     &Options::min_line_search_step_size)
      .def_readwrite("line_search_sufficient_function_decrease",
                     &Options::line_search_sufficient_function_decrease)
      .def_readwrite("max_line_search_step_contraction",
                     &Options::max_line_search_step_contraction)
      .def_readwrite("min_line_search_step_contraction",
                     &Options::min_line_search_step_contraction)
      .def_readwrite("max_num_line_search_step_size_iterations",
                     &Options::max_num_line_search_step_size_iterations)
      .def_readwrite("max_num_line_search_direction_restarts",
                     &Options::max_num_line_search_direction_restarts)
      .def_readwrite("line_search_sufficient_curvature_decrease",
                     &Options::line_search_sufficient_curvature_decrease)
      .def_readwrite("max_line_search_step_expansion",
                     &Options::max_line_search_step_expansion)
      .def_readwrite("trust_region_strategy_type",
                     &Options::trust_region_strategy_type)
      .def_readwrite("dogleg_type", &Options::dogleg_type)
      .def_readwrite("use_nonmonotonic_steps", &Options::use_nonmonotonic_steps)
      .def_readwrite("max_consecutive_nonmonotonic_steps",
                     &Options::max_consecutive_nonmonotonic_steps)
      .def_readwrite("max_num_iterations", &Options::max_num_iterations)
      .def_readwrite("max_solver_time_in_seconds",
                     &Options::max_solver_time_in_seconds)
      .def_property(
          "num_threads",
          [](const Options& self) { return self.num_threads; },
          [](Options& self, int n_threads) {
            int effective_n_threads = GetEffectiveNumThreads(n_threads);
            self.num_threads = effective_n_threads;
#if CERES_VERSION_MAJOR < 2
            self.num_linear_solver_threads = effective_n_threads;
#endif  // CERES_VERSION_MAJOR
          })
      .def_readwrite("initial_trust_region_radius",
                     &Options::initial_trust_region_radius)
      .def_readwrite("max_trust_region_radius",
                     &Options::max_trust_region_radius)
      .def_readwrite("min_trust_region_radius",
                     &Options::min_trust_region_radius)
      .def_readwrite("min_relative_decrease", &Options::min_relative_decrease)
      .def_readwrite("min_lm_diagonal", &Options::min_lm_diagonal)
      .def_readwrite("max_lm_diagonal", &Options::max_lm_diagonal)
      .def_readwrite("max_num_consecutive_invalid_steps",
                     &Options::max_num_consecutive_invalid_steps)
      .def_readwrite("function_tolerance", &Options::function_tolerance)
      .def_readwrite("gradient_tolerance", &Options::gradient_tolerance)
      .def_readwrite("parameter_tolerance", &Options::parameter_tolerance)
      .def_readwrite("linear_solver_type", &Options::linear_solver_type)
      .def_readwrite("preconditioner_type", &Options::preconditioner_type)
      .def_readwrite("visibility_clustering_type",
                     &Options::visibility_clustering_type)
      .def_readwrite("dense_linear_algebra_library_type",
                     &Options::dense_linear_algebra_library_type)
      .def_readwrite("sparse_linear_algebra_library_type",
                     &Options::sparse_linear_algebra_library_type)
      // .def_readwrite("num_linear_solver_threads",
      // &Options::num_linear_solver_threads)
      .def_readwrite("use_explicit_schur_complement",
                     &Options::use_explicit_schur_complement)
      .def_readwrite("dynamic_sparsity", &Options::dynamic_sparsity)
      .def_readwrite("use_inner_iterations", &Options::use_inner_iterations)
      .def_readwrite("inner_iteration_tolerance",
                     &Options::inner_iteration_tolerance)
      .def_readwrite("min_linear_solver_iterations",
                     &Options::min_linear_solver_iterations)
      .def_readwrite("max_linear_solver_iterations",
                     &Options::max_linear_solver_iterations)
      .def_readwrite("eta", &Options::eta)
      .def_readwrite("jacobi_scaling", &Options::jacobi_scaling)
      .def_readwrite("logging_type", &Options::logging_type)
      .def_readwrite("minimizer_progress_to_stdout",
                     &Options::minimizer_progress_to_stdout)
      .def_readwrite("trust_region_problem_dump_directory",
                     &Options::trust_region_problem_dump_directory)
      .def_readwrite("trust_region_problem_dump_format_type",
                     &Options::trust_region_problem_dump_format_type)
      .def_readwrite("check_gradients", &Options::check_gradients)
      .def_readwrite("gradient_check_relative_precision",
                     &Options::gradient_check_relative_precision)
      .def_readwrite(
          "gradient_check_numeric_derivative_relative_step_size",
          &Options::gradient_check_numeric_derivative_relative_step_size)
      .def_readwrite("update_state_every_iteration",
                     &Options::update_state_every_iteration)
      .def_readwrite("inner_iteration_ordering",
                     &Options::inner_iteration_ordering)
      .def_readwrite("linear_solver_ordering",
                     &Options::linear_solver_ordering);
  MakeDataclass(PyOptions);

  using Summary = ceres::Solver::Summary;
  py::class_<Summary, std::shared_ptr<Summary>> PySummary(m, "SolverSummary");
  PySummary.def(py::init<>())
      .def(py::init<const Summary&>())
      .def("BriefReport", &Summary::BriefReport)
      .def("FullReport", &Summary::FullReport)
      .def("IsSolutionUsable", &Summary::IsSolutionUsable)
      .def_readwrite("minimizer_type", &Summary::minimizer_type)
      .def_readwrite("termination_type", &Summary::termination_type)
      .def_readwrite("message", &Summary::message)
      .def_readwrite("initial_cost", &Summary::initial_cost)
      .def_readwrite("final_cost", &Summary::final_cost)
      .def_readwrite("fixed_cost", &Summary::fixed_cost)
      .def_readwrite("num_successful_steps", &Summary::num_successful_steps)
      .def_readwrite("num_unsuccessful_steps", &Summary::num_unsuccessful_steps)
      .def_readwrite("num_inner_iteration_steps",
                     &Summary::num_inner_iteration_steps)
      .def_readwrite("num_line_search_steps", &Summary::num_line_search_steps)
      .def_readwrite("preprocessor_time_in_seconds",
                     &Summary::preprocessor_time_in_seconds)
      .def_readwrite("minimizer_time_in_seconds",
                     &Summary::minimizer_time_in_seconds)
      .def_readwrite("postprocessor_time_in_seconds",
                     &Summary::postprocessor_time_in_seconds)
      .def_readwrite("total_time_in_seconds", &Summary::total_time_in_seconds)
      .def_readwrite("linear_solver_time_in_seconds",
                     &Summary::linear_solver_time_in_seconds)
      .def_readwrite("num_linear_solves", &Summary::num_linear_solves)
      .def_readwrite("residual_evaluation_time_in_seconds",
                     &Summary::residual_evaluation_time_in_seconds)
      .def_readwrite("num_residual_evaluations",
                     &Summary::num_residual_evaluations)
      .def_readwrite("jacobian_evaluation_time_in_seconds",
                     &Summary::jacobian_evaluation_time_in_seconds)
      .def_readwrite("num_jacobian_evaluations",
                     &Summary::num_jacobian_evaluations)
      .def_readwrite("inner_iteration_time_in_seconds",
                     &Summary::inner_iteration_time_in_seconds)
      .def_readwrite("line_search_cost_evaluation_time_in_seconds",
                     &Summary::line_search_cost_evaluation_time_in_seconds)
      .def_readwrite("line_search_gradient_evaluation_time_in_seconds",
                     &Summary::line_search_gradient_evaluation_time_in_seconds)
      .def_readwrite(
          "line_search_polynomial_minimization_time_in_seconds",
          &Summary::line_search_polynomial_minimization_time_in_seconds)
      .def_readwrite("line_search_total_time_in_seconds",
                     &Summary::line_search_total_time_in_seconds)
      .def_readwrite("num_parameter_blocks", &Summary::num_parameter_blocks)
      .def_readwrite("num_parameters", &Summary::num_parameters)
      .def_readwrite("num_effective_parameters",
                     &Summary::num_effective_parameters)
      .def_readwrite("num_residual_blocks", &Summary::num_residual_blocks)
      .def_readwrite("num_residuals", &Summary::num_residuals)
      .def_readwrite("num_parameter_blocks_reduced",
                     &Summary::num_parameter_blocks_reduced)
      .def_readwrite("num_parameters_reduced", &Summary::num_parameters_reduced)
      .def_readwrite("num_effective_parameters_reduced",
                     &Summary::num_effective_parameters_reduced)
      .def_readwrite("num_residual_blocks_reduced",
                     &Summary::num_residual_blocks_reduced)
      .def_readwrite("num_residuals_reduced", &Summary::num_residuals_reduced)
      .def_readwrite("is_constrained", &Summary::is_constrained)
      .def_readwrite("num_threads_given", &Summary::num_threads_given)
      .def_readwrite("num_threads_used", &Summary::num_threads_used)
#if CERES_VERSION_MAJOR < 2
      .def_readwrite("num_linear_solver_threads_given",
                     &Summary::num_linear_solver_threads_given)
      .def_readwrite("num_linear_solver_threads_used",
                     &Summary::num_linear_solver_threads_used)
#endif
      .def_readwrite("linear_solver_type_given",
                     &Summary::linear_solver_type_given)
      .def_readwrite("linear_solver_type_used",
                     &Summary::linear_solver_type_used)
      .def_readwrite("schur_structure_given", &Summary::schur_structure_given)
      .def_readwrite("schur_structure_used", &Summary::schur_structure_used)
      .def_readwrite("inner_iterations_given", &Summary::inner_iterations_given)
      .def_readwrite("inner_iterations_used", &Summary::inner_iterations_used)
      .def_readwrite("preconditioner_type_given",
                     &Summary::preconditioner_type_given)
      .def_readwrite("preconditioner_type_used",
                     &Summary::preconditioner_type_used)
      .def_readwrite("visibility_clustering_type",
                     &Summary::visibility_clustering_type)
      .def_readwrite("trust_region_strategy_type",
                     &Summary::trust_region_strategy_type)
      .def_readwrite("dogleg_type", &Summary::dogleg_type)
      .def_readwrite("dense_linear_algebra_library_type",
                     &Summary::dense_linear_algebra_library_type)
      .def_readwrite("sparse_linear_algebra_library_type",
                     &Summary::sparse_linear_algebra_library_type)
      .def_readwrite("line_search_direction_type",
                     &Summary::line_search_direction_type)
      .def_readwrite("line_search_type", &Summary::line_search_type)
      .def_readwrite("line_search_interpolation_type",
                     &Summary::line_search_interpolation_type)
      .def_readwrite("nonlinear_conjugate_gradient_type",
                     &Summary::nonlinear_conjugate_gradient_type)
      .def_readwrite("max_lbfgs_rank", &Summary::max_lbfgs_rank);
  MakeDataclass(PySummary);

  m.def("solve",
        py::overload_cast<const ceres::Solver::Options&,
                          ceres::Problem*,
                          ceres::Solver::Summary*>(&ceres::Solve),
        py::call_guard<py::gil_scoped_release>());
}
