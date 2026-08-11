// df_dbmw_r - version (ORIG version.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "DNFMySql.h"

int getErrno();

// ORIG 的 TU 边界：这些基类构造在独立 TU 定义（派生类 TU 只见声明，
// 触发 may-throw 预留栈帧 sub $0x18，与 ORIG 对齐，2026-08-11）。
CDBHandle::CDBHandle() {}
CDBHandle::~CDBHandle() {}
CAppInit::CAppInit() {}
CAppInit::~CAppInit() {}

CSourceVersionMgr::SourceVersion::SourceVersion(const SourceVersion& other)
    : m_name(other.m_name), m_version(other.m_version)
{
}
CSourceVersionMgr::SourceVersion& CSourceVersionMgr::SourceVersion::operator=(const SourceVersion& other)
{
    m_name = other.m_name;
    m_version = other.m_version;
    return *this;
}
CSourceVersionMgr::SourceVersion::~SourceVersion() {}
CSourceVersionMgr::CSourceVersionMgr()
{
    InsertSourceVersion(".svn/all-wcprops", 0x19daa);
    InsertSourceVersion("../ServerCommon/.svn/all-wcprops", 0x19daa);
}
CSourceVersionMgr::~CSourceVersionMgr() {}
CVersionMgr::CVersionMgr(int a, int b, int c, int d)
{
    m_versions[0] = a;
    m_versions[1] = b;
    m_versions[2] = c;
    m_versions[3] = d;
}
void CSourceVersionMgr::InsertSourceVersion(char* name, int version)
{
    m_versions.push_back(SourceVersion(name, version));
}
CSourceVersionMgr::SourceVersion::SourceVersion(char* name, int version)
{
    m_name = name;
    m_version = version;
}
