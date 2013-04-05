#include <Python.h>
#include <stdio.h>
#include <mdbtools.h>

#if PY_MAJOR_VERSION >= 3
	#define pymdb_object_to_char_p(o)   PyBytes_AsString(o) 
	#define pymdb_char_p_to_str(s)		PyBytes_FromString(s) 
	#define pymdb_char_p_to_string(s)	PyUnicode_FromFormat("%s", s)
			
#else
	#define pymdb_object_to_char_p(o)	PyString_AsString(o)
	#define pymdb_char_p_to_str(s)		PyString_FromString(s)
	#define pymdb_char_p_to_string(s)	PyString_FromString(s)
	
#endif

static PyObject* tablenames(MdbHandle * mdb){
	
	PyObject *tableList = PyList_New(0);
	int objtype = MDB_TABLE;
	
	if( tableList == NULL){
		Py_RETURN_NONE;
	}
	
 	if (!mdb_read_catalog (mdb, MDB_ANY)) {
		mdb_close(mdb);
		Py_RETURN_NONE;
	}
	
	 for (int i=0; i < mdb->num_catalog; i++) {
		 
		MdbCatalogEntry * entry = (MdbCatalogEntry *) g_ptr_array_index (mdb->catalog, i);
			
		if (entry->object_type != objtype && objtype!=MDB_ANY)
			continue;
		if (mdb_is_system_table(entry))
			continue;
		
		if(strlen(entry->object_name)){
			PyList_Append(tableList,pymdb_char_p_to_str(entry->object_name));
		}
	}
	
	assert(Py_None != tableList);
	
	return tableList;
	
}
	

static PyObject* parsefile_table_dictionary	(PyObject* self, PyObject* args)
{			
	
	PyObject *tableList = NULL, *dbDictionary = NULL, *tableDictionary = NULL;
    const char* name = NULL;
	
	MdbTableDef *table = NULL;
 	MdbHandle * mdb = NULL;
	
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
	
	dbDictionary = PyDict_New();
	
	if( dbDictionary == NULL){
		Py_RETURN_NONE;
	}
	
	if(!(mdb = mdb_open(name, MDB_NOFLAGS))){
		Py_RETURN_NONE;
	}
	
	tableList=tablenames(mdb);
		
	for (int i=0; i < PyList_Size(tableList); i++) {
		PyObject * tableObject= PyDict_New();
		PyObject * dataList = PyList_New(0);
		
		MdbColumn *col;
		char *value;
		char **bound_values;
		int  *bound_lens;
		PyObject *headerList=NULL, *row=NULL;
		size_t length;
		table = NULL;
		
			
		char * tablename  = (char*)pymdb_object_to_char_p(PyList_GetItem(tableList,i));
		
		if(!strlen(tablename))
			continue;
		
		table = mdb_read_table_by_name(mdb, tablename, MDB_TABLE);
		
		if(table == NULL)
			continue;
		
		mdb_read_columns(table);
		mdb_rewind_table(table);
		
		if(!table->num_cols){
			Py_RETURN_NONE;
		}
		
		tableDictionary = PyDict_New();
		headerList = PyList_New(0);
		
		if(tableDictionary == NULL|| headerList==NULL){
			Py_RETURN_NONE;
		}
			
			
		bound_values = (char **) g_malloc(table->num_cols * sizeof(char *));
		bound_lens = (int *) g_malloc(table->num_cols * sizeof(int));
		
		for (int i=0;i<table->num_cols;i++) {
			bound_values[i] = (char *) g_malloc0(MDB_BIND_SIZE);
			mdb_bind_column(table, i+1, bound_values[i], &bound_lens[i]);
		}
		
		
	
		for (int i=0; i<table->num_cols; i++) {			
			col=(MdbColumn *)g_ptr_array_index(table->columns,i);
			PyList_Append(headerList, pymdb_char_p_to_string(col->name));
		}
						
		while(mdb_fetch_row(table)) {	
			row=NULL;
			row = PyList_New(0);
			for (int i=0;i<table->num_cols;i++) {				
				col=(MdbColumn *)g_ptr_array_index(table->columns,i);
				
				if (!bound_lens[i]) {
					PyList_Append(row,Py_None);
				} else {
					if (col->col_type == MDB_OLE) {
						value = (char *) mdb_ole_read_full(mdb, col, &length);
					} else {
						value = bound_values[i];
						length = bound_lens[i];
					}
					
					PyList_Append(row,pymdb_char_p_to_string(value));
					
					if (col->col_type == MDB_OLE)
						free(value);
				}
			}
			
			PyList_Append(dataList, row);
		}
		
		g_free(bound_values);
		g_free(bound_lens);
		
		PyDict_SetItem(tableObject,  pymdb_char_p_to_string("headers"), headerList);
		PyDict_SetItem(tableObject,  pymdb_char_p_to_string("data"), dataList);
		PyDict_SetItem(dbDictionary,  pymdb_char_p_to_string(tablename), tableObject);
	}
	
	mdb_close(mdb);	
		
	return dbDictionary;
}

extern "C" {
	struct module_state {
	    PyObject *error;
	};

	static PyMethodDef pymdbbin_methods[] =
	{
		 {"parsefile_table_dictionary", parsefile_table_dictionary, METH_VARARGS, "parse mdb for accessfile"},
	     {NULL, NULL, 0, NULL}
	};

	#if PY_MAJOR_VERSION >= 3
	#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
	#else
	#define GETSTATE(m) (&_state)
	static struct module_state _state;
	#endif


	#if PY_MAJOR_VERSION >= 3

	static int pymdbbin_traverse(PyObject *m, visitproc visit, void *arg) {
	    Py_VISIT(GETSTATE(m)->error);
	    return 0;
	}

	static int pymdbbin_clear(PyObject *m) {
	    Py_CLEAR(GETSTATE(m)->error);
	    return 0;
	}


	struct PyModuleDef moduledef = {
	        PyModuleDef_HEAD_INIT,
	        "pymdbbin",
	        NULL,
	        sizeof(struct module_state),
	        pymdbbin_methods,
	        NULL,
	        pymdbbin_traverse,
	        pymdbbin_clear,
	        NULL
	};

	#define INITERROR return NULL

	extern PyObject *
	PyInit_pymdbbin(void)

	#else
	#define INITERROR return

	void
	initpymdbbin(void)
	#endif

	{
	#if PY_MAJOR_VERSION >= 3
	    PyObject *module = PyModule_Create(&moduledef);
	#else
	    PyObject *module = Py_InitModule("pymdbbin", pymdbbin_methods);
	#endif

	    if (module == NULL)
	        INITERROR;
	    struct module_state *st = GETSTATE(module);

	    st->error = PyErr_NewException((char *)"pymdbbin.Error", NULL, NULL);
	    if (st->error == NULL) {
	        Py_DECREF(module);
	        INITERROR;
	    }

	#if PY_MAJOR_VERSION >= 3
	    return module;
	#endif
	}
}
