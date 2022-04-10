#include <Python.h>
#include "wrapper.h"

/* Docstrings */
static char module_docstring[] =
    "Esta biblioteca é um wrapper ";

/* Available functions */
static PyObject *adc_read_channel(PyObject *self, PyObject *args);
static PyObject *adc_read_all_channels(PyObject *self, PyObject *args);
static PyObject *adc_start(PyObject *self, PyObject *args);
static PyObject *adc_stop(PyObject *self, PyObject *args);

/* Module specification */
static PyMethodDef module_methods[] = {
 //   {"chi2", chi2_chi2, METH_VARARGS, chi2_docstring},
    {"read_channel", adc_read_channel, METH_VARARGS, {"lê o canal especificado do ads1256"}},
    {"read_all_channels", adc_read_all_channels, METH_VARARGS, {"lê todos os 8 canais do ads1256"}},
    {"start", adc_start, METH_VARARGS, {"inicia e configura o ads1256"}},
    {"stop", adc_stop, METH_VARARGS, {"termina e fecha o ads1256"}},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef ads1256 =
{
    PyModuleDef_HEAD_INIT,
    "ads1256",          /* name of module */
    module_docstring,   /* module documentation, may be NULL */
    -1,                 /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    module_methods
};


/* Initialize the module */
PyMODINIT_FUNC PyInit_ads1256(void)
{
    return PyModule_Create(&ads1256);
}

static PyObject *adc_start(PyObject *self, PyObject *args)
{

    char * ganho, *sps;
    PyObject *yerr_obj;
    double v[8];
    int value ;
                                         

    /* Parse the input tuple */
    if (!PyArg_ParseTuple(args, "ss", &ganho, &sps,&yerr_obj))
        return NULL;

    /* execute the code */ 
    value = adcStart(4,"0",ganho,sps);

    /* Build the output tuple */
    PyObject *ret = Py_BuildValue("i",value);
    return ret;
}

static PyObject *adc_read_channel(PyObject *self, PyObject *args)
{

    int ch;
    long int retorno;
    PyObject *yerr_obj;
    


    /* Parse the input tuple */
    if (!PyArg_ParseTuple(args, "i", &ch,&yerr_obj))
        return NULL;
                                       

    /* execute the code */ 
    retorno = readChannel(ch);
    return Py_BuildValue("l",retorno);
}

static PyObject *adc_read_all_channels(PyObject *self, PyObject *args)
{
    PyObject *yerr_obj;
    long int v[8];
                                         
    /* execute the code */ 
    readChannels(v);

    /* Build the output tuple */
    PyObject *ret = Py_BuildValue("[l,l,l,l,l,l,l,l]",
	 v[0],
	 v[1],
	 v[2],
	 v[3],
	 v[4],
	 v[5],
	 v[6],
	 v[7]
     );
    return ret;
}

static PyObject *adc_stop(PyObject *self, PyObject *args)
{
    /* execute the code */ 
    int value = adcStop();

    /* Build the output tuple */
    PyObject *ret = Py_BuildValue("i",value);
    return ret;
}


