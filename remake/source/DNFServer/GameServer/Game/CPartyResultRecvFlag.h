#ifndef DNF_GAME_CPARTY_RESULT_RECV_FLAG_H_
#define DNF_GAME_CPARTY_RESULT_RECV_FLAG_H_

#include <string.h>

class CPartyResultRecvFlag
{
public:
    CPartyResultRecvFlag();
    bool IsValidIdx(int idx) const;
    char IsRecv(int idx) const;
    void SetRecvFlag(int idx);
    void Clear();

private:
    char m_flag[4];
    char m_pad[0x30];
};

#endif
