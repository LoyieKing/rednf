bool CEventCharacterHandler::_makeEventCharacter(CUser* user, int level)
{
    // [b28d-solo-CEventCharacterHandler] ORIG 0x0848e2a4 用 edi(cmdIdx)/esi(name)/
    // ebx(accStr) 缓存 + 帧 0x6c；name/accStr 无槽（P0）⇒ P227 全表达式临时链
    // 令三值跨 cMyTrace ctor 保活（_AddItem 同法）。
    int cmdIdx = -1;
    if (!_checkMakeCommand(user, level, cmdIdx))
    {
        return false;
    }

    cMyTrace(__PRETTY_FUNCTION__, 0x2b, 0)(
        "%s:'%s' Make Event Character(%d) level(%d).",
        NumberToString(user->get_acc_id(), 0), user->getCurCharacName(), cmdIdx, level);

    STEventCharacterInfo info;
    if (!G_CDataManager()->m_eventCharacScript.GetEventCharacterInfo(cmdIdx, level, info))
    {
        return false;
    }

    if (level > 0xe)
    {
        _SetLevel(user, 0xf);
        if (info.m_growType != 0)
        {
            _SetGrowType(user, 1, info.m_growType);
        }
        if (level > 0xf)
        {
            _SetLevel(user, level);
            if (level > 0x31 && info.m_secondGrowType != 0)
            {
                _SetGrowType(user, 2, info.m_secondGrowType);
            }
        }
    }
    else
    {
        _SetLevel(user, level);
    }

    user->AddDungeonClear(2, 1);
    user->AddDungeonClear(0x1a, 1);
    _SetClearedQuest(user);

    std::vector<std::pair<int, int> >::iterator skillIt = info.m_skillList.begin();
    for (; skillIt != info.m_skillList.end(); ++skillIt)
    {
        // P167：ORIG 发 __normal_iterator::operator*（deEv）而非 operator->（ptEv）。
        _MasterNewSkill(user, (*skillIt).first, (*skillIt).second);
    }

    std::vector<STEventCharacterItem>::iterator itemIt = info.m_itemList.begin();
    for (; itemIt != info.m_itemList.end(); ++itemIt)
    {
        _AddItem(user, (*itemIt).m_itemIdx, (*itemIt).m_count, (*itemIt).m_reason);
    }

    _UpdateEventCharacInfo(user);
    user->SendNotiPacket(CUser::eSendTarget_1, (ENUM_NOTIPACKET)0x25, 0);
    return true;
}