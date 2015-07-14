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
#include <map>
using std::cout;
using std::endl;
using std::string;
using std::stringstream;

void yfs_client::logStart(const std::string& n, int p) {
    logFile << "BEGIN" << " " << n << " " << p << endl;
    logFile.flush();
}

void yfs_client::logEnd(const std::string& n, int p) {
    logFile << "END" << " " << n << " " << p << endl;
    logFile.flush();
}

yfs_client::yfs_client(std::string extent_dst, std::string lock_dst)
{
    ec = new extent_client(extent_dst);
    lc = new lock_client(lock_dst);
    // std::ifstream checkStream("zacks.log", std::ios::binary);
    // checkStream.seekg(0, std::ios::end);
    // int length = checkStream.tellg();
    // checkStream.close();
    std::string buff;
    ec->get(1, buff);
    if (buff == "") {
        logFile.open("zacks.log", std::fstream::out);
        logFile << "First Blood!" << endl;
        logFile.flush();
        logStart("Con", 1);
        logFile << "A 1" << endl;
        logFile.flush();
        lc->acquire(1);
        if (ec->put(1, "") != extent_protocol::OK)
            printf("error init root dir\n"); 
        lc->release(1);
        logFile << "R 1" << endl;
        logFile.flush();
        logEnd("Con", 1);
    }
    else {
        recovery();
    }
    
}

// void readn(std::fstream& fst, int length, string& str) {
//     int i = 0;
//     while (i++ < length) {
//         char c;
//         fst >> c;
//         str += c;
//     }
// }

