// ORIG 0x08230844 W _ZN5CUser31SetDeleteEventDungeonInvitationEb：
// 写 m_deleteEventDungeonInvitation 字节（objdump 逐指令转写；SelectDungeon::process 对全队调用）。
void CUser::SetDeleteEventDungeonInvitation(bool flag)

{
    m_deleteEventDungeonInvitation = flag ? 1 : 0;
}
