#include "Python.h"

int ReadFile()
{
    PyObject* pName;
    PyObject* pModule = NULL;
    PyObject* pDict = NULL;
    PyObject* pFunc = NULL;
    PyObject* pArgs = NULL;
    PyObject* pRetVal = NULL;
    PyObject* pList = NULL;

    int i = 0;
    Py_ssize_t num = 0;

    char* functionName = "getWeb";

    Py_Initialize();
    if ( !Py_IsInitialized() )
    {
        printf( "Cannot initialize python27.\n" );
        return -1;
    }
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");
    /*Create python string symbol, s stand for string and pytest stand for pytest.py*/
    pName = Py_BuildValue( "s", "food" );

    /*Import the python file.*/
    pModule = PyImport_Import( pName );
    if ( !pModule )
    {
        PyErr_Print();
        printf("Couldnot find food.py .\n");
        return -1;
    }

    pDict = PyModule_GetDict( pModule );
    if ( !pDict )
    {
        return -1;
    }

    pFunc = PyDict_GetItemString( pDict, functionName );
    if ( !PyCallable_Check( pFunc ) )
    {
        printf("Couldnot find function.\n" );
        return -1;
    }

    pArgs = PyTuple_New( 1 );
    PyTuple_SetItem( pArgs, 0, Py_BuildValue( "i", 1 ) );

    pRetVal = PyObject_CallObject( pFunc, pArgs );

    printf("%s\n", PyString_AsString( PyDict_GetItemString( pRetVal, "1" ) ) );  
    printf("%s\n", PyString_AsString( PyDict_GetItemString( pRetVal, "2" ) ) );   



    Py_DECREF( pName );
    Py_DECREF( pDict );
    Py_DECREF( pArgs );
    Py_DECREF( pModule );
    //char* tt = PyString_AsString( PyDict_GetItemString( pRetVal, "1" ) );
    //printf("%c\n", *tt);
    Py_Finalize();
    return 1;

}

int main()
{
    if ( -1 == ReadFile() )
    {
        return -1;
    }
    return 0;
}

