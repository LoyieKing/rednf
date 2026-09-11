// ORIG T 0x82e8c5a：包 0x03 全场用户状态；先记录写位置，尾部回填人数。
// [round1-L29 2026-09-10 复核] 同 2 参版：1 处 N7 寄存器角色互换 + 尾部 epilogue 1 条
//   （ORIG 0x082e8c5a）。判据① CLEAR（16 vs 16 分支，目标无错位）。
// [P8S1 2026-09-11 本波复核] 维持封顶：N7 同上(ORIG 0x82e8c5a+0x0a9)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
void CAssaultPlace::_MakeUserState(PacketGuard* packet)
{
    // b28d-solo-CAssaultMgr: N 封顶（N7 寄存器角色互换；清单 DIFF 27 复验）
    // 同双参重载：ORIG 0x82e8c5a+0x0a9 `call GetUserState; mov %eax,%edx;
    // mov packet,%eax; ...`（73 条）vs 4.4 this-first（72 条）——单条
    // %eax↔%edx 角色互换，语义等价。探针矩阵同双参件（/tmp/probe_mus*.cpp）。
    // [S6 2026-09-11 复核] 同判词：p167 CALLSITE CLEAR（12/12）⇒ N7，零改动。
    // ORIG 0x82e8c5a +0x0b：`movl $0,-0x14; movl $0,-0x10` 双清零在前、
    // get_index 覆写 index 在后（dead init 照抄 P171，与 MakeView 同法）。
    packet->put_header(0, 3);
    int index = 0;
    int count = 0;
    index = packet->get_index();
    packet->put_byte(count);
    for (int i = 0; i < 8; ++i)
    {
        if (m_assaulters[i].Empty())
            continue;
        // P48 去缓存每用点重取：ORIG 0x82e8c5a 对 GetUser() 两次独立重调
        //（+0x079 unique_id / +0x0a4 GetUserState），不缓存进具名局部。
        packet->put_short((int)(m_assaulters[i].GetUser()->get_unique_id()
                                & 0xffff));
        // ORIG +0x0a9 GetUserState 结果先借 edx（mov %eax,%edx）再重载 this
        // 于 %eax；本地实现为 eax/edx 换位（寄存器分配 N-residual，语义等价）。
        packet->put_byte(m_assaulters[i].GetUser()->GetUserState());
        ++count;
    }
    packet->put_byte(index, count);
    packet->finalize(true);
}