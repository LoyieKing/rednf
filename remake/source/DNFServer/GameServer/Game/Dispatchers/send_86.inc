// ==== Dispatcher_PcRoomRentItem::send (ORIG 0x081e542e) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e542e orig/ours=229/221 ndiff-blocks=51
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | mov    -0x1c(%ebp),%eax | add    $0x8,%eax
void Dispatcher_PcRoomRentItem::send(CUser* user, ParamBase& param)
{
    stRentItemParam& out = static_cast<stRentItemParam&>(param);
    // [G1Frest 2026-09-10] ORIG 0x081e5436/0x081e543c：`mov 0x10(%ebp),%eax;
    //   mov %eax,-0x1c(%ebp)`（out 引用）+ `mov -0x1c(%ebp),%eax; add $0x8,%eax;
    //   mov %eax,-0x18(%ebp)` ⇒ out.m_output 另占具名引用槽 -0x18，后续各点
    //   `mov -0x18(%ebp),%eax` 重读（旧码每次重算 out+8）。
    InstanceRentalSystem::RentOutput& output = out.m_output;
    bool sendEquipSlot10 = false;

    PacketGuard packet;
    packet.put_header(1, 0x1ee);                         // class=1 cmd=0x1ee
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        // ORIG 0x081e549e `movsbl %al,%eax` ⇒ size() 经 (char) 收窄
        packet.put_byte(static_cast<int>(static_cast<char>(output.m_deleted.size())));
        for (std::vector<InstanceRentalSystem::DeletedRentItem>::iterator it =
                 output.m_deleted.begin();
             it != output.m_deleted.end(); ++it)
        {
            // [b28d-solo-Dispatchers] P167/P183：ORIG 0x081e54d3 只调 1 次
            //   `normal_iterator<DeletedRentItem>::deEv`（operator*），结果落 -0x10
            //   槽后各字段点重读（0x081e54f8/0x081e5505/0x081e552b 均
            //   `mov -0x10(%ebp),%eax; movzwl 0x2(%eax)`）；旧码 `it->` 调 ptEv ×2
            //   且 slot 具名局部缓存。⇒ 改 `r = *it` 引用 + 各点 r.m_slot 重读。
            InstanceRentalSystem::DeletedRentItem& r = *it;
            // [G6-send86] ORIG 0x081e54e3 `je <+0xea>`（m_bEquip==0 跳 else 块）
            //   ⇒ 源序须 then=非零臂（put_byte(3)+范围检查）、else=零臂（put_byte(0)）。
            if (r.m_bEquip != 0)                                      // +0x00
            {
                packet.put_byte(3);
                // ORIG 0x081e55xx `jle/jg`（有符号）：m_slot 为 unsigned short 字段，
                // 使用点须 (short) 收窄（P11）。
                if ((int)(short)r.m_slot > 9 && (int)(short)r.m_slot < 0x16)
                {
                    sendEquipSlot10 = true;                           // 装备位 9..0x15 变动
                }
            }
            else
            {
                packet.put_byte(0);
            }
            packet.put_short(static_cast<int>(static_cast<short>(r.m_slot)));
        }
    }
    else
    {
        packet.put_byte(0);
        // [G6-send86] ORIG 0x081e559a `movzbl %al,%eax` ⇒ (unsigned char) 截断
        //   （旧译 `& 0xff` 产 `and $0xff,%eax` 掩码形）。
        packet.put_byte(static_cast<int>(static_cast<unsigned char>(out.m_4)));
    }
    packet.finalize(true);
    user->Send(packet);

    if (out.m_4 == 0)
    {
        PacketGuard infoPacket;
        infoPacket.put_header(0, 0xe);                   // class=0 cmd=0xe 租借列表刷新
        infoPacket.put_byte(0);
        // ORIG `call size(); cwtl` ⇒ (short) 收窄
        infoPacket.put_short(static_cast<int>(static_cast<short>(output.m_infos.size())));
        // [b28d-solo-Dispatchers] P167：ORIG 0x081e5662 调 const_iterator 的 deEv
        //   （`IPKN…`）+ 0x081e56c7 混合 ne + 0x081e563x 转换构造 ⇒ 原码是
        //   const_iterator + `(*it).m_slot`（旧码 iterator + `it->` 调 ptEv）。
        for (std::vector<InstanceRentalSystem::RentalInfo>::const_iterator it =
                 output.m_infos.begin();
             it != output.m_infos.end(); ++it)
        {
            // [G6·P52] ORIG 0x081e565c 段：`call deEv`→`mov %eax,-0xc`(引用槽)→`movzbl 0x2(%eax);
            //   movswl %ax,%ebx`(slot 值升格 callee-saved 跨 receiver call)→receiver **内联**
            //   （`mov 0xc(%ebp); call getCurCharacInvenR` 结果 %eax 直传 this，无 inven 槽）。
            //   我方 inven 具名局部分裂语句 ⇒ receiver 提前求值 + cwtl 不升格。
            const InstanceRentalSystem::RentalInfo& r = *it;
            const_cast<CInventory*>(user->getCurCharacInvenR())
                ->MakeItemPacket(INVEN_TYPE_EQUIP,
                                 static_cast<int>(static_cast<short>(r.m_slot)),
                                 infoPacket);
        }
        infoPacket.finalize(true);
        user->Send(infoPacket);
    }

    if (sendEquipSlot10)
    {
        user->send_equip(10);
    }
}