bool AvatarRechargeServer::UseEmblem(CUser* user, std::vector<int>& grades,
                                     Inven_Item* emblemItem, short slot)
{
    // [b28d-solo-AvatarRechargeServer] P52 链式：ORIG 0x08190ee8 `mov 0x14(%ebp),%eax;
    // mov 0x7(%eax),%ebx` 把 m_addInfo2 升格 %ebx 跨两 call 保活，无 invenR 具名槽。
    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
            emblemItem->m_addInfo2);
    // ORIG 0x08190f1b `call getSocketCount; mov 0x4(%edx),%edx; imul %edx,%eax`
    // ⇒ 原码书写序为 count * limit（具名基类字段，原裸偏移 §4.3 禁）。
    const int maxEndurance =
        getSocketCount(socketData) * m_rechargeLimit;

    int addSum = 0;
    // [b28d-solo-AvatarRechargeServer] 真语义修正：ORIG 0x08190f39/0x08190f7d
    // `lea 0x20(%eax),%edx` = Server+0x20 = m_emblemRechargeMap（[emblem recharge]），
    // 旧源码误用 m_useEmblemGradeRateMap（this+0x38）。循环形照 ORIG：迭代器先声明
    // 后赋值（P50，C1Ev 默认构造）+ end 缓存（P13）+ i/n 缓存。
    std::map<int, int>::iterator it;
    std::map<int, int>::iterator itEnd = m_emblemRechargeMap.end();
    for (std::vector<int>::size_type i = 0, n = grades.size(); i < n; ++i)
    {
        it = m_emblemRechargeMap.find(grades[i]);
        if (it != itEnd)
            addSum += it->second;
    }

    Inven_Item* avatar = user->getCurCharacInvenW()->GetInvenRef(2, (int)slot);
    if (avatar == 0 || avatar->isEmpty())
    {
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   (unsigned short)slot);
        return false;
    }

    int newEndur = (int)avatar->m_fieldSplit.m_avatarEndur + addSum;
    if (newEndur > maxEndurance)
        newEndur = maxEndurance;
    const int added = newEndur - (int)avatar->m_fieldSplit.m_avatarEndur;
    avatar->m_fieldSplit.m_avatarEndur = (unsigned short)newEndur;

    UpdateAvatarEndurance((CUser::eSendTarget)1, user, slot);

    // [b28d-solo-AvatarRechargeServer] P227 全表达式临时链：ORIG 0x081910a2..0x081910e4
    // 先把 getCurCharacNo→%ebx、emblem 耐久→-0x64、item_id→%edi、slot→%esi 全部求值，
    // 再 call cMyTrace::C1，之后逐槽压 operator() 实参（P139 用 __PRETTY_FUNCTION__）。
    cMyTrace(__PRETTY_FUNCTION__, 0x142, 0)(
        "USE EMBLEM + RECHARGE : success [slot:%d] [item_id:%d] "
        "[Endurance:%d] [charac_no:%d]",
        (int)slot, avatar->m_addInfo,
        (int)(unsigned short)emblemItem->m_fieldSplit.m_avatarEndur,
        user->getCurCharacNo());

    arad::SigAradAvatarEnduranceSave save;   // ORIG ctor：清 +0/+4
    save.uiId = (unsigned int)avatar->m_addInfo2;
    save.endurance =
        (unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur;
    SaveAvatarEnduranceUnit(save);
    return true;
