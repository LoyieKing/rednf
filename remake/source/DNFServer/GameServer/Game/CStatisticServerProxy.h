#ifndef DNF_GAME_STATISTIC_SERVER_PROXY_H_
#define DNF_GAME_STATISTIC_SERVER_PROXY_H_

#include "BaseServerProxy.h"

class CUser;

class CStatisticServerProxy : public BaseServerProxy
{
public:
    CStatisticServerProxy();                        // ORIG W 0x082a6b72
    CStatisticServerProxy(std::string ip, int port);
    void Init(std::string ip, int tcpPort, int udpPort);  // ORIG _ZN21CStatisticServerProxy4InitESsii（0x082a6bc0 W，CServerProxyMgr 模板调用）
    bool Init();
    void SendPacket(char* packet, int size);
    void SendFileStatistic(const char* file);
    void SendFileStatistic(const char* file, const char* key);
protected:
    virtual bool Parsing(int) { return false; }
};

// statistc_proxy::add（ORIG T 0x0860fdd2 _ZN14statistc_proxy3addEP5CUserPKcS3_jS3_z）：
// 变参统计上报（vsprintf 格式化后经统计代理发送）。定义属统计域批次，
// 此处仅声明供调用方（CParty::get_item 等）编译。
namespace statistc_proxy
{
void add(CUser* user, char const* table, char const* key, unsigned int value,
         char const* fmt, ...);
}

#endif
