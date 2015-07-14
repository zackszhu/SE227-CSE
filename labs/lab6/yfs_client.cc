// yfs client.  implements FS operations using extent and lock server

#include "yfs_client.h"
#include "extent_client.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using std::cout;
using std::endl;
using std::string;
using std::stringstream;

void yfs_client::commit(int v) {
    logStream.close();
    char oldName[] = "temp.log";
    stringstream s;
    std::string tmp;
    s << "version-" << v << ".log";
    s >> tmp;
    rename(oldName, tmp.c_str());
    logStream.open("temp.log");
}

void yfs_client::gotoVersion(int v) {
    lc->acquire(1);
    if (ec->put(1, "") != extent_protocol::OK)
        printf("error init root dir\n"); 
    lc->release(1);
    for (int i = 2; i <= 50; i++) {
        ec->remove(i);
    }
    logStream.close();
    logStream.open("ttt.log", std::fstream::app);
    logStream << v << std::endl;
    for (int i = 1; i <= v; i++) {
        stringstream s;
        std::string tmp;
        s << "version-" << i << ".log";
        s >> tmp;
        logStream << tmp << std::endl;
        FILE* redoFile = fopen(tmp.c_str(), "r");
        char tempAction[10];
        while (fscanf(redoFile, "%s", tempAction) != EOF) {
            std::string action = std::string(tempAction);
            if (action == "setattr") {
                fscanf(redoFile, " ");
                inum ino;
                size_t size = 0;
                fscanf(redoFile, "%lld %ld", &ino, &size);
                logStream << "log: " << ino << " " << size;
                setattr(ino, size);
            }
            if (action == "create") {
                fscanf(redoFile, " ");
                inum ino;
                int size = 0;
                mode_t mode = 0;
                inum ino_out;
                fscanf(redoFile, "%lld %d %d %lld\n", &ino, &size, &mode, &ino_out);
                std::string str;
                for (int i = 0; i < size; i++) {
                    char c = getc(redoFile);
                    str += c;
                }
                create(ino, str.c_str(), mode, ino_out);
            }
            if (action == "mkdir") {
                fscanf(redoFile, " ");
                inum ino;
                int size = 0;
                mode_t mode = 0;
                inum ino_out;
                fscanf(redoFile, "%lld %d %d %lld\n", &ino, &size, &mode, &ino_out);
                std::string str;
                for (int i = 0; i < size; i++) {
                    char c = getc(redoFile);
                    str += c;
                }
                mkdir(ino, str.c_str(), mode, ino_out);
            }
            if (action == "write") {
                fscanf(redoFile, " ");
                inum ino;
                size_t size = 0;
                off_t off;
                int length = 0;
                fscanf(redoFile, "%lld %ld %ld %d\n", &ino, &size, &off, &length);
                // std::string str;
                // for (int i = 0; i < length; i++) {
                //     char c = getc(redoFile);
                //     str += c;
                // }
                std::string str;
                for (int i = 0; i < length; i++) {
                    int tmp;
                    fscanf(redoFile, "%d ", &tmp);
                    str += (char)tmp;
                }             
                size_t a;
                logStream << "log: " << str << std::endl;
                write(ino, size, off, str.c_str(), a);
            }
            if (action == "unlink") {
                fscanf(redoFile, " ");
                inum ino;
                int length = 0;
                fscanf(redoFile, "%lld %d\n", &ino, &length);
                std::string str;
                for (int i = 0; i < length; i++) {
                    char c = getc(redoFile);
                    str += c;
                }
                unlink(ino, str.c_str());
            }
            if (action == "symlink") {
                fscanf(redoFile, " ");
                inum ino;
                int length = 0;
                int size = 0;
                inum ino_out;
                fscanf(redoFile, "%lld %d %d %lld\n", &ino, &length, &size, &ino_out);
                std::string str;
                for (int i = 0; i < length; i++) {
                    char c = getc(redoFile);
                    str += c;
                }
                fscanf(redoFile, "\n");
                std::string str2;
                for (int i = 0; i < size; i++) {
                    char c = getc(redoFile);
                    str += c;
                }
                symlink(ino, str, str2, ino_out);
            }
            bzero(tempAction, sizeof(tempAction));
            fscanf(redoFile, "\n");
        }
        fclose(redoFile);
        logStream << tmp << std::endl;
    }
    logStream.close();
    logStream.open("temp.log");
}

yfs_client::yfs_client(std::string extent_dst, std::string lock_dst)
{
    ec = new extent_client(extent_dst);
    lc = new lock_client(lock_dst);
    logStream.open("temp.log");
    // lc->acquire(1);
    // if (ec->put(1, "") != extent_protocol::OK)
    //     printf("error init root dir\n"); 
    // lc->release(1);
}


yfs_client::inum
yfs_client::n2i(std::string n)
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
    lc->acquire(inum);
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK) {
        printf("error getting attr\n");
        lc->release(inum);
        return false;
    }

    if (a.type == extent_protocol::T_FILE) {
        printf("isfile: %lld is a file\n", inum);
        lc->release(inum);
        return true;
    } 
    printf("isfile: %lld is not a file\n", inum);
    lc->release(inum);
    return false;
}


