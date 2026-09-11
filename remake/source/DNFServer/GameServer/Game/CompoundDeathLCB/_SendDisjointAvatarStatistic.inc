// ORIG 0x08333ecc。统计服上报本次分离的两件头像分级；注意 B 组 subType 也读
// scriptA 的 vtbl+0xc（ORIG 如此）。
// 【b28d-solo-CompoundDeathLCB 判词】NEAR（79/79 逐条全等；帧 `push %ebx;
//   sub $0x54`、全部槽号、`lea -0x3c(%ebp),%ebx` 与 ORIG 全等）。修法：
//   ①两组分级复用同一 gradeIdx 槽（P160，两个独立局部会各占一槽 +4 帧）；
//   ②proxy 具名局部改单语句链式调用（P52，消槽且 &packet 升格 ebx）。
//   残余 = CI_SubType 虚调用目标寄存器 N7 ×2（4.4.7 恒 %eax / ORIG %edx）。
// 【b28e-M1S2 本波复核】复测 orig 79 / ours 79（NEAR 不变；帧 push %ebx +
//   sub $0x54、`movl $0x2,-0x32`、`movb $0x0,-0x26/-0x19` 槽号全等）。
//   语义轴：本件无字符串字面量（无 §6.5/§6.5.1 盲区）；B 组 CI_SubType 亦读
//   scriptA（ORIG 0x08333f8e 用 -0x14(%ebp)）已一致。唯一残余 = N7 ×2
//   （ORIG `mov (%eax),%edx; mov -0x14(%ebp),%eax; mov %eax,(%esp); call *%edx`
//    / 我方 `mov (%eax),%eax; mov -0x14(%ebp),%edx; mov %edx,(%esp); call *%eax`）。
void CCompoundAvatar::_SendDisjointAvatarStatistic(tagCompoundInfo& info)
{
    Packet_Avater_Disjoint_Statistic packet;

    const CItem* scriptA = info.m_scriptA;
    const CItem* scriptB = info.m_scriptB;

    packet.m_field0a = 2;                                      // -0x36/local_36
    // [b28d-solo-CompoundDeathLCB] ORIG 0x08333ef6/0x08333f52 两组分级**复用同一
    // -0xc 槽**（各以 `movl $0,-0xc` 重新初始化）⇒ 源码必须是同一个具名局部
    // 两次使用（P160 复用槽；两个独立局部会各占一槽，实测 +4 帧）。
    int gradeIdx = 0;
    if (scriptA->get_rarity() == 2)
    {
        gradeIdx = 2;
    }
    else
    {
        gradeIdx = scriptA->get_grade() - 1;
    }
    packet.m_gradeIdxA = gradeIdx;
    packet.m_subTypeA = CI_SubType(scriptA);
    packet.m_flagA = 0;
    gradeIdx = 0;
    if (scriptB->get_rarity() == 2)
    {
        gradeIdx = 2;
    }
    else
    {
        gradeIdx = scriptB->get_grade() - 1;
    }
    packet.m_gradeIdxB = gradeIdx;
    packet.m_subTypeB = CI_SubType(scriptA);                   // 【ORIG 怪癖】保留
    packet.m_flagB = 0;

    // [b28d-solo-CompoundDeathLCB] ORIG 0x08333fa7：`lea -0x3c(%ebp),%ebx` 先把
    // &packet 升格 callee-saved，再求 receiver（GetServerProxy）⇒ 单语句链式
    // 表达式（P52）；具名 proxy 局部会多一个槽且 &packet 退化为实参处现算。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet),
                      static_cast<int>(sizeof(packet)));       // 0x28
}