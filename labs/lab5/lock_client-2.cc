// RPC stubs for clients to talk to lock_server

#include "lock_client.h"
#include "rpc.h"
#include <arpa/inet.h>

#include <sstream>
#include <iostream>
#include <stdio.h>
#include <fstream>

lock_client::lock_client(std::string dst)
{
  sockaddr_in dstsock;
  make_sockaddr(dst.c_str(), &dstsock);
  cl = new rpcc(dstsock);
  if (cl->bind() < 0) {
    printf("lock_client: call bind\n");
  }
  printf("Lock Client Init\n");
}

int
lock_client::stat(lock_protocol::lockid_t lid)
{
  int r;
  lock_protocol::status ret = cl->call(lock_protocol::stat, cl->id(), lid, r);
  VERIFY (ret == lock_protocol::OK);
  return r;
}

lock_protocol::status
lock_client::acquire(lock_protocol::lockid_t lid)
{
  // Your lab4 code goes here
  int r;
  printf("Acquire %lld\n", lid);
  lock_protocol::status ret = cl->call(lock_protocol::acquire, cl->id(), lid, r);
  VERIFY (ret == lock_protocol::OK);
  printf("Acquire Done\n");
  return r;
}

lock_protocol::status
lock_client::release(lock_protocol::lockid_t lid)
{
  std::ofstream ofs("lock_client.log", std::fstream::app);
  ofs << "Hello client" << std::endl;
  // Your lab4 code goes here
  int r;
  printf("Release %lld\n", lid);
  lock_protocol::status ret = cl->call(lock_protocol::release, cl->id(), lid, r);
  ofs << ret <<std::endl;
  ofs.close();
  VERIFY (ret == lock_protocol::OK);
  printf("Release done\n");
  return r;
}
