# Answer

## Question 1

> What do the first two parameters to WordCounts __init__ method control?

The number of processes allowed to do maptask and the number of processes allowed to do reducetask.

## Question 2

> Briefly explain how calling run triggers calls to both the map and reduce methods of the WordCount instance.

The map(func, iterable[, chunksize]) assigns the values in the iterable object to the function as parameters, then calls the function in different processes for several times.

## Question 3

> What do the parameters keyvalue and value of the map method in WordCount represent?

keyvalue: the offset of this file

value: the content of this partrition

## Question 4

> What do the parameters key and keyvalues of the reduce method in WordCount represent?

key: all titlecase words in the hashed file

keyvalues: all the tuples of the `key` word as (`key`, 1)

## Question 5

> How many invocations are there to doMap and how many to doReduce? Why?

4 and 2, it is decided by the value of maptask and reducetask

## Question 6

> Which invocations run in parallel? (Assuming there are enough cores.)

All `doMap`s run in parallel while all `doReduce`s run in parallel too.

## Question 7

> How much input (in number of bytes) does a single doMap process?

1208691, 1208697, 1208686 and 1208683

## Question 8

> How much input (in number of keys) does a single doReduce process?

2250 and 2222


## Question 9

> For which parameters of maptask and reducetask do you see speedup? Why do you observe no speedup for some parameters? (You might see no speedup at all on a busy machine or a machine with a single core.)

The addition of reducetask speed the program up but not increasing the value too much. As long as the number of processes is too big it will not speedup any more because the cores are limited.

## Question 10

> Include the code for your ReverseIndex class in your submission. 

    def Map(self, keyvalue, value):
        results = []
        i = 0
        n = len(value)
        while i < n:
            # skip non-ascii letters in C/C++ style a la MapReduce paper:
            while i < n and value[i] not in string.ascii_letters:
                i += 1
            start = i
            while i < n and value[i] in string.ascii_letters:
                i += 1
            w = value[start:i]
            if start < i and w.istitle():
                results.append ((w.lower(), start + int(keyvalue)))
        return results


    def Reduce(self, key, keyvalues):
        return (key, [pair[1] for pair in keyvalues])


