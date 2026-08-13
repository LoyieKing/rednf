#ifndef NSL_TMSGCELL_H_
#define NSL_TMSGCELL_H_

#include <string.h>

#include "MsgCell.h"

namespace nsl {

template <int N>
class TMsgCell : public CMsgCell
{
public:
    TMsgCell()
    {
        m_nBufLen = N;
        memset(buf, 0, N);
        m_bBuf = buf;
    }
    ~TMsgCell()
    {
    }

private:
    char buf[N];
};

} // namespace nsl

#endif // NSL_TMSGCELL_H_
