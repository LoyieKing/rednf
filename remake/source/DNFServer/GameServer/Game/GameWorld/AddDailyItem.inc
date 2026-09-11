// [b28d-11b] 修法：it++（P28 增量侧）+ 镜像 if（setle/jne）+ 负条件 goto 尾标签
// 补尾 jmp 岛（P223）→ IDENTICAL 116 @0x086ccdce
void GameWorld::AddDailyItem()
{
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() <= 2)
        {
            goto L_next;
        }
        user->AddDailyItem();
        if (user->getCurCharacR() != 0)
        {
            user->ResetDailyQuest();
            user->ResetTrainingQuest();
            PacketGuard packet;
            user->getCurCharacQuestR()->get_quest_info((char*)&packet);
            user->Send(packet);
        }
        // b28d-lane11：P225 推广——ORIG 两条件各发 `setne %al; test; je`，
        // 合并 && 被 4.4 折成直跳 ⇒ 拆嵌套 if。
        if (user->getCurCharacR() != 0)
        {
            if (user->get_charac_level() > 0x3b)
            {
                user->DimensionInoutUpdate(true, true);
            }
        }
    L_next:;
    }
}