bool
yfs_client::isdir(inum inum)
{
    lc->acquire(inum);
    extent_protocol::attr a;

    if (ec->getattr(inum, a) != extent_protocol::OK) {
        printf("error getting attr\n");
        lc->release(inum);
        return false;
    }

    if (a.type == extent_protocol::T_DIR) {
        printf("isdir: %lld is a dir\n", inum);
        lc->release(inum);
        return true;
    } 
    printf("isdir: %lld is not a dir\n", inum);
    lc->release(inum);
    return false;
}

bool
yfs_client::isdirNoLock(inum inum)
{
    extent_protocol::attr a;

    if (ec->getattr(inum, a) != extent_protocol::OK) {
        printf("error getting attr\n");
        return false;
    }

    if (a.type == extent_protocol::T_DIR) {
        printf("isdir: %lld is a dir\n", inum);
        return true;
    } 
    printf("isdir: %lld is not a dir\n", inum);
    return false;
}

int
yfs_client::getfile(inum inum, fileinfo &fin)
{
    lc->acquire(inum);
    int r = OK;

    printf("getfile %016llx\n", inum);
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK) {
        r = IOERR;
        goto release;
    }

    fin.atime = a.atime;
    fin.mtime = a.mtime;
    fin.ctime = a.ctime;
    fin.size = a.size;
    printf("getfile %016llx -> sz %llu\n", inum, fin.size);

release:
    lc->release(inum);
    return r;
}

int
yfs_client::getdir(inum inum, dirinfo &din)
{
    lc->acquire(inum);
    int r = OK;

    printf("getdir %016llx\n", inum);
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK) {
        r = IOERR;
        goto release;
    }
    din.atime = a.atime;
    din.mtime = a.mtime;
    din.ctime = a.ctime;

release:
    lc->release(inum);
    return r;
}

#define EXT_RPC(xx) do { \
    if ((xx) != extent_protocol::OK) { \
        printf("EXT_RPC Error: %s:%d \n", __FILE__, __LINE__); \
        r = IOERR; \
        goto release; \
    } \
} while (0)


int
yfs_client::setattr(inum ino, size_t size)
{
    logStream << "setattr " << ino << " " << size << std::endl;
    lc->acquire(ino);
    int r = OK;
    /*
     * your lab2 code goes here.
     * note: get the content of inode ino, and modify its content
     * according to the size (<, =, or >) content length.
     */
    string buf;   
    if (ec->get(ino, buf) != extent_protocol::OK) {
        r = IOERR;
        lc->release(ino);
        return r;
    }
    size_t bufsize = buf.size();
    if(size > bufsize)
        buf.resize(size, '\0'); 
    else 
        buf.resize(size);
    ec->put(ino, buf);
    lc->release(ino);
    return r;
}

int
yfs_client::create(inum parent, const char *name, mode_t mode, inum &ino_out)
{
    lc->acquire(parent);
    int r = OK;
    /*
     * your lab2 code goes here.
     * note: lookup is what you need to check if file exist;
     * after create file or dir, you must remember to modify the parent infomation.
     */
    bool found = false;
    lookupNoLock(parent, name, found, ino_out);
    if(!found)
    {
        ec->create(extent_protocol::T_FILE, ino_out);
        string buf;
        ec->get(parent,buf);
        buf.append(string(name) + ':' + filename(ino_out) + '|');
        ec->put(parent, buf);
    }
    else
        r = EXIST; 
    lc->release(parent);
    logStream << "create " << parent << " " << strlen(name) << " " << mode << " " << ino_out << std::endl << name << std::endl;
    return r;    
}

int
yfs_client::mkdir(inum parent, const char *name, mode_t mode, inum &ino_out)
{
    lc->acquire(parent);
    int r = OK;
    /*
     * your lab2 code goes here.
     * note: lookup is what you need to check if directory exist;
     * after create file or dir, you must remember to modify the parent infomation.
     */
    bool found = false;
    lookupNoLock(parent, name, found, ino_out);
    if(!found)
    {
        ec->create(extent_protocol::T_DIR, ino_out);
        string buf;
        ec->get(parent,buf);
        buf.append(string(name) + ':' + filename(ino_out) + '|');
        ec->put(parent, buf);
    }
    else
        r = EXIST; 
    lc->release(parent);
    logStream << "mkdir " << parent << " " << strlen(name) << " " << mode << " " << ino_out << std::endl << name << std::endl;
    return r;
}

int
yfs_client::lookup(inum parent, const char *name, bool &found, inum &ino_out)
{
    lc->acquire(parent);
    int ret = lookupNoLock(parent, name, found, ino_out);
    lc->release(parent);
    return ret;
}

