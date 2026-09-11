// [b28d-11b] 修法：party/curVill/curArea 声明序翻槽 + && 链拆嵌套 if 复现 sete +
// party 预清前移 + ret 移入内层块 + while(ret!=0){return ret;} 复现 setne
// → IDENTICAL 205 @0x086d1002
int GameWorld::check_move_area(CUser* user, int vill, int area, int x, int y,
                               int z, bool flag, int param)
{
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    if (user->CheckInTrade())
    {
        return 0xda;
    }
    if (user->IsBusyDisjointStore())
    {
        return 0xbf;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x3c;
    }
    if (user->GetCurCharacExpertJob() != NULL)      // +0x085 第 1 次调用
    {
        // +0x093 第 2 次独立调用（P48）；+0x0a9 sete 物化
        if (((expert_job::CExpertJob*)user->GetCurCharacExpertJob())->GetType() == 1)
        {
        return 0x38;
        }
    }
    CParty* party = 0;                              // +0x0b4 预清槽 -0x1c
    if (user->GetAssaultPlace() != 0)
    {
        return 0x9c;
    }
    if (user->IsCurCharacGhost() &&
        !GlobalData::s_power_manager->IsPowerWarEventOn() &&
        user->getCurCharacVill() != 7)
    {
        return 0x9f;
    }

    if (!user->isGMUser() &&
        IsPowerWarSafeZone(vill, area, (ENUM_POWER_SIDE_TYPE)user->getPowerSide()))
    {
        return 4;
    }

    int curVill;
    int curArea;
    curVill = (int)user->getCurCharacVill();
    curArea = user->get_area(false);
    party = (CParty*)user->GetParty();
    // ORIG +0x1bf..+0x1f3：正向 && 链、全过落 mov $0x13；主路径为跳转目标（块布局反转）
    if (party != NULL)
    {
        if (party->getManager() == user)   // +0x1cc sete 物化
        {
            if (vill == curVill)
            {
                if (area == curArea)
                {
                    if (param != 2)
                    {
                        return 0x13;
                    }
                }
            }
        }
    }
    if (!check_valid_area(vill, area) ||                   // +0x20f xor $1
        !check_valid_area(curVill, curArea))               // +0x231
    {
        return 0x13;
    }
    Village* pV = 0;                                       // +0x24d 先置 0 槽
    if (vill >= 0 && vill < MAX_VILLAGE_NUM)               // +0x254 js/+0x262 jge 正向
    {
        pV = &m_pVill[vill];                               // +0x267 imul $0x34
        if (pV != 0)                                       // +0x27a
        {
            int ret = 0;                                   // +0x27c 先置 0 槽
            ret = user->CheckMoveTown(vill);               // +0x290
            while (ret != 0)
    {
                return ret;                                // +0x2a3
    }
        }
    }
    return 0;                                              // +0x2aa
}