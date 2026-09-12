// [b28d-lane2] verdict=AE ndiff=0（ORIG 0x084e4b02）：混合残差；gap0。
// [b28d-solo-InterDispatcher] verdict=AE ndiff=0（本波已归零；AE 归一化后逐条相等）
// [R19a04] ④ 形态修复（§16.1 四元组不配对 ⇒ 复刻 ORIG 出口形态）：
//   此前 `user != 0 && …`（单 && 链）：ORIG 84e4b10 `cmpl $0x0,0xc(%ebp)` +
//   84e4b14 `je 84e4c1d`（直落函数尾 `mov $0,%eax`），
//   我方则 817a9f8 `je 817aa32`（落到与另三条件共享的 bool 物化岛 `mov $0,%eax;test;je`）。
//   求值序两侧一致（user 先 + 失败即短路，不触及后续条件/不解引用）⇒ 语义等价、仅出口形态差；
//   但 O-taken(函数尾) ≢ N-taken(共享岛) ⇒ 按用户「完全复刻 ORIG」裁定改形。
//   试形 A（`if (user == 0) return 0;`）与形 C（goto 尾标签）4.4 均生反向 `jne`+岛（劣化）；
//   形 B（下）令 4.4 生 `je 29a0f <+0x11b>` = ORIG `je 84e4c1d <+0x11b>` 同角色同偏移。
int Inter_PunishCancel::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PunishCancelPktView* pPkt = reinterpret_cast<Inter_PunishCancelPktView*>(data);
    if (user != 0)
    {
        if (pPkt->m_e == 2 &&
            pPkt->m_10 == 0x20 &&
            user->isTradePunishType(0x20))
        {
            user->DelTradePunishType(
                (int)(unsigned short)pPkt->m_10);

            PacketGuard packet;
            packet.put_header(0, 0x13f);
            packet.put_short((unsigned short)pPkt->m_e);
            packet.put_short((unsigned short)pPkt->m_10);
            packet.finalize(true);
            user->Send(packet);
        }
    }
    return 0;
}