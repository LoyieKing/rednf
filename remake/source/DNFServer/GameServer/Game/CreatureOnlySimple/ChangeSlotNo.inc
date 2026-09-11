// 【objdump】ORIG _ZN13user_creature12CDBMsgSender12ChangeSlotNoEP10Inven_Itemiii
// W 0x083400a4（nm -S --defined-only 实测；旧注释 0x081faf4c 系历史误记）：差异体——
//   +0x51(byte)=p3(reason)；+0x48=p1(characNo)；hdr|=0x20；+0x2c=p2(slot)；
//   +0x64=2；返 1（无 +0x68 区写入）。
// [N96 清绑 2026-09-09] 原 `register char* compatEax asm("eax")` 全域禁用（P0②/N96/
// P248 尾条），还原表达式直传 `reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)`。
// 我方 .o 逐条复现 ORIG `lea 0x4(%eax),%edx` setCopy 段。TU 三数零变化
//（identical=9 ae=19 near=0 diff=1），本函数 TU 口径 AE 不变（orig=49 our=49）。
bool CDBMsgSender::ChangeSlotNo(Inven_Item* item, int characNo, int slot,
                                int reason)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x20;                               // or $0x20
    buf->m_slot = slot;                               // +0x2c
    buf->m_field64 = 2;
    return true;
}