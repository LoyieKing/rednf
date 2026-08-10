#ifndef DNF_APP_CONFIG_H_
#define DNF_APP_CONFIG_H_

#include <string>

#include "DNFTableBase.h"

// ---- CAppConfig ----
class CAppConfig : public CTableBase
{
public:
    CAppConfig();
    virtual ~CAppConfig();
    virtual int Load_Table(const std::string& fileName);
    virtual int Parse_Table(char* data, int size);
    int Check_FileName(const std::string& fileName);
    int Get_ServerUdpPort();
    int Get_ServerTcpPort();
    unsigned int Get_FrameCountValue();
    unsigned char m_frameCount;  // +4
    char m_pad5[3];
    int m_serverUdpPort;    // +8
    int m_serverTcpPort;    // +0xc
};

#endif  // DNF_APP_CONFIG_H_
