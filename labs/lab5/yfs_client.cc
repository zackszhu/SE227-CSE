// yfs client.  implements FS operations using extent and lock server
#include "yfs_client.h"
#include "extent_client.h"
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <map>
#include <stdio.h>
#define MIN(a,b) (a<b?a:b)
using namespace std;

yfs_client::yfs_client(std::string extent_dst, std::string lock_dst) 
{
    ec = new extent_client(extent_dst);
    lc = new lock_client(lock_dst);

    recoveryFile = fopen("zacks.log", "r");
    recovery();
    fclose(recoveryFile);

    ofs.open("zacks.log");
}

void yfs_client::recovery() {
    lc->acquire(0);
    list<Line> lines;
    std::string oper;
    char tempOper[10];
    inum ino = 0;
    int temp = 0;
    std::string data;
    while (fscanf(recoveryFile,"%s %lld %d",tempOper,&ino,&temp) != EOF) {
        oper = std::string(tempOper);
        if (oper == "old" && (temp > 0)) {
            fscanf(recoveryFile," ");
            char tempData[temp+1];
            fread(tempData,temp,1,recoveryFile);
            data = std::string(tempData);
        }

        lines.push_back(Line(oper,ino,temp,data));
        fscanf(recoveryFile,"\n");
        bzero(tempOper, sizeof(tempOper));
    }


    if ((int)lines.size() <= 0) {
        lc->release(0);
        return;
    }

    map<inum, Transaction*> fixmap;

    for (auto i = lines.begin(); i != lines.end(); i++) {
        if (i->temp == -1) {
            fixmap.erase(i->ino);
        }
        else {
            if (i->temp == -2) {
                if (fixmap.count(i->ino) > 0) {
                    fix(fixmap[i->ino]);
                    fixmap.erase(i->ino);
                    delete fixmap[i->ino];
                }
                if (i->oper == "CREATE" || i->oper == "MKDIR")
                    fixmap[i->ino] = new CreateTransaction(i->ino);
                else if (i->oper == "UNLINK")
                    fixmap[i->ino] = new UnlinkTransaction(i->ino);
                else if (i->oper == "SYMLINK")
                    fixmap[i->ino] = new SymLinkTransaction(i->ino);
                else
                    fixmap[i->ino] = new CommonTransaction(i->ino);
            }
            else
                fixmap[i->ino]->actions.push_back(*i);
        }
    }
    for (auto i = fixmap.begin(); i != fixmap.end(); i++) {
        fix(i->second);
        delete i->second;
    }
    lc->release(0);
}

void yfs_client::fix(Transaction* trans) {
    lc->release(trans->lock);
    trans->recovery(ec);
}

yfs_client::inum yfs_client::n2i(std::string n)
{
    std::istringstream ist(n);
    unsigned long long finum;
    ist >> finum;
    return finum;
}

std::string
yfs_client::filename(inum inum)
{
    std::ostringstream ost;
    ost << inum;
    return ost.str();
}

bool
yfs_client::isfile(inum inum)
{
    extent_protocol::attr a;

    if (ec->getattr(inum, a) != extent_protocol::OK)
    {
        return false;
    }

    if (a.type == extent_protocol::T_FILE)
    {
        return true;
    }
    return false;
}

/** Your code here for Lab...
 * You may need to add routines such as
 * readlink, issymlink here to implement symbolic link.
 *
 * */
bool yfs_client::issymlink(inum inum)
{
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK)
    {
        return false;
    }
    if (a.type == extent_protocol::T_LINK)
    {
        return true;
    }
    return false;
}


bool
yfs_client::isdir(inum inum)
{

    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK)
        return false;
    if (a.type == extent_protocol::T_DIR)
        return true;
    return false;
}


int
yfs_client::getfile(inum inum, fileinfo &fin)
{
    int r = OK;

    printf("getfile %016llx\n", inum);
    extent_protocol::attr a;


    if (ec->getattr(inum, a) != extent_protocol::OK)
    {
        r = IOERR;
        goto release;
    }

    fin.atime = a.atime;
    fin.mtime = a.mtime;
    fin.ctime = a.ctime;
    fin.size = a.size;
    printf("getfile %016llx -> sz %llu\n", inum, fin.size);

release:
    return r;
}

