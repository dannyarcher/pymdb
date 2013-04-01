#include <Python.h>
#include <stdio.h>
#include <mdbtools.h>
 
 
enum parsefile_Stratergy {
    CLUBS    = 1,
    DIAMONDS = 2,
    HEARTS   = 4,
    SPADES   = 8
};

 
void logObject (PyObject* o)
{
	PyObject * objectsRepresentation = PyObject_Repr(o);
	char * s = PyString_AsString(objectsRepresentation);
	printf("\n%s\n",s);
}

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
		
		if(strlen(entry->object_name))
			PyList_Append(tableList,PyString_FromString(entry->object_name));
	}
	
	assert(Py_None != tableList);
	
	return tableList;
	
}
	

static PyObject* parsefile_table_dictionary(PyObject* self, PyObject* args)
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
		
		char* tablename = PyString_AsString(PyList_GetItem(tableList,i));
				
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
			PyList_Append(headerList, PyString_FromString(col->name));
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
					
					PyList_Append(row,PyString_FromString(value));
					
					if (col->col_type == MDB_OLE)
						free(value);
				}
			}
			
			PyList_Append(dataList, row);
		}
		
		g_free(bound_values);
		g_free(bound_lens);
		
		PyDict_SetItem(tableObject,  PyString_FromString("headers"), headerList);
		PyDict_SetItem(tableObject,  PyString_FromString("data"), dataList);
		PyDict_SetItem(dbDictionary,  PyString_FromString(tablename), tableObject);
	}
	
	mdb_close(mdb);		
	return dbDictionary ;

}


 
static PyMethodDef RotemMethods[] =
{
	 {"parsefile_table_dictionary", parsefile_table_dictionary, METH_VARARGS, "parse mdb for accessfile"},
     {NULL, NULL, 0, NULL}
};
 
PyMODINIT_FUNC
 
initpymdbbin(void)
{
     (void) Py_InitModule("pymdbbin", RotemMethods);
}