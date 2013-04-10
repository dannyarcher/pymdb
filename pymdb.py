import pymdbbin

class PymdbStrategy:
  table_dictionary  = 1
  objects           = 2
  
def parsefile(file, parsestrategy=PymdbStrategy.table_dictionary):
    if(parsestrategy==PymdbStrategy.table_dictionary):
        return pymdbbin.parsefile_table_dictionary(file)
    return None
