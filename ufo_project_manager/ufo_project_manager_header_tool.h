#pragma once
#include "../console/console.h"
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <string>

namespace UFOProjectManagerHeaderTool{

std::vector<std::string> Run(const std::string& _arg){
    pybind11::scoped_interpreter guard{};

    pybind11::module_ py_os = pybind11::module_::import("os");
    py_os.attr("chdir")("../header_tool");

    pybind11::module_ mython = pybind11::module_::import("ufo_project_manager_header_tool");

    pybind11::object res_list = mython.attr("search_file")(_arg);

    std::vector<std::string> msgs = res_list.cast<std::vector<std::string>>();

    py_os.attr("chdir")("../build");

    return msgs;
}

}