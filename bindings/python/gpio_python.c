#include <Python.h>

#include "libsoc_gpio.h"

static PyMethodDef functions[] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC
init_libsoc(void)
{
  PyObject *m;
  m = Py_InitModule ("_libsoc", functions);

  PyModule_AddIntConstant(m, "DIRECTION_ERROR", DIRECTION_ERROR);
  PyModule_AddIntConstant(m, "DIRECTION_INPUT", INPUT);
  PyModule_AddIntConstant(m, "DIRECTION_OUTPUT", OUTPUT);

  PyModule_AddIntConstant(m, "LEVEL_ERROR", LEVEL_ERROR);
  PyModule_AddIntConstant(m, "LEVEL_LOW", LOW);
  PyModule_AddIntConstant(m, "LEVEL_HIGH", HIGH);

  PyModule_AddIntConstant(m, "EDGE_ERROR", EDGE_ERROR);
  PyModule_AddIntConstant(m, "EDGE_RISING", RISING);
  PyModule_AddIntConstant(m, "EDGE_FALLING", FALLING);
  PyModule_AddIntConstant(m, "EDGE_NONE", NONE);
  PyModule_AddIntConstant(m, "EDGE_BOTH", BOTH);

  PyModule_AddIntConstant(m, "LS_SHARED", LS_SHARED);
  PyModule_AddIntConstant(m, "LS_GREEDY", LS_GREEDY);
  PyModule_AddIntConstant(m, "LS_WEAK", LS_WEAK);
}

