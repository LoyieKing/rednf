#ifndef DBMW_DNFAPPCONFIG_H_
#define DBMW_DNFAPPCONFIG_H_

#include "DBMWCommon.h"


class CAppConfig : public CTableBase
{
public:
    CAppConfig();
    virtual ~CAppConfig();
    virtual int Load_Table(const std::string& fileName);
    virtual bool Parse_Table(char* data, int size);
    int Check_FileName(const std::string& fileName);
    int Get_ServerUdpPort();
    int Get_ServerTcpPort();
    unsigned short Get_FrameCountValue();
    unsigned char Get_DbmwType();
    STDBConnInfo* GetDBConnInfo(ENUM_DB_HANDLE_IDX idx);
    int GetServerGroup();
    bool DecryptValue(const char* value, char* dst);
    CTEA m_cipher;          // +4（0x48 字节）
    char m_tickValue;       // +0x4c
    char m_pad4d[3];
    int m_udpPort;          // +0x50
    int m_tcpPort;          // +0x54
    char m_dbmwType;        // +0x58
    char m_pad59[3];
    int m_serverGroup;      // +0x5c
    STDBConnInfo m_dbConnInfo[0x11];  // +0x60（每项 0x168）
};

#endif  // DBMW_DNFAPPCONFIG_H_
