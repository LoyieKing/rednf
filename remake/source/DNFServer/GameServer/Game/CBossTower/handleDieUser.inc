bool CBossTower::handleDieUser(CUser* user)
{
    if (user == 0)
        return 0;
    if (!CBossPlay::handleDieUser(user))
        return 0;
    if (_checkMemberDie())
    {
        onFinishStage();
        // b28d-54 翻正：ORIG 0x08143ab6 `movzbl 0xafc(%eax),%eax; xor $1,%eax;
        // test %al,%al; je` ⇒ 字段 bool 化（CBossTower.h）+ 裸 `if(!f)`（P248 档②）。
        if (!m_prepareFinishFlag)
            _onPrepareFinishTower(false);
    }
    return 1;
