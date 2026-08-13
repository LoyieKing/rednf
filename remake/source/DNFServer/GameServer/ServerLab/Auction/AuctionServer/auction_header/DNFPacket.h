#ifndef NSL_DNFPACKET_H_
#define NSL_DNFPACKET_H_

#include <string.h>

#include "PacketDesign.h"

namespace nsl {

struct PCK_CS_POSTING_LOG : public PACKET_HEADER
{
    PCK_CS_POSTING_LOG()
    {
        PACKET_CTOR_BODY(1, 0, 0x12);
    }

    void _Clear()
    {
        memset(this, 0, 0x12);
    }

    char* _GetLastAddress()
    {
        return (char*)this + getSize();
    }

    char* _getOuterData()
    {
        return (char*)this + sizeof(PACKET_HEADER);
    }

    int _getOuterDataSize()
    {
        return getSize() - sizeof(PACKET_HEADER);
    }
};

} // namespace nsl

#endif // NSL_DNFPACKET_H_
