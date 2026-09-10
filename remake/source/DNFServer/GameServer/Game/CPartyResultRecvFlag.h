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
    // [b28d-solo-CParty2] 尾部 `char m_pad[0x30]` 是错误建模，已删：
    // ORIG Clear 0x082a4cc6 = `movl $0x4,0x8(%esp); movl $0,0x4(%esp); call memset`
    // ⇒ **sizeof(CPartyResultRecvFlag)==4**（CParty 内只占 +0x1dc..+0x1df，
    // 其后 +0x1e0 起为 CParty 直属字段）。
};

#endif
