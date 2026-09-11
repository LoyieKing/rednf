// b28d-solo-CParty: N 封顶 —— N82 帧 0x5c/0x60 + 块序极性；270 vs 263
bool CParty::UseAncientDungeonItems(CDungeon const* dungeon, Inven_Item* items, int* param)
{
    // b28d-49 判词：N 封顶（N82 帧/槽）。P167 CLEAR。@0x859eac2
    // [P16/P133] ORIG 0x0859eadx：`cmp $0x1,%eax; jne <body>; mov $1,%eax; jmp <end>`
    // = 第一条守卫独立早退（非 && 合并）；第二条守卫落尾共享 `return 1`。
    if (m_battleField.m_bloodState == 1)
        return 1;
    if (dungeon->m_entryRequiredItem.m_enableFlag != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (_checkValidUser(i) != 1)
                continue;
            // [2026-09-10 + 本轮 P0/P183] ORIG 0x0859eb4x 实参装载段：
            //   `mov 0xc(%ebp),%eax; mov 0x7f8(%eax),%esi`（itemCount→%esi）
            //   `mov -0x20(%ebp),%eax; shl $2; add 0x14(%ebp),%eax; mov (%eax),%ebx`
            //   （slot = param[i] → %ebx）——**两者均无栈槽**，跨 receiver call 保活
            //   ⇒ 删除 itemIdx/slot 具名局部，实参写字面表达式。
            if ((char)m_member[i].m_pUser->getCurCharacInvenW()->delete_item(
                    (INVEN_TYPE)1, param[i],
                    dungeon->m_entryRequiredItem.m_itemCount,
                    (eItemDelReason)0xe, true) != 1)
            {
                // [P227 全表达式临时链] ORIG 0x0859eb84..0x0859ebf6：ctor 前右起求 varargs
                //   并跨 call 保活（dungeon+0x7f8→%edi、param[i]→%esi、characNo→%ebx），
                //   ctor 后才压栈 ⇒ 旧码三个具名局部 + 具名 trace 令 4.4 在 ctor 后求值，
                //   少 push %edi（帧 0x5c vs 0x60）。
                cMyTrace(__PRETTY_FUNCTION__, 0xa60, 5)(
                    "CParty::UseAncientDungeonItems, delete_item failed , User ch=%d , %d %d",
                    m_member[i].m_pUser->getCurCharacNo(),
                    param[i], dungeon->m_entryRequiredItem.m_itemCount);
                return 0;
            }
            PacketGuard packet;
            packet.put_header(0, 0xe);
            packet.put_byte(0);
            packet.put_short(1);
            packet.put_short(param[i]);
            int diff = items[i].m_addInfo2 - dungeon->m_entryRequiredItem.m_itemCount;
            if (diff == 0)
            {
                packet.put_int(-1);
            }
            else
            {
                packet.put_int(items[i].m_addInfo);
            }
            packet.put_int(diff);
            packet.put_byte(items[i].GetItemAttr());
            packet.put_short(items[i].m_fieldb);
            packet.put_byte(items[i].m_field0);
            packet.put_int(items[i].m_fieldd);
            packet.put_byte(items[i].m_amp.getAbilityType());
            packet.put_short(items[i].m_amp.getAbilityValue());
// b28d-solo-CParty: DIFF 残 —— P209/N82：4.4 对 enum 形参比较恒 mem 直比（ORIG mov;test 复用寄存器）+ 自增段多载 slot；123 vs 121
            packet.put_byte(0);
            packet.put_packet(items[i]);
            packet.finalize(true);
            m_member[i].m_pUser->Send(packet);
            m_member[i].m_pUser->SaveInventory();
        }
    }
    return 1;
}