int
yfs_client::getdir(inum inum, dirinfo &din)
{
    int r = OK;

    printf("getdir %016llx\n", inum);
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK)
    {
        r = IOERR;
        goto release;
    }
    din.atime = a.atime;
    din.mtime = a.mtime;
    din.ctime = a.ctime;

release:
    return r;
}


#define EXT_RPC(xx) do { \
    if ((xx) != extent_protocol::OK) { \
        printf("EXT_RPC Error: %s:%lld \n", __FILE__, __LINE__); \
        r = IOERR; \
        goto release; \
    } \
} while (0)

// Only support set size of attr
int
yfs_client::setattr(inum ino, size_t size)
{
    ofs << "SETATTR " << ino << " -2\n";
    ofs.flush();
    lc->acquire(ino);
    int r = OK;

    string buf;   
    if (ec->get(ino, buf) != extent_protocol::OK) {
        r = IOERR;
        lc->release(ino);
        ofs << "SETATTR " << ino << " -1\n";
        ofs.flush();
        return r;
    }
    size_t bufsize = buf.size();
    if(size > bufsize)
        buf.resize(size, '\0'); 
    else 
        buf.resize(size);

    ec->put(ino, buf);
    lc->release(ino);
    ofs << "SETATTR " << ino << " -1\n";
    ofs.flush();
    return r;
}

//mode -unsigned int
//file
int
yfs_client::create(inum parent, const char *name, mode_t mode, inum &ino_out)
{
    ofs << "CREATE " << parent << " -2\n";
    ofs.flush();
    lc->acquire(parent);

    int r = OK;
    std::string buf;
    bool found;
    lookup_nolock(parent, name, found, ino_out);
    if (found == true) {
        lc->release(parent);
        ofs << "CREATE " << parent << " -1\n";
        ofs.flush();
        return EXIST;
    }

    ec->create(extent_protocol::T_FILE,ino_out);

    ofs << "new " << parent << " " << ino_out << std::endl;
    ofs.flush();

    string dir_data;
    ec->get(parent,dir_data);
    stringstream str_num;

    ofs << "old " << parent << " " << int(dir_data.size()) << " " << dir_data << std::endl;
    ofs.flush();

    str_num << ino_out;

    dir_data += name;
    dir_data += "/";
    dir_data += str_num.str() + "/";

    ec->put(parent,dir_data);
    lc->release(parent);
    ofs << "CREATE " << parent << " -1\n";
    ofs.flush();

    return r;
}

int
yfs_client::lookup_nolock(inum parent, const char *name, bool &found, inum &ino_out)
{
    int r = OK;

    /*
     * your lab2 code goes here.
     * note: lookup file from parent dir according to name;
     * you should design the format of directory content.
     */

    string dir_data;
    ec->get(parent,dir_data);
    string tname("");
    inum tempInum = 0;
    int i=0;
    bool state=true;

    while (i < (int)dir_data.size() && dir_data[i] != '\0') {
        if (dir_data[i] == '/') {
            if (state)
                tempInum = 0;
            else {
                if (strcmp(tname.c_str(),name) == 0) {
                    found = true;
                    ino_out = tempInum;
                    return r;
                }
                tname.clear();
            }
            state = !state;
        }
        else {
            if (state)
                tname+=dir_data[i];
            else 
                tempInum = tempInum * 10 + (int)(dir_data[i] - '0');
        }
        i++;
    }
    found = false;
    return IOERR;
}

int
yfs_client::mkdir(inum parent, const char *name, mode_t mode, inum &ino_out)
{
    ofs << "MKDIR " << parent << " -2\n";
    ofs.flush();

    lc->acquire(parent);
    int r = OK;

    std::string buf;
    bool found;
    lookup_nolock(parent, name, found, ino_out);
    if (found == true) {
        lc->release(parent);
        ofs << "MKDIR " << parent << " -1\n";
        ofs.flush();
        return EXIST;
    }

    ec->create(extent_protocol::T_DIR,ino_out);
    ofs << "new " << parent << " " << ino_out << std::endl;
    ofs.flush();

    string dir_data;
    ec->get(parent,dir_data);
    stringstream str_num;
    ofs << "old " << parent << " " << int(dir_data.size()) << " " << dir_data << std::endl;
    ofs.flush();

    str_num << ino_out;
    dir_data += name;
    dir_data += "/" + str_num.str() + "/";
    ec->put(parent,dir_data);

    lc->release(parent);
    ofs << "MKDIR " << parent << " -1\n";
    ofs.flush();
    return r;
}

