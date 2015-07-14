// the lock server implementation

#include "lock_server.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

lock_server::lock_server():
  nacquire (0)
{
  std::ofstream ofss("serverstart.log", std::fstream::app);
  ofss << "start!" << std::endl;
  ofss.close();
  lockMap.clear();
  pthread_mutex_init(&mutexLock, NULL);
  pthread_cond_init(&condLock, NULL);
}

lock_protocol::status
lock_server::stat(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
  printf("stat request from clt %d\n", clt);
  pthread_mutex_lock(&mutexLock);
  r = lockMap[lid].times;
  pthread_mutex_unlock(&mutexLock);
  return ret;
}

lock_protocol::status
lock_server::acquire(int clt, lock_protocol::lockid_t lid, int &r)
{
  lock_protocol::status ret = lock_protocol::OK;
	// Your lab4 code goes here
  if (clt < 0) 
    return lock_protocol::RPCERR;
  pthread_mutex_lock(&mutexLock);
  if (lockMap.find(lid) != lockMap.end()) {
    while (!lockMap[lid].isFree){
      pthread_cond_wait(&condLock, &mutexLock);
    }
  }
  lockMap[lid].isFree = false;
  lockMap[lid].times++;
  lockMap[lid].user = clt;
  pthread_mutex_unlock(&mutexLock);
  return ret;
}

lock_protocol::status
lock_server::release(int clt, lock_protocol::lockid_t lid, int &r)
{
  printf("Hello server %lld\n", lid);
  struct timeval tv;    
  gettimeofday(&tv,NULL);    
  printf("time: %lld\n", tv.tv_sec * 1000 + tv.tv_usec / 1000);
  lock_protocol::status ret = lock_protocol::OK;
	// Your lab4 code goes here
  pthread_mutex_lock(&mutexLock);
  if (lockMap.find(lid) == lockMap.end() || lockMap[lid].isFree) {
    pthread_mutex_unlock(&mutexLock);
    return lock_protocol::NOENT;
  }
  if (lockMap[lid].user != clt) {
    pthread_mutex_unlock(&mutexLock);
    return lock_protocol::RPCERR;
  }
  lockMap[lid].isFree = true;
  lockMap[lid].user = -1;
  pthread_cond_broadcast(&condLock);
  pthread_mutex_unlock(&mutexLock);
  return ret;
}
