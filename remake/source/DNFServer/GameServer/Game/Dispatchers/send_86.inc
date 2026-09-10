void Dispatcher_PcRoomRentItem::send(CUser* user, ParamBase& param)
{
    stRentItemParam& out = static_cast<stRentItemParam&>(param);
    bool sendEquipSlot10 = false;

    PacketGuard packet;
    packet.put_header(1, 0x1ee);                         // class=1 cmd=0x1ee
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_byte(static_cast<int>(out.m_output.m_deleted.size()));
        for (std::vector<InstanceRentalSystem::DeletedRentItem>::iterator it =
                 out.m_output.m_deleted.begin();
             it != out.m_output.m_deleted.end(); ++it)
        {
            // [b28d-solo-Dispatchers] P167/P183：ORIG 0x081e54d3 只调 1 次
            //   `normal_iterator<DeletedRentItem>::deEv`（operator*），结果落 -0x10
            //   槽后各字段点重读（0x081e54f8/0x081e5505/0x081e552b 均
            //   `mov -0x10(%ebp),%eax; movzwl 0x2(%eax)`）；旧码 `it->` 调 ptEv ×2
            //   且 slot 具名局部缓存。⇒ 改 `r = *it` 引用 + 各点 r.m_slot 重读。
            InstanceRentalSystem::DeletedRentItem& r = *it;
            if (r.m_bEquip == 0)                                      // +0x00
            {
                packet.put_byte(0);
            }
            else
            {
                packet.put_byte(3);
                if (r.m_slot > 9 && r.m_slot < 0x16)
                {
                    sendEquipSlot10 = true;                           // 装备位 9..0x15 变动
                }
            }
            packet.put_short(static_cast<int>(r.m_slot));
        }
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4 & 0xff);
    }
    packet.finalize(true);
    user->Send(packet);

    if (out.m_4 == 0)
    {
        PacketGuard infoPacket;
        infoPacket.put_header(0, 0xe);                   // class=0 cmd=0xe 租借列表刷新
        infoPacket.put_byte(0);
        infoPacket.put_short(static_cast<int>(out.m_output.m_infos.size()));
        // [b28d-solo-Dispatchers] P167：ORIG 0x081e5662 调 const_iterator 的 deEv
        //   （`IPKN…`）+ 0x081e56c7 混合 ne + 0x081e563x 转换构造 ⇒ 原码是
        //   const_iterator + `(*it).m_slot`（旧码 iterator + `it->` 调 ptEv）。
        for (std::vector<InstanceRentalSystem::RentalInfo>::const_iterator it =
                 out.m_output.m_infos.begin();
             it != out.m_output.m_infos.end(); ++it)
        {
            const CInventory* inven = const_cast<CInventory*>(user->getCurCharacInvenR());
            inven->MakeItemPacket(INVEN_TYPE_EQUIP,
                                  static_cast<int>(static_cast<short>((*it).m_slot)),
                                  infoPacket);
        }
        infoPacket.finalize(true);
        user->Send(infoPacket);
    }

    if (sendEquipSlot10)
    {
        user->send_equip(10);
    }
