void WarRoom::CheckFatuigue()
{
    for (int i = 0; i <= 5; ++i)
    {
        // ORIG 086bc4ff：je1→086bc5e4(nop;fall end)空岛；body1 尾/body2 尾/je2
        // 均直落 end(086bc5e5)，body2 尾显式 jmp 跳岛(086bc5e2)。
        if (m_slots[i] == 0)
            goto CF_ISLAND;
        if (!(m_slotsUser[i])->CheckFatigue())
        {
            CUser* u = m_slotsUser[i];
            WalkOutUserBySlot(i);
            SendWalkOut(u, (ENUM_WALKOUT_TYPE)5);
            goto CF_END;
        }
        else if ((m_slotsUser[i])->getCurCharacMoney() <
                 G_CDataManager()->m_warAreaCounter.m_doorFee[(unsigned char)m_levelBand])
        {
            CUser* u = m_slotsUser[i];
            WalkOutUserBySlot(i);
            SendWalkOut(u, (ENUM_WALKOUT_TYPE)8);
            goto CF_END;
        }
CF_END:
        ;
CF_ISLAND:
        ;
    }
