#ifndef DNF_GAME_CPARTY_RESULT_RECV_FLAG_H_
#define DNF_GAME_CPARTY_RESULT_RECV_FLAG_H_

#include <string.h>

class CPartyResultRecvFlag
{
public:
    CPartyResultRecvFlag();
    bool IsValidIdx(int idx) const;
    unsigned char IsRecv(int idx) const;  // P9/P11：ORIG 0x085bf406 尾 movzbl 直返无归一
    void SetRecvFlag(int idx);
    void Clear();

private:
    char m_flag[4];
    char m_pad[0x30];
};

#endif
