# Answer

## Question 1

> What exactly would happen if one block of one hard drive got erased during a map/reduce computation? What parts of the system would fix the error (if any), and what parts of the system would be oblivious (if any)?

When doing map/reduce tasks, the worker will be reset to idle and becomes eligible for rescheduling if it fails. All its work will be cleared and the task will be re-assigned to other worker. It is the master which finds and fixes this error. All other working workers including map and reduce will be oblivious to this error.

## Question 2

> How do "stragglers" impact performance?

Straggler is a machine that takes an unusually long time to complete one of the last few map or reduce tasks in the computation.

Because map/reduce is a big question and is consist of many workers, this unusually slow worker may delay the next part which is waiting for its result, and finally impact the whole system's performance.

## Question 3

> There is an article named "MapReduce: A Major Step Backwards" (URL). Please skim the article and try to understand why the authors had such statement. Do you agree them or not?

The author looks at the mapreduce algorithm in a database's perspective. He had five conserns with this computing paradigm.

1. MapReduce is a step backwards in database access: the DBMS community learned the importance of schemas decades ago, while mapreduce didn't apply it. There may be 'garbage' in MapReduce which it may silently break all the MapReduce applications, and the absence of schema may anguages.

2. MapReduce is a poor implementation: mapreduce use only brute force as a processing option instead of more efficient indexing like B-tree. In addition, the MapReduce may have two or more reduce instances read the same file which make faster 'push' operation impossible.

3. MapReduce is not novel: the techniques employed by MapReduce are more than 20 years old.

4. MapReduce misses features: mapreduce misses many features of DBMS such as bulk loader, updates, transactions and so on.

5. MapReduce is incompatible with the DBMS tools: many of the DBMS tools can't be used by mapreduce and mayreduce has none of its own.

I don't agree with the author, because the aim of mapreducing is not to manage data like a DBMS. Instead, it can calculate mass of data in parallel and finally output into files. MapReduce is a great progress not a major step backwards.

## Question 4

Will it perform better if we don't store map output locally but transfer them to corresponding reduce worker simultanously?