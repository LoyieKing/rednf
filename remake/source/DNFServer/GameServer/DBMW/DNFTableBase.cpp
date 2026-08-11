// df_dbmw_r - DNFTableBase (ORIG DNFTableBase.cpp)
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

int getErrno();

// CTableBase / CAppInit / CAppConfig / CServerConfig
CTableBase::CTableBase() {}
CTableBase::~CTableBase() {}
int CTableBase::Load_Txt_Table_Data(const char* fileName, int idx)
{
    int count = 0;
    FILE* f;
    if ((f = fopen(fileName, "rb")) == NULL)
        return -1;
    char buf[0x400];
    while (!feof(f) && fgets(buf, 0x400, f))
    {
        if (buf[0] == '#')
            continue;
        if (count >= idx)
            return -2;
        if (Parse_Table(buf, count))
            count++;
    }
    fclose(f);
    return count;
}
CDNFException::CDNFException(const std::string& msg) : m_msg(msg) {}
CDNFException::~CDNFException() throw() {}
const char* CDNFException::what() const throw()
{
    DNF_LOG_SCOPE_LINE(0x1a, "./log/Except", "%s", m_msg.c_str());
    return m_msg.c_str();
}
