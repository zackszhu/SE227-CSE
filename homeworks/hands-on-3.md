# AnswerSheet

## Question 1

> Study helgrinds output and the ph.c program. Clearly something is wrong with put() on line 61. Why are there missing keys with 2 or more threads, but not with 1 thread? Identify a sequence of events that can lead to keys missing for 2 threads.

The data race appeared when add keys into the table. Here is the sequence of events that can lead to keys missing for 2 threads:

- T0: if (!table[b][i].inuse)   // when it is true

- T1: if (!table[b][i].inuse)   // also true

- T0: table[b][i]=key;          // add key1

- T1: table[b][i]=key;          // overwrite old key1 to key2, leads to the absence of key1

...

## Question 2

> Describe your changes and argue why these changes ensure correctness.

I modified the code in put like this:

    // Insert (key, value) pair into hash table.  
    static 
    void put(int key, int value)
    {
      int b = key % NBUCKET;
      int i;
        pthread_mutex_lock(&lock);
      // Loop up through the entries in the bucket to find an unused one:
      for (i = 0; i < NENTRY; i++) {
        if (!table[b][i].inuse) {
          table[b][i].key = key;
          table[b][i].value = value;
          table[b][i].inuse = 1;
            pthread_mutex_unlock(&lock);
          return;
        }
      }
      pthread_mutex_unlock(&lock);
      assert(0);
    }

Add locks so that it will check and wait until last putting finished.

## Question 3

> Is the two-threaded version faster than the single-threaded version in the put phase? Report the running times for the two-threaded version and the single-threaded version. (Make sure to undo your NKEYS change.)

Actually the two-threaded version is slower than the single-threaded version.

The time used by two-threaded version is 2.354, while the one of single-threaded version is 2.272.

## Question 4

> Most likely (depending on your exact changes) ph will not achieve any speedup. Why is that?

It is beacuse when we add lock like that, the program runs exactly like a achronized one and the additional lock operations slow it down.

## Question 5

> You should observe speedup on several cores for the get phase of ph. Why is that?

Yes, there is speedup. It is beacause that the removement of the lock in `get` function will make get operations go concurrently and will speed the process up.

## Question 6

> Why does valgrind report no errors for get()? Can you imagine a execution sequence where valgrind may also report error for get(), with current get() and put() functions?

Because the `main` ends all `put` operations up and then do the `get` operations, so the data race cannot be discovered. Let us imagine a execution like this:

- T0: 

        // lock is acquired
        if (!table[b][i].inuse) {
            table[b][i].key = key;

- T1:

        if (table[b][i].key == key && table[b][i].inuse) // because the inuse has not yet changed by T1, it cannot get the key which is actually wrote in table[b][i].

## Question 7

Declare an array of locks: `lock[NBUCKET]`

When getting or putting things, just aquire or release the corresponding bucket's lock instead a global lock.

It will behance the concurrency.
