// ---- GetAssaultPrevent（报告，ORIG T 0x085d137c sz 0xa7）----
// b28c-1 AE: P0/N96 清绑 + P223 双岛（0x085d140a/0x085d140d）后 0 差。
bool CAlchemist::GetAssaultPrevent(CUser* user, bool* peerFlag)   // [P354/P292] ORIG `call; test %al,%al` ⇒ bool（原 int 误标）
{
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (IsExistContinuousEffectItem(user, 0x27ac4a))
    {
        return 1;
    }
    if (party != 0)
    {
        for (int i = 0; i < 4; ++i)
        {
            CUser* member = party->get_user(i);
            // b28c-1：P223 多岛形（ORIG 0x085d140a member==0 岛 / 0x085d140d
            //  member==user 岛），清绑后裸条件 + goto 到相邻尾标签。
            if (member == 0)
                goto NEXT1;
            if (member == user)
                goto NEXT2;
            // b28：清 register asm（N96/P0），裸 if 直测。
            if (IsExistContinuousEffectItem(member, 0x27ac4a))
            {
                if (peerFlag != 0)
                {
                    *peerFlag = true;
                }
                return 1;
            }
        NEXT1:
            ;
        NEXT2:
            ;
        }
    }
    return 0;
}