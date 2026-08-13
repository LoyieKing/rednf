#ifndef NSL_PORTINFO_H_
#define NSL_PORTINFO_H_

#include <stdio.h>

namespace nsl {

class PortInfo
{
public:
    PortInfo()
    {
        puts("Start PortInfo~");
        mTCPPort = 0;
        mUDPPort = 0;
    }
    void setTCPPort(int port)
    {
        mTCPPort = port;
    }
    void setUDPPort(int port)
    {
        mUDPPort = port;
    }
    int getTCPPort() const
    {
        return mTCPPort;
    }
    int getUDPPort()
    {
        return mUDPPort;
    }

    int mTCPPort;
    int mUDPPort;
};

} // namespace nsl

#endif // NSL_PORTINFO_H_
