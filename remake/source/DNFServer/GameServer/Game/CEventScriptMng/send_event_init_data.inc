// [b28d-solo-CEventScriptMng] send_event_init_data：DIFF — N82附 封顶。
// ORIG 0x0810c40e：包 0x176；end() 循环头缓存（P13）后全帧槽位逐条对齐
// （PacketGuard -0x28 / countIdx -0x2c / it -0x30 / endIt -0x34 / vfn -0x40 /
// count -0x19 与 ORIG 全等）。
// 残差唯一：多实参虚调用三活值（vfn/flag/m_todayTimeDay）的 callee-saved 角色
// 对调——ORIG vfn→槽 -0x40、flag→%edi、day→%esi；4.4.7 vfn→%esi、flag→槽
// -0x40、day→%edi（多 1 条 `mov -0x40(%ebp),%edx` 回读，135 vs 136 条）。
// 实测两形：①原实参形（本形）；②P226 语句表达式 `({register bool f=flag;f;})`
// 反劣（多 movzbl 中转，且循环旋转）。flag 的裸 register 三条件中「4.4 不自行
// 升格」不成立 ⇒ 无源码解，封 N82附（多实参 call 首值 callee-saved 对调）。
// p167 CLEAR（19/19），语义闭合。
void CEventScriptMng::send_event_init_data(CUser* user, bool flag)
{
    if (user == 0)
        return;

    PacketGuard packet;
    packet.put_header(0, 0x176);
    packet.put_int(m_todayTime);
    int countIdx = packet.get_index();
    packet.put_byte(0);
    unsigned char count = 0;
    // ORIG 0x0810c487：end() 在循环初始化区求值一次（P13），it 槽 -0x30 /
    // endIt 槽 -0x34；每轮重调 end() 会多占 sret 临时槽并顶偏 PacketGuard 槽。
    for (std::map<int, CEventEntity*>::iterator it = m_entities.begin(),
             endIt = m_entities.end();
         it != endIt; ++it)
    {
        if (is_eventing(it->first))
        {
            if (it->second->send_init_data(user, packet,
                                           static_cast<short>(it->first),
                                           m_todayTimeDay, flag))
                ++count;
        }
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    user->Send(packet);
}