#ifndef DNF_GAME_STATISTIC_SERVER_PROXY_H_
#define DNF_GAME_STATISTIC_SERVER_PROXY_H_

#include "BaseServerProxy.h"

class CStatisticServerProxy : public BaseServerProxy
{
public:
    CStatisticServerProxy();                        // ORIG W 0x082a6b72
    CStatisticServerProxy(std::string ip, int port);
    bool Init();
    void SendPacket(char* packet, int size);
    void SendFileStatistic(const char* file);
    void SendFileStatistic(const char* file, const char* key);
protected:
    virtual bool Parsing(int) { return false; }
};

#endif
