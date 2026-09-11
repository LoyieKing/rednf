// ORIG 0x85ba804：村庄攻击/复仇模式进入检查。
int CParty::CheckEnterVillageAttackRevenge(CDungeon const* dungeon)
{
    // 共享出口：flag==0 与循环完成共用末尾 `return 0`（ORIG +0xfb 单出口）。
    if (dungeon->m_villageAttackEventFlag != 0)
    {
        int memberCount = get_member_count();
        if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
            return 0x15;
        // [b28d-SweepCDungeon T1 直证] 字段真型为 unsigned char（ORIG 0x085ba84b/
        //   0x085ba85d `movzbl; movsbl %al` = uchar 字段 + 源码 (signed char) cast），
        //   故此处保留显式 cast 以维持 ORIG 的有符号比较（uchar 字段无 cast 会转无符号）。
        if ((signed char)dungeon->m_maxAttackPartyMember < memberCount ||
            memberCount < (signed char)dungeon->m_minAttackPartyMember)
            return 0xab;
        for (int i = 0; i <= 3; ++i)
        {
            // ORIG 085ba88d 直测 %al（checkValidUser 返 bool）：al!=0 才进入体，
            // ==0 跳循环递增 → 用嵌套 if(checkValidUser(i))，不用 `==0 continue`（免 xor$1）。
            if (checkValidUser(i))
            {
                unsigned char cur = get_user(i)->GetCurRevengeDungeonCount();
                if (cur == 0 || (unsigned char)dungeon->m_maxRevengeCount < cur)
                {
                    // ORIG 085ba8d0 对同一 i 二次调用 get_user（读/写各一次，无 u 缓存局部=P0）。
                    get_user(i)->SetCurRevengeDungeonCount(0);
                    return 0xad;
                }
            }
        }
    }
    return 0;
}