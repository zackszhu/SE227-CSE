// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server():
    nacquire (0),mutex(PTHREAD_MUTEX_INITIALIZER),cond(PTHREAD_COND_INITIALIZER)
{
    lockmap.clear();
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
    lock_protocol::status ret = lock_protocol::OK;
    printf("stat request from clt %d\n", clt);
    r = nacquire;
    return ret;
}
/*
On the server, a lock can be in one of two states:
free: no clients own the client
locked: some client owns the lock
The RPC handler for acquire should first check if the lock is locked,
and if so, the handler should block until the lock is free.
When the lock is free,acquire changes its state tolocked, then returns to the client,
which indicates that the client now has the lock. The valuer returned by acquiredoesn't matter.
The handler for release should change the lock state to free, and notify any threads that are waiting for the lock.
Consider using the C++ STL (Standard Template Library) std::map class to hold the table of lock states.
*/


lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r)
{
    lock_protocol::status ret = lock_protocol::OK;
    // Your lab4 code goes here

    printf("ACQUIRE cid:%d \t lid:%d\n",clt,lid);
    fflush(stdout);
    pthread_mutex_lock(&mutex);
    if (lockmap.find(lid) != lockmap.end())
    {
        while (lockmap[lid]==true)
            pthread_cond_wait(&cond,&mutex);
    }
    lockmap[lid]=true;

    pthread_mutex_unlock(&mutex);
    return ret;
}

lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r)
{
    lock_protocol::status ret = lock_protocol::OK;
    // Your lab4 code goes here
    printf("RELEASE cid:%d \t lid:%d\n",clt,lid);
    fflush(stdout);
    pthread_mutex_lock(&mutex);
    if (lockmap.find(lid) == lockmap.end())
        ret = lock_protocol::NOENT;
    else
    {
        lockmap[lid]=false;
        pthread_cond_broadcast(&cond);
    }

    pthread_mutex_unlock(&mutex);
    return ret;
}