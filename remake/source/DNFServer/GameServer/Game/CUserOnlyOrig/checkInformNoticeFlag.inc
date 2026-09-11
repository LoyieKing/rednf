// ---------------------------------------------------------------------------
// 40) CUser::checkInformNoticeFlag(uint bitIdx, uint byteIdx)
//     报告：docs/class_func_reports/CUser/checkInformNoticeFlag.md。ORIG 0x0868f9e8。
//     位表 @+0x8ebb1（uchar[16]，bit 由低到高）。返回 1 = 该位未置。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig AE] ORIG 0x0868f9e8：调用点/形态已对齐（本波修法见函数内注释）。
bool CUser::checkInformNoticeFlag(unsigned int bitIdx, unsigned int byteIdx)
{
    if (bitIdx > 7) return false;
    if (byteIdx > 15) return false;
    // ORIG 0x0868f9e8：表值经 1B 槽 `mov %al,-0x5(%ebp); movzbl -0x5(%ebp),%edx`（具名 uchar 变量）
    // 尾 `test %al,%al; je; mov $1; jmp; mov $0` bool 物化岛（bool 值无槽：绑 %eax 直落）
    const unsigned char byte = m_informNoticeFlags[byteIdx];
    // [b28d-solo-CUserOnlyOrig P226/P0] ORIG 0x0868fa2x 尾 `xor $0x1,%eax; test %al,%al;
    // je` 值在 %al 无栈槽 ⇒ 原码无具名 bool（旧 `bool off; off=...;` 生
    // `mov %al,-0x5; cmpb $0,-0x5` 且把 byte 槽挤到 -0x6）。
    if (!((byte >> bitIdx) & 1))
        return true;
    return false;
}