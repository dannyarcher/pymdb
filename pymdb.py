import pymdbbin

class Strategy:
  table_dictionary = range(1)
  
def parsefile(file, parsestrategy=Strategy.table_dictionary):
    if(parsestrategy==Strategy.table_dictionary):
        return pymdbbin.parsefile_table_dictionary(file)
    return None
