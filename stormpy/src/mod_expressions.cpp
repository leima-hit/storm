#include "common.h"
#include "helpers.h"

#include <src/storage/expressions/ExpressionManager.h>

PYBIND11_PLUGIN(expressions) {
	py::module m("stormpy.expressions", "Storm expressions");

    py::class_<std::shared_ptr<storm::expressions::ExpressionManager>>(m, "ExpressionManager", "Manages variables for expressions")
        ;
    
    py::class_<storm::expressions::Expression>(m, "Expression", "Holds an expression")
        ;

	return m.ptr();
}