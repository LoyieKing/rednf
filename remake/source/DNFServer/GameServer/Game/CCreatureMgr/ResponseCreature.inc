bool CCreatureMgr::ResponseCreature()
{
    CPacketRespondent respondent(m_user, 0, 0);
    respondent.MakeNotipacketResponseCreature();
    // ORIG 0x833a726/0x833a74c：area 与 Broadcast 返回值两段生命周期复用 %ebx
    // （无栈槽，frame 0x30、respondent@-0x28）。
    // [b28d-23] P0/N96 清绑：删 asm("ebx") 绑定；按 P231/P248 保留**纯 register**
    // 存储建议（非绑定形，P0 合规）：值跨 getCurCharacVill call 保活且 4.4 不自升格
    // （三条件齐备）。
    // [b28d-solo-CPacketRespondent 消费点同步] Broadcast 返回 bool（ORIG 0x8340404
    // gate 臂直透 `call CUser::Send` 无 movzbl）⇒ 直接 return 让 4.1/4.4 都走
    // `call Broadcast; mov %eax,%ebx; <dtor>; mov %ebx,%eax`（返回值跨析构保活，
    // bool 无需归一化）；area 单独用 register int 落 %ebx（get_area 返回 int）。
    register int area;
    area = m_user->get_area(false);
    return respondent.Broadcast((int)(char)m_user->getCurCharacVill(), area);
}
