import sys
import re

from pyspark import SparkContext

def flat_map(document):
    """
    Takes in document, which is a key, value pair, where document[0] is the
    document ID and document[1] is the contents of the document.
    HINT: You need to keep track of three things, word, document ID, and the
    index inside of the document, but you are working with key, value pairs.
    Is there a way to combine these three things and make a key, value pair?
    """
    """ Your code here. """
    ret = []
    wordlist = re.findall(r"\w+", document[1])
    for i in range(len(wordlist)):
        tup1 = (wordlist[i],document[0])
        tup2 = i
        tup3 = (tup1,tup2)
        ret.append(tup3)
    return ret#re.findall(r"\w+", document[1])

def map(arg):
    """ Your code here. """
    return(arg)
    #return (arg, arg)

def reduce(arg1, arg2):
    """ Your code here. """
    #for i in range(len(arg2)):

    return str(arg1)+" "+str(arg2)

def index(file_name, output="spark-wc-out-index"):
    sc = SparkContext("local[8]", "Index")
    file = sc.sequenceFile(file_name)

    indices = file.flatMap(flat_map) \
                  .map(map) \
                  .reduceByKey(reduce).sortByKey(ascending = True)

    indices.coalesce(1).saveAsTextFile(output)

""" Do not worry about this """
if __name__ == "__main__":
    argv = sys.argv
    if len(argv) == 2:
        index(argv[1])
    else:
        index(argv[1], argv[2])
