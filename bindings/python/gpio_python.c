#include <Python.h>

#include "libsoc_gpio.h"

static PyObject *python_cb = NULL;

static int
gpio_cb(void* arg)
{
  int rc = EXIT_SUCCESS;
  PyGILState_STATE state = PyGILState_Ensure();
  PyObject *result = PyObject_CallObject(python_cb, NULL);
  if (!result)
    rc = EXIT_FAILURE;
  Py_DECREF(result);
  PyGILState_Release(state);
  return rc;
}


static PyObject *
start_gpio_cb(PyObject *self, PyObject *args)
{
  unsigned long gpio_addr;
  PyObject *temp;

  if (PyArg_ParseTuple(args, "kO", &gpio_addr, &temp))
    {
      if (!PyCallable_Check(temp))
        {
          PyErr_SetString(PyExc_TypeError, "parameter must be callable");
          return NULL;
        }
      Py_XINCREF(temp);       /* Add a reference to new callback */
      Py_XDECREF(python_cb);  /* Dispose of previous callback */
      python_cb = temp;       /* Remember new callback */

      libsoc_gpio_callback_interrupt((gpio*)gpio_addr, gpio_cb, NULL);
      Py_INCREF(Py_None);
      return Py_None;
    }
  return NULL;
}

static PyMethodDef functions[] = {
  {"start_gpio_cb", start_gpio_cb, METH_VARARGS,
   "Runs libsoc_gpio_callback_interrupt with logic for the python callback"},

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

