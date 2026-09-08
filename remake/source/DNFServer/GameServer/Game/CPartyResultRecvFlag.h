#ifndef DNF_GAME_CPARTY_RESULT_RECV_FLAG_H_
#define DNF_GAME_CPARTY_RESULT_RECV_FLAG_H_

#include <string.h>

class CPartyResultRecvFlag
{
public:
    CPartyResultRecvFlag();
    bool IsValidIdx(int idx) const;
    // b28d-45：ORIG 调用点 0x085b3fc0 为 `call; xor $0x1,%eax; test %al,%al; jne`
    // （P248 档①：bool callee 裸形），改 bool。体尾 movzbl 直返不变，mangled 不变。
    bool IsRecv(int idx) const;
    void SetRecvFlag(int idx);
    void Clear();

private:
    char m_flag[4];
    char m_pad[0x30];
};

#endif
