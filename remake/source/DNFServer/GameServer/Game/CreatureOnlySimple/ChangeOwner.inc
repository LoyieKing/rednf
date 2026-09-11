// 【objdump】ORIG _ZN13user_creature12CDBMsgSender11ChangeOwnerEP10Inven_Itemiiiiii
// W 0x0833ffe8（nm -S --defined-only 实测；旧注释 0x081fae8a 系历史误记）：差异体——
//   +0x51(byte)=p3(reason)；+0x48=p1(characNo)；hdr|=0x30；+0x2c=p2(slot)；
//   +0x64=1；+0x68=p4(mapSize)；+0x6c=p5；+0x70=p6；返 1。
// [N96 清绑 2026-09-09] 原 `register char* compatEax asm("eax")` 全域禁用（P0②/N96/
// P248 尾条），还原表达式直传 `reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)`。
// 我方 .o 逐条复现 ORIG：`mov -0xc(%ebp),%eax; lea 0x4(%eax),%edx; mov 0xc(%ebp),%eax;
// mov %eax,0x4(%esp); mov %edx,(%esp); call setCopy`。TU 三数零变化
//（identical=9 ae=19 near=0 diff=1），本函数 TU 口径 AE 不变（orig=58 our=59）。
bool CDBMsgSender::ChangeOwner(Inven_Item* item, int characNo, int slot,
                               int reason, int mapSize, int field6c, int field70)
{
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason;            // +0x51
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x30;                               // or $0x30
    buf->m_slot = slot;                               // +0x2c
    buf->m_field64 = 1;
    buf->m_mapSize = mapSize;                         // +0x68
    buf->m_field6c = field6c;                         // +0x6c
    buf->m_field70 = field70;                         // +0x70
    return true;
}