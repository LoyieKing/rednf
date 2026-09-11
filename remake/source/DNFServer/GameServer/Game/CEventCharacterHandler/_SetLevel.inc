bool CEventCharacterHandler::_SetLevel(CUser* user, int level)
{
    if (level > 0x46)
    {
        return false;
    }

    bool bChanged = true;
    int diff = level - user->get_charac_level();
    // [b28d-solo-CEventCharacterHandler] P212 全头部声明复现 ORIG 槽位（b28d S107）：
    // 4.4 对函数头声明按「先声明浅」分配（/tmp/s107/p5 探针实证），ORIG 槽对为
    // newLevelUp -0x28 / maxLevelUp -0x24 / newLevelDown -0x20 / maxLevelDown -0x1c
    // ⇒ 头部声明序取浅→深 maxLevelDown/newLevelDown/maxLevelUp/newLevelUp，
    // 块内仅赋值（store 时机 newLevel 先、常量后与 ORIG 同）。旧 N98 封顶判词作废。
    int maxLevelDown;
    int newLevelDown;
    int maxLevelUp;
    int newLevelUp;
    if (diff == 0)
    {
        bChanged = false;
    }
    else if (diff > 0)
    {
        for (int i = 0; i < diff; ++i)
        {
            newLevelUp = user->get_charac_level() + 1;
            maxLevelUp = 0x46;
            user->SetCharacLevel(std::min(maxLevelUp, newLevelUp));
        }
    }
    else
    {
        for (int i = 0; i > diff; --i)
        {
            newLevelDown = user->get_charac_level() - 1;
            maxLevelDown = 0x46;
            user->SetCharacLevel(std::min(maxLevelDown, newLevelDown));
        }
    }
    return bChanged;
}