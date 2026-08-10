#ifndef DBMW_VERSION_H_
#define DBMW_VERSION_H_

#include "DBMWCommon.h"


class CVersionMgr
{
public:
    CVersionMgr(int a, int b, int c, int d);
    int m_versions[4];  // +0
};

class CSourceVersionMgr
{
public:
    class SourceVersion
    {
    public:
        SourceVersion(char* name, int version);
        SourceVersion(const SourceVersion& other);
        SourceVersion& operator=(const SourceVersion& other);
        ~SourceVersion();
        std::string m_name;  // +0
        int m_version;       // +4
    };
    CSourceVersionMgr();
    ~CSourceVersionMgr();
    void InsertSourceVersion(char* name, int version);
    std::vector<SourceVersion> m_versions;  // +0
};

#endif  // DBMW_VERSION_H_
