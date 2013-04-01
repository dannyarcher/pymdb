import pymdbbin
from enum import Enum

strategy = Enum('table_dictionary')

def parsefile(file, parsestrategy=strategy.table_dictionary):
    if(parsestrategy==strategy.table_dictionary):
        return pymdbbin.parsefile_table_dictionary(file)
    return None
