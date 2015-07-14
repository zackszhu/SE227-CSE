# Answer

## Question 1

> According to the lockset algorithm, when does eraser signal a data race? Why is this condition chosen?

It signals a data race when C(v), the set of candidate locks of v, is {}.

This condition is chosen because the represents that the new access to the resource does not have a lock or has a wrong lock which will cause data race.

## Question 2

> Under what conditions does Eraser report a false positive? What conditions does it produce false negatives?

The conditions of false alarms (false positive):

- Memory reuse: if the program implement free lists or private allocators and reuse the memory without resetting the shadow memory.

- Private locks: if the programmer have private implementations of reader/writer locks, it will alarm that the lock is taken privately.

- Benign races: some data races are benign, and do not need to alarm.

The conditions of false negatives:

- If we refine the algorithm to adapt the Trestle programs that protected shared locations with multiple locks, it may prevent the false alarms but also will possibly cause false negatives.

## Question 3
    buf += "|";

> Typically, instrumenting a program changes the intra-thread timing (the paper calls it interleaving). This can cause bugs to disappear when you start trying to find them. What aspect of the Eraser design mitigates this problem? 

The lockset algorithm which Eraser based on deals with set operations and is not so affected by the interleaving of threads, because the sequence is not cared about, but what lock it is holding does matter.

## Question 4

> Please raise at least one question of your own for the discussion.

I am still not quite familiar with the condition which causes false negatives. In addition, I wonder that is there any improvements that has been applied on real data race monitor?
