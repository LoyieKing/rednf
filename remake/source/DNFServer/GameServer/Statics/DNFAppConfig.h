#ifndef DNF_APP_CONFIG_H_
#define DNF_APP_CONFIG_H_

#include <string>

#include "DNFTableBase.h"

// CAppConfig：vptr@0 / frameCount@4 / serverUdpPort@8 / serverGroup@0xc（0x10）
class CAppConfig : public CTableBase
{
public:
    CAppConfig();
    virtual ~CAppConfig();
    virtual void Load_Table(const std::string& filename);
    virtual bool Parse_Table(char* line, int idx);
    unsigned char Get_FrameCountValue();
    unsigned int Get_ServerUdpPort();
    unsigned char Get_ServerGroup();
    void Check_FileName(const std::string& filename);
    char m_frameCount;       // +4
    unsigned int m_udpPort;  // +8
    char m_serverGroup;      // +0xc
};

#endif // DNF_APP_CONFIG_H_