int yfs_client::lookup(inum parent, const char *name, bool &found, inum &ino_out)
{
    int r = OK;
    ofs << "LOOKUP " << parent << " -2\n";
    ofs.flush();
    lc->acquire(parent);
    /*
     * your lab2 code goes here.
     * note: lookup file from parent dir according to name;
     * you should design the format of directory content.
     */

    string dir_data;
    ec->get(parent,dir_data);
    string tname("");
    inum tempInum = 0;
    int i = 0;
    bool state = true;

    while (i < (int)dir_data.size() && dir_data[i] != '\0')
    {
        if (dir_data[i] == '/') {
            if (state)
                tempInum = 0;
            else {
                if (strcmp(tname.c_str(),name) == 0) {
                    found = true;
                    ino_out = tempInum;
                    lc->release(parent);
                    ofs << "LOOKUP " << parent << " -1\n";
                    ofs.flush();
                    return r;
                }
                tname.clear();
            }
            state = !state;
        }
        else {
            if (state)
                tname += dir_data[i];
            else 
                tempInum = tempInum * 10 + (int)(dir_data[i] - '0');
        }
        i++;
    }
    found = false;
    lc->release(parent);
    ofs << "LOOKUP " << parent << " -1\n";
    ofs.flush();
    return IOERR;
}

int yfs_client::readdir(inum dir, std::list<dirent> &list)
{
    int r = OK;
    ofs << "READDIR " << dir << " -2\n";
    ofs.flush();
    lc->acquire(dir);

    string dir_data;
    ec->get(dir,dir_data);
    string name;
    inum num = 0;
    int i = 0;
    bool state = true;
    while (i < (int)dir_data.size() && dir_data[i] != '\0') {
        if (dir_data[i] == '/') {
            if (state)
                num=0;
            else {
                list.push_back(dirent(name,num));
                name.clear();
            }
            state = !state;
        }
        else {
            if (state)
                name += dir_data[i];
            else
                num = num * 10 + (int)dir_data[i];
        }
        i++;
    }
    lc->release(dir);
    ofs << "READDIR " << dir << " -1\n";
    ofs.flush();
    return r;
}

int yfs_client::read(inum ino, size_t size, off_t off, std::string &data)
{
    ofs << "READ " << ino << " -2\n";
    ofs.flush();
    lc->acquire(ino);
    int r = OK;

    string buf;
    if(ec->get(ino, buf) != extent_protocol::OK) {
        r = IOERR;
        lc->release(ino);
        ofs << "READ " << ino << " -1\n";
        ofs.flush();
        return r;
    }
    int length = buf.size();
    if (off <= length) {
        int total = off + size < length ? off + size : length;
        data = buf.substr(off, total - off);
    }
    else
        data = "\0";

    lc->release(ino);
    ofs << "READ " << ino << " -1\n";
    ofs.flush();
    return r;
}

int
yfs_client::read_unlock(inum ino, size_t size, off_t off, std::string &data)
{
    int r = OK;
    /*
        *-
         * your lab2 code goes here.
         * note: read using ec->get().
         */
    string buf;
        if(ec->get(ino, buf) != extent_protocol::OK) {
            r = IOERR;
            return r;
        }
        int length = buf.size();
        if (off <= length) {
            int total = off + size < length ? off + size : length;
            data = buf.substr(off, total - off);
        }
        else
            data = "\0";
    return r;
}

