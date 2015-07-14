#ifndef yfs_client_h
#define yfs_client_h

#include <string>
#include "lock_protocol.h"
#include "lock_client.h"
#include "extent_client.h"
#include <vector>
#include <fstream>

#include <list>
#include <string>
using namespace std;
typedef unsigned long long inum;
class Line {
public:
    Line (std::string o,inum i,int t,std::string d)
        :oper(o), ino(i), temp(t), data(d){}

    Line (const Line &l)
        :oper(l.oper), ino(l.ino), temp(l.temp), data(l.data) {}

    std::string oper;
    inum ino;
    int temp;
    std::string data;
};

class Transaction {
public:
    int lock;
    std::vector<Line> actions;
    Transaction(int lk):lock(lk) {}
    virtual void recovery(extent_client*) = 0;
};

class CommonTransaction : public Transaction{
public:
    CommonTransaction(int lk): Transaction(lk){}
    virtual void recovery(extent_client*) {}
};

class CreateTransaction : public Transaction{
public:
    CreateTransaction(int lk): Transaction(lk){}
    virtual void recovery(extent_client *ec) {
        auto i = actions.begin();
        if (i == actions.end() || i->oper != "new")
            return;
        ec->remove(i->temp);
        i++;
        if (i == actions.end() || i->oper != "put")
            return;
        ec->put(i->ino,string(i->data));
    }
};

class SymLinkTransaction : public Transaction{
public:
    SymLinkTransaction(int lk): Transaction(lk){}
    virtual void recovery(extent_client *ec) {
        auto i = actions.begin();
        if (i == actions.end() || i->oper != "old")
            return;
        ec->put(i->ino,string(i->data));
        if (i == actions.end() || i->oper != "new")
            return;
        ec->remove(i->temp);
    }
};

class UnlinkTransaction : public Transaction{
public:
    UnlinkTransaction(int lk): Transaction(lk){}
    virtual void recovery(extent_client *ec) {
        auto i = actions.begin();
        if (i == actions.end() || i->oper != "old")
            return;
        unlink_nologlock(i->ino,i->data, ec);
    }

private:
    void unlink_nologlock(inum parent,const std::string& name, extent_client *ec) {
        bool found=false;
        string dir_data;
        ec->get(parent,dir_data);
        string tname("");
        inum tinum=0;
        int i=0;
        bool state=true;
        int pre=0;
        while (i<(int)dir_data.size() && dir_data[i]!='\0') {
            if (dir_data[i]=='/') {
                if (state)
                    tinum=0;
                else {
                    if (tname == name) {
                        found=true;
                        ec->remove(tinum);
                        break;
                    }
                    tname.clear();
                    pre=i+1;
                }
                state=!state;
            }
            else {
                if (state) tname+=dir_data[i];
                else tinum=tinum*10+(int)(dir_data[i]-'0');
            }
            i++;
        }

        if (!found) {
            return ;
        }
        else {
            dir_data.erase(pre,i-pre+1);
            ec->put(parent,dir_data);
        }
        return;
    }
};

class yfs_client
{
    extent_client *ec;
    lock_client *lc;
    FILE *recoveryFile;
    ofstream ofs;

public:

    enum xxstatus { OK, RPCERR, NOENT, IOERR, EXIST };
    typedef int status;
    typedef unsigned long long inum;

    struct fileinfo
    {
        unsigned long long size;
        unsigned long atime;
        unsigned long mtime;
        unsigned long ctime;
    };
    struct dirinfo
    {
        unsigned long atime;
        unsigned long mtime;
        unsigned long ctime;
    };

    struct dirent
    {
        dirent(std::string n,yfs_client::inum i): name(n),inum(i){}
        std::string name;
        yfs_client::inum inum;
    };
    struct symlinkinfo
    {
        unsigned long long size;
        unsigned int atime;
        unsigned int mtime;
        unsigned int ctime;
    };

private:
    static std::string filename(inum);
    static inum n2i(std::string);
    void recovery();
    void fix(Transaction*);

public:
    yfs_client();
    yfs_client(std::string,std::string);
    ~yfs_client() { ofs.close(); };
    bool isfile(inum);
    bool isdir(inum);

    int getfile(inum, fileinfo &);
    int getdir(inum, dirinfo &);
    int setattr(inum, size_t);
    int lookup(inum, const char *, bool &, inum &);
    int lookup_nolock(inum parent, const char *name, bool &found, inum &ino_out);
    int create(inum, const char *, mode_t, inum &);
    int readdir(inum, std::list<dirent> &);
    int write(inum, size_t, off_t, const char *, size_t &);
    int read(inum, size_t, off_t, std::string &);
    int read_unlock(inum, size_t, off_t, std::string &);
    int unlink(inum,const char *);
    int mkdir(inum , const char *, mode_t , inum &);
    int readlink(inum ino, std::string &result);
    int symlink(const char *link, inum parent, const char *name, inum &ino);
    int getsymlink(inum inum, symlinkinfo &sin1);
    bool issymlink(inum inum);

};

#endif
