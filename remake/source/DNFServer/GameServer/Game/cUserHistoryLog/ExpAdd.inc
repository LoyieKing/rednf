void cUserHistoryLog::ExpAdd(int exp, int sp, eExpAddReason reason, int reasonIdx)
{
    // [b28d-solo-cUserHistoryLog] N6 翻案：`(int)` cast 阻断 4.4 前端的 bool 位测试折叠
    // （探针 /tmp/hlprobe/Game/probe4.cpp k1：`(int)((1u<<r)&0x1892u)==0` 产 ORIG 的
    // shl;and $0x1892;test，无 cast 恒折 shr;and $1）。
    // 证据（ORIG 0x08684bd7..0x08684c27）：`mov 0x14(%ebp),%eax; cmp $0xc,%eax; ja body;
    // shl/and/test; jne <nop 岛>; body: ... call; jmp +0x52; nop`。P223 尾空标签复现
    // （探针 probe4.cpp e3 与 ORIG 同形，仅 reason 重载一条差）。
    if (!((0xc < (unsigned)reason) ||
          (((int)((1u << reason) & 0x1892U)) == 0)))
        goto end;
    m_user->LogHistory("Exp+,%d,%d,%d", exp, sp, reason);
end:;