void yfs_client::recovery() {
    FILE* recoveryFile("zacks.log", "r");
    // std::ifstream recoveryStream("zacks.log");
    std::string temp;
    std::map<int, Transaction> trans;
    int flag = 0;
    std::ofstream tmpStream("tmp.log", std::fstream::app);
    char a[10];
    while (fscanf(recoveryFile, "%s", a)) {
        if (strcmp(a, "BEGIN") == 0) {
            char name[10];
            int id;
            fscanf(recoveryFile, "%s %d", name, &id);
            trans[id] = Transaction();
            trans[id].begin = true;
            flag = id;
            tmpStream << temp << name << id << std::endl;
        }
        if (strcmp())
    }
    // while (recoveryStream >> temp) {
    //     if (temp == "BEGIN") {
    //         std::string name;
    //         int id;
    //         recoveryStream >> name >> id;
    //         trans[id] = Transaction();
    //         trans[id].begin = true;
    //         flag = id;
    //         tmpStream << temp << name << id << std::endl;
    //     }
    //     if (temp == "END") {
    //         std::string name;
    //         int id;
    //         recoveryStream >> name >> id;
    //         trans[flag].end = true;
    //         trans.erase(flag);
    //         flag = 0;
    //         tmpStream << temp << name << id << std::endl;
    //     }
    //     if (temp == "P") {
    //         inum ino;
    //         int length;
    //         std::string buff;
    //         recoveryStream >> ino >> length;
    //         char tmp;
    //         recoveryStream.get(tmp);
    //         int i = 0;
    //         while (i++ < length) {
    //             char c;
    //             recoveryStream.get(c);
    //             buff += c;
    //         }
    //         // readn(recoveryStream, length, buff);
    //         trans[flag].actions.push_back(new Transaction::PutAction('P', ino, length, buff));
    //         tmpStream << temp << ino << length << std::endl;
    //     }
    //     if (temp == "D") {
    //         inum ino;
    //         extent_protocol::types type;
    //         int length;
    //         std::string buff;
    //         recoveryStream >> ino;
    //         int tmpType;
    //         recoveryStream >> tmpType;
    //         type = extent_protocol::types(tmpType);
    //         recoveryStream >> length;
    //         char tmp;
    //         recoveryStream.get(tmp);
    //         int i = 0;
    //         while (i++ < length) {
    //             char c;
    //             recoveryStream.get(c);
    //             buff += c;
    //         }
    //         // readn(recoveryStream, length, buff);
    //         trans[flag].actions.push_back(new Transaction::RemoveAction('D', ino, length, type, buff));
    //         tmpStream << temp << ino << tmpType << length << std::endl;
    //     }
    //     if (temp == "C") {
    //         inum ino;
    //         extent_protocol::types type;
    //         int tmpType;
    //         recoveryStream >> tmpType;
    //         recoveryStream >> ino;
    //         type = extent_protocol::types(tmpType);
    //         trans[flag].actions.push_back(new Transaction::CreateAction('C', ino, type));            
    //         tmpStream << temp << ino << tmpType << std::endl;
    //     }
    //     if (temp == "A") {
    //         int num;
    //         recoveryStream >> num;
    //         trans[flag].lockAcquire = true;
    //         trans[flag].lockNum = num;
    //         tmpStream << temp << num << std::endl;
    //     }
    //     if (temp == "R") {
    //         int num;
    //         recoveryStream >> num;
    //         trans[flag].lockRelease = true;
    //         tmpStream << temp << num << std::endl;
    //     }
    // }

    for (std::map<int, Transaction>::iterator iter = trans.begin(); iter != trans.end(); iter++) {
        iter->second.outputLog();
    }
    for (std::map<int, Transaction>::iterator iter = trans.begin(); iter != trans.end(); iter++) {
        std::ofstream ofs("some.log", std::fstream::app);
        ofs << "Go recovery" << iter->first << std::endl;
        iter->second.recovery(ec, lc);
    }
    recoveryStream.close();
    logFile.open("zacks.log", std::fstream::out);
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
    logStart("ISF", inum);
    logFile << "A " << inum << endl;
    logFile.flush();
    lc->acquire(inum);
    extent_protocol::attr a;
    if (ec->getattr(inum, a) != extent_protocol::OK) {
        printf("error getting attr\n");
        lc->release(inum);
        logFile << "R " << inum << endl;
        logFile.flush();
        logEnd("ISF", inum);
        return false;
    }

    if (a.type == extent_protocol::T_FILE) {
        printf("isfile: %lld is a file\n", inum);
        lc->release(inum);
        logFile << "R " << inum << endl;
        logFile.flush();
        logEnd("ISF", inum);
        return true;
    } 
    printf("isfile: %lld is not a file\n", inum);
    lc->release(inum);
    logFile << "R " << inum << endl;
    logFile.flush();
    logEnd("ISF", inum);
    return false;
}


bool
yfs_client::isdir(inum inum)
{
    logStart("ISD", inum);
    logFile << "A " << inum << endl;
    logFile.flush();
    lc->acquire(inum);
    extent_protocol::attr a;

    if (ec->getattr(inum, a) != extent_protocol::OK) {
        printf("error getting attr\n");
        lc->release(inum);
        logFile << "R " << inum << endl;
        logFile.flush();
        logEnd("ISD", inum);
        return false;
    }

    if (a.type == extent_protocol::T_DIR) {
        printf("isdir: %lld is a dir\n", inum);
        lc->release(inum);
        logFile << "R " << inum << endl;
        logFile.flush();
        logEnd("ISD", inum);
        return true;
    } 
    printf("isdir: %lld is not a dir\n", inum);
    lc->release(inum);
    logFile << "R " << inum << endl;
    logFile.flush();
    logEnd("ISD", inum);
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
    logStart("GF", inum);
    logFile << "A " << inum << endl;
    logFile.flush();
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
    logFile << "R " << inum << endl;
    logFile.flush();
    logEnd("GF", inum);
    return r;
}

