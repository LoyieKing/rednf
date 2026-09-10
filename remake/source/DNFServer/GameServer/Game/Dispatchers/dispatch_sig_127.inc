int Dispatcher_Compound_ExtreamItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char cVar3;
    bool bVar2;
    int local_1c;                        // -0x1c：ReSeal 合计
    unsigned char rounded;               // -0x22：fistps 结果
    unsigned short cw;                   // -0x1e/-0x20：x87 控制字

    if (user->get_state() <= 2)          // setle：<=2 直接返回
    {
        return 0;
    }
    m_user = user;                       // +0xd0
    cVar3 = _read_packet(packet);
    if ((char)(cVar3 ^ 1) == 0)          // cVar3==1 继续
    {
        cVar3 = (char)((ExtreamDungeon::CCompound_ExtreamDun_Item*)GlobalData::s_pCompoundBindCube)
                    ->checkCompoundable(&m_materials[0], (const CEquipItem**)m_itemRefs, (char)m_targetKind);
        if (cVar3 == 0)
        {
            cVar3 = (char)user->CheckItemLock(1, (unsigned short)m_slots[0]);
            if ((cVar3 == 0) &&
                (cVar3 = (char)user->CheckItemLock(1, (unsigned short)m_slots[1]), cVar3 == 0))
            {
                bVar2 = false;
            }
            else
            {
                bVar2 = true;
            }
            if (bVar2)
            {
                m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0xd5);
            }
            else
            {
                cVar3 = (char)((ServiceRestrictManager*)ARAD::Singleton<ServiceRestrictManager>::Get())
                            ->isRestricted(user, (RestrictCategory::Enum)1, 0xd);
                if (cVar3 == 0)
                {
                    cVar3 = (char)((ExtreamDungeon::CCompound_ExtreamDun_Item*)GlobalData::s_pCompoundBindCube)
                                ->makeCompoundItem((const CEquipItem**)m_itemRefs, (char)m_select, m_resultItem,
                                                   (ExtreamDungeon::eBindSphereType)(m_targetKind == 3 ? 1 : 0));
                    if (cVar3 == 1)
                    {
                        unsigned int uVar6 = (unsigned int)m_materials[0].GetReSealCount();
                        unsigned int uVar7 = (unsigned int)m_materials[1].GetReSealCount();
                        local_1c = (unsigned char)uVar6 + (unsigned char)uVar7;
                        // flds 2.0f; fdivrp; flds 0.5f; faddp; fldcw 截断; fistps（ORIG 0x8bd6348=2.0f, 0x8bd634c=0.5f）
                        float f = ((float)local_1c / 2.0f) + 0.5f;
                        rounded = (unsigned char)(unsigned short)(int)f;
                        m_resultItem.SetReSealCount(rounded);
                        if (m_resultItem.m_addInfo == 0)   // +0x92（m_resultItem 内偏移）
                        {
                            m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0x11);
                        }
                        else
                        {
                            cVar3 = (char)_remove_victim();
                            if (cVar3 == 0)
                            {
                                m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0x11);
                            }
                            else
                            {
                                _push_newitem();
                                _send_result();
                            }
                        }
                    }
                    else
                    {
                        m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0x11);
                    }
                }
                else
                {
                    m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0xd1);
                }
            }
        }
        else
        {
            m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, (unsigned char)(int)(char)(cVar3));
        }
    }
    else
    {
        m_user->SendCmdErrorPacket((ENUM_CMDPACKET)0x146, 0);
    }
    return 0;
