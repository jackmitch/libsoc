#include <Python.h>

#include "libsoc_gpio.h"
#include "libsoc_spi.h"

static PyMethodDef functions[] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static void
_add_constants(PyObject *m)
{
  PyObject *mod, *func, *args, *api;

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

  PyModule_AddIntConstant(m, "LS_GPIO_SHARED", LS_GPIO_SHARED);
  PyModule_AddIntConstant(m, "LS_GPIO_GREEDY", LS_GPIO_GREEDY);
  PyModule_AddIntConstant(m, "LS_GPIO_WEAK", LS_GPIO_WEAK);

  PyModule_AddIntConstant(m, "BITS_8", BITS_8);
  PyModule_AddIntConstant(m, "BITS_16", BITS_16);
  PyModule_AddIntConstant(m, "BPW_ERROR", BPW_ERROR);

  PyModule_AddIntConstant(m, "MODE_0", MODE_0);
  PyModule_AddIntConstant(m, "MODE_1", MODE_1);
  PyModule_AddIntConstant(m, "MODE_2", MODE_2);
  PyModule_AddIntConstant(m, "MODE_3", MODE_3);
  PyModule_AddIntConstant(m, "MODE_ERROR", MODE_ERROR);

  mod = PyImport_ImportModule("ctypes");
  if (mod == NULL) {
      printf("ERROR importing 'ctypes' module:\n");
      PyErr_Print();
      exit(-1);
  }
  func = PyObject_GetAttrString(mod, "CDLL");
  if (func == NULL) {
      printf("ERROR getting 'CDLL' function:\n");
      PyErr_Print();
      exit(-1);
  }
  if (!PyCallable_Check(func)) {
	  printf("'CDLL' function is not callable:\n");
      PyErr_Print();
      exit(-1);
  }

  args = Py_BuildValue("(s)", LIBSOC_SO);
  api = PyObject_CallObject(func, args);
  if (api == NULL) {
      printf("'CDLL' function call to load '%s' failed:\n", LIBSOC_SO);
      PyErr_Print();
      exit(-1);
  }
  Py_DECREF(args);
  PyModule_AddObject(m, "api", api);
}


#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "_libsoc",
        NULL,
        -1,
        functions,
        NULL,
        NULL,
        NULL,
        NULL
};

PyMODINIT_FUNC
PyInit__libsoc(void) {
  PyObject *m;
  m = PyModule_Create(&moduledef);
  if (!m)
   return NULL;
  _add_constants(m);
  return m;
}
#else
PyMODINIT_FUNC
init_libsoc(void)
{
  PyObject *m;
  m = Py_InitModule ("_libsoc", functions);
  _add_constants(m);
}
#endif