int
yfs_client::getdir(inum inum, dirinfo &din)
{
    logStart("GD", inum);
    logFile << "A " << inum << endl;
    logFile.flush();
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
    logFile << "R " << inum << endl;
    logFile.flush();
    logEnd("GD", inum);
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
    logStart("SA", ino);
    logFile << "A " << ino << endl;
    logFile.flush();
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
        logFile << "R " << ino << endl;
        logFile.flush();
        logEnd("SA", ino);
        return r;
    }
    string oldBuf(buf);
    size_t bufsize = buf.size();
    if(size > bufsize)
        buf.resize(size, '\0'); 
    else 
        buf.resize(size);
    logFile << "P " << ino << " " << oldBuf.length() << endl << oldBuf << endl;
    logFile.flush();
    ec->put(ino, buf);
    lc->release(ino);
    logFile << "R " << ino << endl;
    logFile.flush();
    logEnd("SA", ino);
    return r;
}

int
yfs_client::create(inum parent, const char *name, mode_t mode, inum &ino_out)
{
    logStart("CR", parent);
    logFile << "A " << parent << endl;
    logFile.flush();
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
        logFile << "C " << extent_protocol::T_FILE << " " << ino_out << endl;
        logFile.flush();
        string buf;
        ec->get(parent,buf);
        string oldBuf(buf);
        buf.append(string(name) + ':' + filename(ino_out) + '|');
        logFile << "P " << parent << " " << oldBuf.length() << endl << oldBuf << endl;
        logFile.flush();
        ec->put(parent, buf);
    }
    else
        r = EXIST; 
    lc->release(parent);
    logFile << "R " << parent << endl;
    logFile.flush();
    logEnd("CR", parent);
    return r;    
}

int
yfs_client::mkdir(inum parent, const char *name, mode_t mode, inum &ino_out)
{
    logStart("MD", parent);
    logFile << "A " << parent << endl;
    logFile.flush();
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
        logFile << "C " << extent_protocol::T_DIR << " " << ino_out << endl;
        logFile.flush();
        string buf;
        ec->get(parent,buf);
        string oldBuf(buf);
        buf.append(string(name) + ':' + filename(ino_out) + '|');
        logFile << "P " << parent << " " << oldBuf.length() << endl << oldBuf << endl;
        logFile.flush();
        ec->put(parent, buf);
    }
    else
        r = EXIST; 
    lc->release(parent);
    logFile << "R " << parent << endl;
    logFile.flush();
    logEnd("MD", parent);
    return r;
}

int
yfs_client::lookup(inum parent, const char *name, bool &found, inum &ino_out)
{
    logStart("LK", parent);
    logFile << "A " << parent << endl;
    logFile.flush();
    lc->acquire(parent);
    int ret = lookupNoLock(parent, name, found, ino_out);
    lc->release(parent);
    logFile << "R " << parent << endl;
    logFile.flush();
    logEnd("LK", parent);
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
    logStart("RD", dir);
    logFile << "A " << dir << endl;
    logFile.flush();
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
        logFile << "R " << dir << endl;
        logFile.flush();
        logEnd("RD", dir);
        return r;
     }

    string buf;
    if(ec->get(dir, buf) != extent_protocol::OK) {
        r = IOERR;
        lc->release(dir);
        logFile << "R " << dir << endl;
        logFile.flush();
        logEnd("RD", dir);
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
    logFile << "R " << dir << endl;
    logFile.flush();
    logEnd("RD", dir);
    return r;
    
}

int
yfs_client::read(inum ino, size_t size, off_t off, std::string &data)
{
    logStart("R", ino);
    logFile << "A " << ino << endl;
    logFile.flush();
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
        logFile << "R " << ino << endl;
        logFile.flush();
        logEnd("R", ino);
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
    logFile << "R " << ino << endl;
    logFile.flush();
    logEnd("R", ino);
    return r;
}

