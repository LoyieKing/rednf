void Secu_AccountHacking::sendAvatar(char const* name, unsigned int a)
{
    // ORIG T 0x08278f16（sendAvatar.md）：同名直返；fatigue<=100 时自增计数，
    // 检测过金币伪型→AVATAR_COUNT；计数>=8 且距登录<600 秒→AVATAR_TIME。
    if (isSameAccountCharacter(name))
        return;

    // 条件物化形（ORIG @0x08278f36..0x08278f62 / 0x08278faf..0x08278fe9）：
    // 正条件 if 包裹（非 if(!x)return）令 4.4 产 mov$1/mov$0 汇聚 + test/je
    // 失败岛（探针 /tmp/l12_probe.cpp f3 实证）；getCurSec 内联无具名局部（P0）。
    if ((m_userP != 0) &&
        ((unsigned int)((CUser*)m_userP)->getCurCharacUsedFatigue() <= 100))
    {
        m_sendCnt = m_sendCnt + 1;

        if (isDetectionGoldPatten())
            setPatten(AccountHackingType::AVATAR_COUNT, name, a, 0);

        if ((m_sendCnt >= 8) &&
            ((unsigned int)(GlobalData::s_systemTime_.getCurSec() - m_loginTime) <=
             599))
        {
            setPatten(AccountHackingType::AVATAR_TIME, name, a, 0);
        }
        // b28c-lane12：ORIG 尾有三个独立 `nop; jmp <leave>` 中继岛（+0x100/
        // +0x103/+0x106），4.4 对共享出口合并；显式出口 return 使各出口独立
        // 发射（P63 散布 return 中继块）。
        return;
    }
    return;
