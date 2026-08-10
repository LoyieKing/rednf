#ifndef DNF_APP_CONFIG_H_
#define DNF_APP_CONFIG_H_

#include <string>

#include "DNFTableBase.h"

// CAppConfig：CTableBase@0 / frameCount@4 / udpPorts[101]@8
class CAppConfig : public CTableBase
{
public:
    CAppConfig();
    virtual ~CAppConfig();
    virtual void Load_Table(const std::string& filename);
    virtual bool Parse_Table(char* line, int idx);
    unsigned short Get_FrameCountValue();
    unsigned int Get_ServerUdpPort(unsigned char idx);
    void Check_FileName(const std::string& filename);

    unsigned char m_frameCount;    // +4
    unsigned int m_udpPorts[101];  // +8
};

#endif // DNF_APP_CONFIG_H_
