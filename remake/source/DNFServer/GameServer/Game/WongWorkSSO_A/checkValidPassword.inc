// ---- checkValidPassword(...)  ORIG 0x086028c6：两相邻 unsigned 局部作 long& 传入；
// ORIG 返回 bool（= _getGameStartTime 结果，GetAccountInfo 失败时记日志返回 false）。
bool WongWork::CSimpleSSO::checkValidPassword(unsigned int mId, char const* a,
                                              char const* b, stOutputData_t& out)
{
    // [b28d-solo-WongWorkSSO_A] ORIG 0x086028c6：帧 0xf8 由 dead char[0xc0] 复现
    // （P31/P232 未读写局部仍占帧）；两个 long 局部落 -0x14/-0x18 需两个未使用
    // int 先占 -0xc/-0x10（P232 死局部占槽）。实参必须为真 long 左值直传——
    // unsigned+reinterpret_cast 引用形触发 4.4 提前 lea(%edx/%eax)+ecx 中转（P0 反例）。
    char framePad[0xc0];
    (void)framePad;
    int slotPad1;
    (void)slotPad1;
    int slotPad2;
    (void)slotPad2;
    long v2 = 0;  // loginTime&  → ORIG -0x14(%ebp)（movl $0 在前）
    long v1 = 0;  // expireTime& → ORIG -0x18(%ebp)
    return _getGameStartTime(mId, v2, v1, out);
}