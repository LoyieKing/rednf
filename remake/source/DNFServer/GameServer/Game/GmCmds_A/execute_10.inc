// ORIG execute 0x084b3ad4：cera→虚表[1](type=5)、point→虚表[3](type=4) 充值下单，
// 最后统一走虚表[11] 通知。CIPGHelper 仅有前向声明，按 ORIG 虚表槽间接调用。
// [b28d-47] verdict=NEAR(routed) [N7] ORIG 0x084b3ad4 orig/ours=67/67
//   残差：间接 call 目标寄存器——ORIG 4.1 %ecx；4.4.7 恒 %eax、4.4.6 恒 %edx
//   （P230 实测 446 后端亦不达 %ecx）⇒ N7 封顶，连锁实参/数据寄存器名互换。
// [b28d-solo-GmCmds_A] verdict=NEAR ndiff=21 [N7] p167 CLEAR(3/3)；三处间接 call
// 目标寄存器 ORIG %ecx vs 4.4.7 %eax / 4.4.6 %edx（P230 实测），语义完成封顶。
// [b29-Lane21] N7 封顶（446 可救，已上报 Main）：ORIG `mov (%eax),%ecx; … call *%ecx` / `call *%edx`
//   vs 4.4.7 全 %eax；槽/条数两侧全等，仅寄存器角色（residual 6 行）。
// [round5-GmCmdsA] verdict=IDENTICAL：旧形为 §4.3 禁的裸指针 cast
//   `(*reinterpret_cast<void***>(p))[n]` —— 4.4 对 `void*`→函数指针的转换恒多发
//   `mov (%eax),%eax; mov %eax,%ecx` 两段（N18a 反向）。改用 TaiwanCash.h 权威虚接口
//   真成员调用（P5 虚槽直调形，Slot1=IPGInput/3=IPGInputPoint/11=IPGQuery）后
//   4.4 直发 ORIG 的 `mov (%eax),%eax; add $0x4,%eax; mov (%eax),%ecx` 单段链，
//   orig/ours=67/67 逐条全等（零新增符号）。
void CBillingRechargeCommand::execute()
{
    CUser* user = GetUser();
    // ORIG（P157 逐点重取形）：每个调用表达式独立重读 s_pIPGHelper → 取 vptr →
    // add 槽 → deref fn，随后再读一次 helper 作 this；禁 vtable 缓存局部
    //（缓存少发 ORIG 的逐点 fetch 行且引入额外栈槽）。
    if (m_str0 == "cera")
    {
        // 读侧用 union 的 unsigned 视角（与 callee 形参 unsigned int 同型 ⇒ 单段直落）。
        GlobalData::s_pIPGHelper->IPGInput(user, 5, m_amountU, "", "", "GM", 0, 0, 0);
    }
    else if (m_str0 == "point")
    {
        GlobalData::s_pIPGHelper->IPGInputPoint(user, m_amountU, 4, 0, 0);
    }
    GlobalData::s_pIPGHelper->IPGQuery(user);
}