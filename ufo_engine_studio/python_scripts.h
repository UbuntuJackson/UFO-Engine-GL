#include <python3.10/Python.h>
#include "../console/console.h"
#include "../file/file.h"

namespace UFOEngineStudioPython{

    namespace PythonHighLevel{

    void PyException(PyConfig* _python_config, PyStatus* _python_status){
        PyConfig_Clear(_python_config);
        Py_ExitStatusException(*_python_status);
    }

    void PythonQuickRunString(const std::string& _code, const std::string& _args){
        PyRun_SimpleString(_code.c_str());
    }

    void InitialisePython(){
        PyStatus python_status;
        PyConfig python_config;
        PyConfig_InitPythonConfig(&python_config);

        python_status = PyConfig_SetBytesString(&python_config, &python_config.program_name, "");
        if(PyStatus_Exception(python_status)){
            PyException(&python_config, &python_status);
        }

        python_status = Py_InitializeFromConfig(&python_config);
        if(PyStatus_Exception(python_status)){
            PyException(&python_config, &python_status);
        }

        PyConfig_Clear(&python_config);
    }

    void QuitPython(){
        if(Py_FinalizeEx() < 0){
            Console::PrintLine("Finalizing python with exit code 120");
            exit(120);
        }
        Console::PrintLine("Finalizing python with exit code 0");
    }

    }

    void ImportHeaderFileToProject(std::string _path){
        PythonHighLevel::PythonQuickRunString(File().Read(_path));
    }

}