int Dispatcher_SecurityCardAuthCancel::process(CUser* user)
{
    // [b23S3C] P48：ORIG getSecurityCard ×5 / getPad ×5 每用点独立重取
    // （p167 MISSING 各 -4 即此），card/pad 无具名槽。
    if (user->getSecurityCard()->isActivate() != 0)
    {
        user->getSecurityCard()->setRequestState(
            WongWork::CSecurityCard::eRequestStateNone);
        user->getSecurityCard()->incCancelCnt();
        DB_SecurityCardUpdateCancelCnt::makeRequest(
            user->get_acc_id(),
            (unsigned int)user->getSecurityCard()->getCancelCnt());
        // [b23S3C] ORIG +0x08c：尾部 getCancelCnt 后 `cmp $0x9,%eax` 死比较
        // （结果弃置、无分支）⇒ 比较表达式语句形。
        if (user->getSecurityCard()->getCancelCnt() == 9)  // ORIG cmp $9（死比较岛）
        {
        }
    }
    else
    {
        if (user->getPad()->isActivate() != 0)
        {
            user->getPad()->setRequestState(Sanicova::CPad::eRequestStateNone);
            user->getPad()->incCancelCnt();
            DB_PassPadUpdateCancelCnt::makeRequest(
                user->get_acc_id(),
                (unsigned int)user->getPad()->getCancelCnt(),
                (char*)user->getWebAddress());
            if (user->getPad()->getCancelCnt() == 9)      // ORIG +0x12d cmp $9（死）
            {
            }
        }
    }
    return 1;