int
yfs_client::lookupNoLock(inum parent, const char *name, bool &found, inum &ino_out)
{
    int r = OK;
    /*
     * your lab2 code goes here.
     * note: lookup file from parent dir according to name;
     * you should design the format of directory content.
     */
    if(!isdirNoLock(parent)) {
        found = false;
        return r;
    }

    std::string buf;
    ec->get(parent, buf);
    std::string filename(""), inode(""), temp(name);
    int pos = buf.find(temp + ":");
    if (pos != std::string::npos) {
        int posA = buf.find(":", pos + 1);
        int posB = buf.find("|", pos + 1);
        inode = buf.substr(posA + 1, posB - posA - 1);
        ino_out = n2i(inode);
        found = true;
        return r;
    }
    else
        found = false;
    return IOERR;
}

int
yfs_client::readdir(inum dir, std::list<dirent> &list)
{
    lc->acquire(dir);
    int r = OK;
    /*
     * your lab2 code goes here.
     * note: you should parse the dirctory content using your defined format,
     * and push the dirents to the list.
     */
     if (!isdirNoLock(dir)) {
        r = IOERR;
        lc->release(dir);
        return r;
     }

    string buf;
    if(ec->get(dir, buf) != extent_protocol::OK) {
        r = IOERR;
        lc->release(dir);
        return r;
    }

    ec->get(dir, buf);
    dirent d;
    std::string buffer("");
    for (std::string::iterator i = buf.begin(); i != buf.end(); i++) {
        if (*i == ':') {
            d.name = buffer;
            buffer = "";
        }
        else if (*i == '|') {
            d.inum = n2i(buffer);
            buffer = "";
            list.push_back(d);
        }
        else
            buffer += *i;
    }
    lc->release(dir);
    return r;
    
}

int
yfs_client::read(inum ino, size_t size, off_t off, std::string &data)
{
    lc->acquire(ino);
    int r = OK;
    /*
     * your lab2 code goes here.
     * note: read using ec->get().
     */
    string buf;
    if(ec->get(ino, buf) != extent_protocol::OK) {
        r = IOERR;
        lc->release(ino);
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
    return r;
}

int
yfs_client::write(inum ino, size_t size, off_t off, const char *data,
        size_t &bytes_written)
{
    logStream << "write " << ino << " " << size << " " << off << " " << strlen(data) << std::endl;
    for (int i = 0; i < strlen(data); i++) 
        logStream << (int)data[i] << " ";
    logStream << std::endl;
    lc->acquire(ino);
    int r = OK;
    /*
     * your lab2 code goes here.
     * note: write using ec->put().
     * when off > length of original file, fill the holes with '\0'.
     */
    string buf, dataBuf(data);
    dataBuf.resize(size, 0);
    ofstream fp("aaa.log", std::fstream::app);
    fp << dataBuf << std::endl;
    fp.close();
    if(ec->get(ino, buf) != extent_protocol::OK) {
        r = IOERR;
        lc->release(ino);
        return r;
    }

    if (off + size > buf.size())
        buf += std::string(off + size - buf.size(), 0);
    buf.replace(off, size, dataBuf);
    bytes_written = size;
    
    ec->put(ino, buf);
    lc->release(ino);
    return r;    
}

int yfs_client::unlink(inum parent,const char *name)
{
    logStream << "unlink " << parent << " " << strlen(name) << std::endl << name << std::endl;
    lc->acquire(parent);
    int r = OK;
    /*
     * your lab2 code goes here.
     * note: you should remove the file using ec->remove,
     * and update the parent directory content.
     */
     
    inum inodenum = 0;
    bool found = false;
    r = lookupNoLock(parent, name, found, inodenum);
    if (r == IOERR) {
        lc->release(parent);
        return r;
    }

    if(found) {
        std::string buf;
        ec->get(parent, buf);
        std::string filename(""), inode("");
        std::string temp(name);
        int pos = buf.find(temp + ":");
        if (pos != std::string::npos) {
            int posA = buf.find(":", pos + 1);
            int posB = buf.find("|", pos + 1);
            inode = buf.substr(posA + 1, posB - posA - 1);
            ec->remove(n2i(inode));
            // buf = buf.substr(0, pos) + buf.substr(posB + 1);
            buf.erase(pos, posB + 1 - pos);
        }
        else {
            lc->release(parent);
            return ENOENT;
        }
        ec->put(parent, buf);
    }
    else
        r = NOENT; 
    lc->release(parent); 
    return r;
}

int yfs_client::symlink(inum parent, const string name, const string link, inum &ino_out)
{
    logStream << "symlink " << parent << " " << name.length() << " " << link.length() << " " << ino_out << std::endl << name << std::endl << link << std::endl;
    lc->acquire(parent);
    int r = OK;

    bool found = false;
    lookupNoLock(parent, name.c_str(), found, ino_out);
    if (found) {
        r = EXIST;
        lc->release(parent);
        return r;
    }

    // create a link file
    ec->create(extent_protocol::T_LINK, ino_out);
    string buf;
    ec->get(parent,buf);
    buf.append(string(name) + ':' + filename(ino_out) + '|');
    ec->put(parent, buf);

    // write thing to link file
    ec->put(ino_out, link);
    lc->release(parent);
    return r;    
}



int yfs_client::readlink(inum ino, std::string &link)
{
    lc->acquire(ino);
    int r = OK;
    if(ec->get(ino, link) != extent_protocol::OK) {
        r = IOERR;
    }
    lc->release(ino);
    return r;
}
