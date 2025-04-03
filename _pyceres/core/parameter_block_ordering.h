#pragma once

#include "_pyceres/helpers.h"
#include "_pyceres/logging.h"

#include <ceres/ceres.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

using namespace ceres;

void BindParameterBlockOrdering(py::module& m) {
  py::class_<ParameterBlockOrdering, std::shared_ptr<ParameterBlockOrdering>> (m, "ParameterBlockOrdering")
    .def(py::init<>())
    .def(
      "clear",
      &ParameterBlockOrdering::Clear)
    .def("reverse",
      &ParameterBlockOrdering::Reverse)
    .def("num_elements",
      &ParameterBlockOrdering::NumElements)
    .def("num_groups",
      &ParameterBlockOrdering::NumGroups)
    .def(
      "add_element_to_group", 
    [] (ParameterBlockOrdering& self, py::array_t<double>& values, const int group) {
      py::buffer_info info = values.request();
      return self.AddElementToGroup((double*)info.ptr, group);
    },
    py::arg("values").noconvert(),
    py::arg("group"))
    .def(
      "remove",
      [] (ParameterBlockOrdering& self, py::array_t<double>& values) {
        py::buffer_info info = values.request();
        return self.Remove((double*)info.ptr);
      },
      py::arg("values").noconvert())
    .def("group_id",
      [] (ParameterBlockOrdering& self, py::array_t<double>& values) {
        py::buffer_info info = values.request();
        return self.GroupId((double*)info.ptr);
      },
      py::arg("values").noconvert())
    .def("is_member",
      [] (ParameterBlockOrdering& self, py::array_t<double>& values) {
        py::buffer_info info = values.request();
        return self.IsMember((double*)info.ptr);
      },
      py::arg("values").noconvert())
    .def("group_size",
      [] (ParameterBlockOrdering& self, const int group) {
        return self.GroupSize(group);
      },
      py::arg("group"));

}
