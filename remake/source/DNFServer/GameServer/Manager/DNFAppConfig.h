#ifndef DNF_APP_CONFIG_H_
#define DNF_APP_CONFIG_H_

#include <string>

#include "DNFTableBase.h"

// ---- CAppConfig ----
class CAppConfig : public CTableBase
{
public:
    // R10: ORIG 无 `new CAppConfig` 的 delete 清理块（throw() 声明 +
    // -fno-enforce-eh-specs 下编译器不生成 landing pad；二进制证据：
    // Init 首处 new 后直接存成员，无 _ZdlPv 路径）。
    CAppConfig() throw();
    virtual ~CAppConfig();
    virtual int Load_Table(const std::string& fileName);
    virtual bool Parse_Table(char* data, int size);
    int Check_FileName(const std::string& fileName);
    int Get_ServerUdpPort();
    int Get_ServerTcpPort();
    unsigned int Get_FrameCountValue();
    unsigned char m_frameCount;  // +4
    char m_pad5[3];
    unsigned int m_serverUdpPort;  // +8（ORIG 无符号：atoi 后隐式转换产生 edx 物化形态）
    unsigned int m_serverTcpPort;  // +0xc
};

#endif  // DNF_APP_CONFIG_H_
