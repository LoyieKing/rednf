// ORIG _ZN13user_creature12CDBMsgSender13AuctionRegistEP10Inven_Itemiiii
// W 0x08340144（nm -S --defined-only 实测；旧注释 0x081faff2 系历史误记）：差异体——
//   +0x48=p1(characNo)；hdr|=0x100（or $0x1,%dh）；+0x53=1；+0x64=5；
//   +0x51(byte)=p3(@0x18)；+0x68=p4(mapSize)；返 1。p2(@0x14) 未被使用。
// [N96 清绑 2026-09-09] 原 `register char* compatEax asm("eax")` 全域禁用（P0②/N96/
// P248 尾条），还原表达式直传 `reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)`。
// 我方 .o 逐条复现 ORIG `lea 0x4(%eax),%edx` setCopy 段。TU 三数零变化
//（identical=9 ae=19 near=0 diff=1），本函数 TU 口径 AE 不变（orig=51 our=51）。
bool CDBMsgSender::AuctionRegist(Inven_Item* item, int characNo, int unused14,
                                 int reason18, int mapSize)
{
    (void)unused14;   // ORIG 未读
    if (m_stream == 0)
        return false;
    SIG_CREATURE_ITEM* buf = GetInBuffer<SIG_CREATURE_ITEM>();
    memset(buf, 0, 0x74);
    reinterpret_cast<Inven_Item_Compatible*>((char*)buf + 4)->setCopy(*item);
    buf->m_addInfo2 = item->m_addInfo2;               // ← item+0x07
    buf->m_reason = (unsigned char)reason18;          // +0x51 ← p3(@0x18)
    buf->m_characNo = characNo;                       // +0x48
    buf->m_hdr |= 0x100;                              // or $0x1,%dh
    buf->m_field53 = 1;                               // +0x53
    buf->m_field64 = 5;
    buf->m_mapSize = mapSize;                         // +0x68
    return true;
}