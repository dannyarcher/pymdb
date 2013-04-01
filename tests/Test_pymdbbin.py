import nose
import unittest

accessfile = "tests/StandardAccess.mdb"

class pymdbbin(unittest.TestCase):
    def tearDown(self):
        pymdbbin = None
    def test_import(self):
        import pymdbbin
        self.assertIsNotNone(pymdbbin)
    
    def test_parsefile_Exists(self):
        import pymdbbin
        self.assertTrue(pymdbbin.parsefile_table_dictionary!=None)

    def test_parsefileNone_Runs(self):
        import pymdbbin
        result = pymdbbin.parsefile_table_dictionary("")
        self.assertIsNone(result)
    
    def test_parsefile_Runs(self):
        import pymdbbin
        result = pymdbbin.parsefile_table_dictionary(accessfile)
        self.assertIsNotNone(result)
        
    def test_parsefile_Returns_Tables(self):
        import pymdbbin
        result = pymdbbin.parsefile_table_dictionary(accessfile)
        self.assertGreater(len(result.keys()), 0)
    
    def test_parsefile_Returns_Dictionary(self):
        import pymdbbin
        result = pymdbbin.parsefile_table_dictionary(accessfile)
        self.assertIsInstance(result, dict)
        
    def test_parsefile_Returns_Dictionary_Keys(self):
        import pymdbbin
        result = pymdbbin.parsefile_table_dictionary(accessfile)
        
        for entry in result.keys():
            self.assertIsInstance(entry, str)
    
        
    def test_parsefile_Returns_Dictionary_Contains_Dictionaries(self):
        import pymdbbin
        result = pymdbbin.parsefile_table_dictionary(accessfile)
        
        for entry in result.keys():
            self.assertIsInstance(result[entry], dict)
    
    def test_Tables_KnownAccessFile(self):
        import pymdbbin
        result = pymdbbin.parsefile_table_dictionary(accessfile)
            
            
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