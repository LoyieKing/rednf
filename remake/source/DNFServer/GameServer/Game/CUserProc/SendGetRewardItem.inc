// [b28d-solo-CUserProc N-G] ORIG 0x081224ac：52 条与重建全等（含 EH landing / 帧 0x40 /
// 槽 -0x19/-0x18/-0x2c），p167 8/8 CLEAR；唯一残差 = put_binary 实参装载序：ORIG(4.1)
// 先 `lea -0x19(%ebp),%eax` 再 `movl $0x5,0x8(%esp)`，4.4 在直线块内恒反（先压 arg1）。
// 该序由基本块上下文决定而非源码形：同文件 SendActionClear 的 put_binary 位于 if/else
// 汇合块首，4.4 即产 ORIG 的 lea-先形（该件 AE 实证）⇒ 源码不可控。穷举 18 形（cast /
// (void*)中转 / reinterpret_cast / &首成员 / 具名指针 / volatile 指针 / 具名 int /
// sizeof / 语句表达式 / 逗号表达式 / 地址算术 / 5u / (int)5 / 具名 receiver / volatile
// 对象 / 指针+0 / 5+0 / 宏形）全败，且 4.4.6 后端读数与 4.4.7 逐条相同（非后端敏感族，
// P230 补）⇒ 判 N-G（P75 跨版本实参装载时刻）。
// [N6S5ZeroA 复核] 重跑确认：ours 53 / ORIG 53（含尾部 nop），唯 19/20 两条对调。
//   新增第 19 形 `sizeof(data)`（size_t 常量 5，pack(1) ⇒ 值等于 5，语义不变）仍全败。
//   形参槽逐条核对无「字段错位/形参互换」：0xc(%ebp) 4 字节 → data+1(itemIdx，-0x18)，
//   0x10(%ebp) 低字节 → -0x2c 实参溢出槽 → data+0(flag，-0x19)，与 ORIG 0x081224e0/
//   0x081224e6/0x081224ea 三段逐条同形；缺失校验/接收者均无发明。判 N-G 不变。
//   [C4A0Nine 2026-09-11 复验] 第 20 形 deEv 视图（`(*(InterfacePacketBuf*)&guard)
//   .put_binary(...)`）亦不翻（52/53 不变，ORIG 0x081224ed `lea -0x19(%ebp),%eax`
//   先于 0x081224f0 `movl $0x5,0x8(%esp)`）⇒ N-G（P75）封顶不变。
void APSystem::CUserProc::SendGetRewardItem(CUser& user,
                                            _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward,
                                            bool flag)
{
    PacketGuard guard;
    ((InterfacePacketBuf*)&guard)->put_header(1, 0x229);

    UserProcRewardPayload data;
    data.itemIdx = reward.m_itemIdx;
    data.flag = flag;
    ((InterfacePacketBuf*)&guard)->put_binary((char*)&data, 5);
    ((InterfacePacketBuf*)&guard)->finalize(true);
    user.Send(guard);
}