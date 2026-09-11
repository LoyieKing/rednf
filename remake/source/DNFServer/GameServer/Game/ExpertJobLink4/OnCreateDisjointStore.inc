// ---- CDisjointer::OnCreateDisjointStore（报告 OnCreateDisjointStore.md，
//      ORIG T 0x085d2d08 sz 0x391）----
// [L2Free4to15.S3 复核] p167 CLEAR（差集仅 C1/C2·D1/D2 同址别名，P93）。
// 残差 = 双对象槽方向相反（ORIG CDisjointMachine 临时 -0x3c / PacketGuard
// -0x48；我方 -0x48 / -0x1c = N93）+ EH cleanup 块序（ORIG 侧
// _Unwind_Resume 多 1）。交换声明序会改构造序/指令序 ⇒ N93 封顶。
int CDisjointer::OnCreateDisjointStore(CUser* user, char const* name, int level,
                                       int x, int y)
{
    if (level < 0 || GetScript()->m_limitDisjointGold < level)   // +0x28c
        return 10;

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
        return 0xbe;

    if ((char)user->getCurCharacVill() == 7)
        return 0xbe;

    // [b28d-solo-ExpertJobLink4 P0/P23]：ORIG 0x085d2d80/0x085d2dd7 两次独立
    // 调用 get_area→%ebx、getCurCharacVill→%esi（无 area/vill 具名槽）。
    if (!G_GameWorld()->is_available_point((int)user->getCurCharacVill(),
                                           user->get_area(false), x, y))
        return 0xbe;

    int restrictErr =
        G_GameWorld()->check_restrictive_commercial_transaction_zone(
            (int)user->getCurCharacVill(), user->get_area(false), x, y);
    if (restrictErr != 0)
        return restrictErr;

    // 耐久检查（两次独立取扩展数据，与 ORIG 调用序一致）。
    if (((expert_job::CCharacExpertJob*)user->GetCharacExpandData(
             (ENUM_CHARAC_EXPAND_TYPE)0))->get_disjoint_machine_endurance() <= 0)
        return 0x16;

    // 局部构造后赋给 m_machine（ORIG 经栈上临时 + operator= + 临时析构）。
    // ORIG 0x085d2e77 `call get_disjoint_machine_grade; movsbl %al,%eax` 无
    // machineGrade 具名槽（内联进 ctor 第4实参）。
    CDisjointMachine local(
        user, name, level,
        (char)((expert_job::CCharacExpertJob*)user->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0))->get_disjoint_machine_grade(),
        x, y);
    m_machine = local;

    user->set_position((unsigned short)x, (unsigned short)y, '\0');

    // [b28d-solo-ExpertJobLink4 P212/N93]：ORIG Zone 槽 -0xe + &m_machine 提前入
    // %esi（0x085d2f10）⇒ 匿名临时实参形（具名 zone 在 4.4 下占更深槽）。
    if (!GlobalData::s_villageObjectMgr->register_object(
            village_object::Zone(user->getCurCharacVill(),
                                 (unsigned char)user->get_area(false)),
            &m_machine))
        return 0x14;

    PacketGuard packet;
    packet.put_header(0, 0xce);
    m_machine.put_disjoint_machine_info(packet);
    packet.finalize(true);
    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                packet);

    packet.clear();
    packet.put_header(1, 0xee);
    packet.put_byte(1);
    packet.finalize(true);
    user->Send(packet);

    return 0;
}