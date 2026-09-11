// ORIG 0x081d7d56（报告 Dispatcher_TitleBookOtherUser/process.md）：
// msg+0xd（unsigned short）为目标 uniqueId；两侧均要求 get_state() > 2。
int Dispatcher_TitleBookOtherUser::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_TITLEBOOK_OTHERUSER& mref = reinterpret_cast<MSG_TITLEBOOK_OTHERUSER&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    (void)param;
    // ORIG 0x081d7d56（报告 Dispatcher_TitleBookOtherUser/process.md 全文已读）：
    // 三段早退（bool 物化 state<=2 / null / bool 物化 state<=2），局部仅 mref@-0x10、
    // target@-0xc；find_from_world 的 ushort 实参跨 G_GameWorld() 调用物化 %ebx；
    // GetCharacExpandData 结果不经局部槽直传 sendListOtherUser this。
    if ((bool)(user->get_state() <= 2))
    {
        return 0;
    }
    CUser* target = G_GameWorld()->find_from_world((unsigned short)mref.m_targetIdx);
    if (target == 0)
    {
        return 0;
    }
    if ((bool)(target->get_state() <= 2))
    {
        return 0;
    }
    ((CTitleBook*)target->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xe))
        ->sendListOtherUser(user, (ENUM_TITLE_BOOK_INFO_TYPE)1);
    return 0;
}