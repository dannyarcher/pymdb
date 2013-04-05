import nose
import unittest

accessfile = "tests/StandardAccess.mdb"

class pymdb(unittest.TestCase):
    def tearDown(self):
        pymdb = None
    
    def test_import(self):
        import pymdb
        self.assertIsNotNone(pymdb)

    def test_parsefile_Exists(self):
        import pymdb
        self.assertTrue(pymdb.parsefile!=None)
        
    def test_parsefileNone_Runs(self):
        import pymdb
        result = pymdb.parsefile("")
        self.assertIsNone(result)

    def test_parsefile_Runs(self):
        import pymdb
        result = pymdb.parsefile(accessfile)
        self.assertIsNotNone(result)
        
    def test_parsefile_Returns_Tables(self):
        import pymdb
        result = pymdb.parsefile(accessfile)
        self.assertGreater(len(result.keys()), 0)
    
    def test_parsefile_Returns_Dictionary(self):
        import pymdb
        result = pymdb.parsefile(accessfile)
        self.assertIsInstance(result, dict)
        
    def test_parsefile_Returns_Dictionary_Keys(self):
        import pymdb
        result = pymdb.parsefile(accessfile)
        
        for entry in result.keys():
            self.assertIsInstance(entry, str)
        
    def test_parsefile_Returns_Dictionary_Contains_Dictionaries(self):
        import pymdb
        result = pymdb.parsefile(accessfile)
        
        for entry in result.keys():
            self.assertIsInstance(result[entry], dict)
    
    def test_Tables_KnownAccessFile(self):
        import pymdb
        result = pymdb.parsefile(accessfile)
        
        self.assertTrue(
            result == {
                'Table2': {'headers': ['ID','Field1'], 'data': [['1', 'Thing 1'], ['2', 'Thing 2'], ['3', 'Thing 3']]}, 
                'Table3': {'headers': ['ID','Field1'], 'data': [['1', 'Thing 1'], ['2', 'Thing 2'], ['3', 'Thing 3']]}, 
                'Table1': {'headers': ['ID','Field1'], 'data': [['1', 'Thing 1'], ['2', 'Thing 2'], ['3', 'Thing 3']]}
            }
        )
        self.assertTrue( "Table1" in result.keys())
        self.assertTrue( "Table2" in result.keys())
        self.assertTrue( "Table3" in result.keys())
        
        for entry in result.keys():
            headercount = len(result[entry]["headers"])
            self.assertTrue( len(result[entry]['data'] ) >= 3 )
            for element in result[entry]['data']:
                self.assertTrue( len(element) >= 2 )
                self.assertTrue( len(element) == headercount )