int
yfs_client::write(inum ino, size_t size, off_t off, const char *data,
                  size_t &bytes_written)
{
    ofs << "WRITE " << ino << " -2\n";
    ofs.flush();

    lc->acquire(ino);

    int r = OK;

    string buf, dataBuf(data, size);
    if(ec->get(ino, buf) != extent_protocol::OK) {
        r = IOERR;
        lc->release(ino);
        ofs << "WRITE " << ino << " -1\n";
        ofs.flush();
        return r;
    }

    if (off + size > buf.size())
        buf += std::string(off + size - buf.size(), 0);
    buf.replace(off, size, dataBuf);
    bytes_written = size;
    ec->put(ino, buf);
    lc->release(ino);

    ofs << "WRITE " << ino << " -1\n";
    ofs.flush();


    return r;
}
int yfs_client::unlink(inum parent,const char *name)
{
    int r = OK;
    ofs << "UNLINK " << parent << " -2\n";
    ofs.flush();
    lc->acquire(parent);
    ofs << "old " << parent << " " << strlen(name) << " " << name << std::endl;
    bool found=false;

    string dir_data;
    ec->get(parent,dir_data);
    string tname("");
    inum tempInum = 0;
    int i = 0;
    bool state = true;

    int pre = 0;
    while (i < (int)dir_data.size() && dir_data[i] != '\0' ) {
        if (dir_data[i] == '/') {
            if (state)
                tempInum = 0;
            else {
                if (strcmp(tname.c_str(),name) == 0) {
                    found = true;
                    ec->remove(tempInum);
                    break;
                }
                tname.clear();
                pre = i+1;
            }
            state = !state;
        }
        else {
            if (state)
                tname += dir_data[i];
            else 
                tempInum = tempInum * 10 + (int)(dir_data[i] - '0');
        }
        i++;
    }

    if (!found) {
        lc->release(parent);
        ofs << "UNLINK " << parent << " -1\n";
        ofs.flush();
        return ENOENT;
    }
    else {
        dir_data.erase(pre,i-pre+1);
        ec->put(parent,dir_data);
    }
    lc->release(parent);
    ofs << "UNLINK " << parent << " -1\n";
    ofs.flush();
    return r;
}

int yfs_client::symlink(const char *link, inum parent, const char *name, inum &ino)
{
    ofs << "SYMLINK " << parent << " -2\n";
    ofs.flush();
    lc->acquire(parent);
    int r = OK;
    bool found = false;
    std::string dir_data;
    lookup_nolock(parent, name, found, ino);
    if (found == true) {
        lc->release(parent);
        ofs << "SYMLINK " << parent << " -1\n";
        ofs.flush();
        return EXIST;
    }

    ec->get(parent, dir_data);
    ofs << "old " << parent << " " << int(dir_data.size()) << " " << dir_data << std::endl;
    ofs.flush();

    ec->create(extent_protocol::T_LINK, ino);

    ofs << "new " << parent << " " << ino << std::endl;
    ofs.flush();

    stringstream ss;
    ss<<ino;
    dir_data += name;
    dir_data += "/";
    dir_data += ss.str() + "/";


    ec->put(parent, dir_data);
    ec->put(ino, string(link));

    lc->release(parent);
    ofs << "SYMLINK " << parent << " -1\n";
    ofs.flush();
    return r;
}

int yfs_client::readlink(inum ino, std::string &result)
{
    ofs << "READLINK " << ino << " -2\n";
    ofs.flush();
    lc->acquire(ino);
    int r = OK;
    if(ec->get(ino, result) != extent_protocol::OK) {
        r = IOERR;
    }
    lc->release(ino);
    ofs << "READLINK " << ino << " -1\n";
    ofs.flush();
    return r;
}

int yfs_client::getsymlink(inum inum, symlinkinfo &symatt)
{
    ofs << "GETSYMLINK " << inum << " -2\n";
    ofs.flush();

    lc->acquire(inum);
    int r = OK;
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK)
    {
        lc->release(inum);
        ofs << "GETSYMLINK " << inum << " -1\n";
        ofs.flush();
        return  IOERR;
    }
    symatt.atime = a.atime;
    symatt.mtime = a.mtime;
    symatt.ctime = a.ctime;
    symatt.size = a.size;
    lc->release(inum);
    ofs << "GETSYMLINK " << inum << " -1\n";
    ofs.flush();
    return r;
}
