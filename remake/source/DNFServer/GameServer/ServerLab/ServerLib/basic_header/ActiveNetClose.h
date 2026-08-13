#ifndef NSL_ACTIVENETCLOSE_H_
#define NSL_ACTIVENETCLOSE_H_

#include <map>

#include "ThreadLock.h"

namespace nsl {

class TCPUser;

class ActiveNetClose
{
public:
    ActiveNetClose();
    void pushActiveClose(TCPUser* pUser);
    bool onActvieClose(unsigned int& outId, TCPUser*& outUserPtr);
    bool isInData()
    {
        return bInData;
    }

    bool bInData;
    ThreadLock mLock;
    std::map<unsigned int, TCPUser*> activeCloseMap;
};

ActiveNetClose* G_ActiveNetClose();

} // namespace nsl

#endif // NSL_ACTIVENETCLOSE_H_