int
yfs_client::write(inum ino, size_t size, off_t off, const char *data,
        size_t &bytes_written)
{
    logStart("WT", ino);
    logFile << "A " << ino << endl;
    logFile.flush();
    lc->acquire(ino);
    int r = OK;
    /*
     * your lab2 code goes here.
     * note: write using ec->put().
     * when off > length of original file, fill the holes with '\0'.
     */
    string buf, dataBuf(data, size);
    if(ec->get(ino, buf) != extent_protocol::OK) {
        r = IOERR;
        lc->release(ino);
        logFile << "R " << ino << endl;
        logFile.flush();
        logEnd("WT", ino);
        return r;
    }
    string oldBuf = buf;
    if (off + size > buf.size())
        buf += std::string(off + size - buf.size(), 0);
    buf.replace(off, size, dataBuf);
    bytes_written = size;
    logFile << "P " << ino << " " << oldBuf.length() << endl << oldBuf << endl;
    logFile.flush();
    ec->put(ino, buf);
    lc->release(ino);
    logFile << "R " << ino << endl;
    logFile.flush();
    logEnd("WT", ino);
    return r;    
}

int yfs_client::unlink(inum parent,const char *name)
{
    logStart("UL", parent);
    logFile << "A " << parent << endl;
    logFile.flush();
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
        logFile << "R " << parent << endl;
        logFile.flush();
        logEnd("UL", parent);
        return r;
    }

    if(found) {
        std::string buf;
        ec->get(parent, buf);
        std::string filename(""), inode("");
        std::string temp(name);
        string oldBuf2 = buf;
        int pos = buf.find(temp + ":");
        if (pos != std::string::npos) {
            int posA = buf.find(":", pos + 1);
            int posB = buf.find("|", pos + 1);
            inode = buf.substr(posA + 1, posB - posA - 1);
            string oldBuf;
            ec->get(n2i(inode), oldBuf);
            extent_protocol::attr att;
            ec->getattr(n2i(inode), att);
            logFile << "D " << inode << " " << att.type << " " << oldBuf.length() << endl << oldBuf << endl;
            logFile.flush();
            ec->remove(n2i(inode));
            // buf = buf.substr(0, pos) + buf.substr(posB + 1);
            buf.erase(pos, posB + 1 - pos);
        }
        else {
            lc->release(parent);
            logFile << "R " << parent << endl;
            logFile.flush();
            logEnd("UL", parent);
            return ENOENT;
        }
        logFile << "P " << parent << " " << oldBuf2.length() << endl << oldBuf2 << endl;
        logFile.flush();
        ec->put(parent, buf);
    }
    else
        r = NOENT; 
    lc->release(parent);
    logFile << "R " << parent << endl;
    logFile.flush();
    logEnd("UL", parent);
    return r;
}

int yfs_client::symlink(inum parent, const string name, const string link, inum &ino_out)
{
    logStart("SL", parent);
    logFile << "A " << parent << endl;
    logFile.flush();
    lc->acquire(parent);
    int r = OK;

    bool found = false;
    lookupNoLock(parent, name.c_str(), found, ino_out);
    if (found) {
        r = EXIST;
        lc->release(parent);
        logFile << "R " << parent << endl;
        logFile.flush();
        logEnd("SL", parent);
        return r;
    }

    // create a link file
    ec->create(extent_protocol::T_LINK, ino_out);
    logFile << "C " << extent_protocol::T_LINK << " " << ino_out << endl;
    logFile.flush();
    string buf;
    ec->get(parent,buf);
    string oldBuf(buf);
    buf.append(string(name) + ':' + filename(ino_out) + '|');
    logFile << "P " << parent << " " << oldBuf.length() << endl << oldBuf << endl;
    logFile.flush();
    ec->put(parent, buf);

    // write thing to link file
    logFile << "P " << ino_out << " " << link.length() << endl << link << endl;
    logFile.flush();
    ec->put(ino_out, link);
    lc->release(parent);
    logFile << "R " << parent << endl;
    logFile.flush();
    logEnd("SL", parent);
    return r;    
}



int yfs_client::readlink(inum ino, std::string &link)
{
    logStart("RL", ino);
    logFile << "A " << ino << endl;
    logFile.flush();
    lc->acquire(ino);
    int r = OK;
    if(ec->get(ino, link) != extent_protocol::OK) {
        r = IOERR;
    }
    lc->release(ino);
    logFile << "R " << ino << endl;
    logFile.flush();
    logEnd("RL", ino);
    return r;
}
