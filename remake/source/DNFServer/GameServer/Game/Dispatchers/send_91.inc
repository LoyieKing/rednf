// ==== Dispatcher_ExpertExtraction::send (ORIG 0x081d9700) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d9700 orig/ours=130/137 ndiff-blocks=25
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | mov    -0x1c(%ebp),%eax | jne    081d989d <+0x19d>
void Dispatcher_ExpertExtraction::send(CUser* user, ParamBase& param)
{
    stExpertExtraction& p = static_cast<stExpertExtraction&>(param);
    // [L7DispG1 2026-09-11] ORIG 0x081d970a `mov 0x4(%eax),%eax; test %eax,%eax; jne 081d989d`
    //   —— 跳向**尾部**的错误块（good path 的 `jmp L_end` 之后），good path 在 fall-through
    //   ⇒ 原码是单出口 `if (m_4 == 0) { … } else { err }`；旧形 `if (m_4 != 0) { err; return; }`
    //   把错块前置于 fall-through（ours 131 vs ORIG 130）。
    if (p.m_4 == 0)
    {
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1a0);
    guard.put_byte(1);
    guard.put_short((int)p.m_8);

    // [round5 A01 · B 轴（循环变量符号性，Main 派单②）] ORIG 0x081d9700 槽图：
    //   count=-0x18（uint，`size()/3` magic 除法）、索引累加器=-0x10（uint）、
    //   循环变量=-0xc（int）；尾测 0x081d983d..0x081d9848
    //   `mov -0xc(%ebp),%eax; cmp -0x18(%ebp),%eax; setl %al; test; jne` ⇒ **setl=有符号**
    //   （P196 前置已过）⇒ i 必须 `int` + `(int)count`；索引走独立累加器（ORIG 每元素 post-inc）。
    // [L7DispG1 负结论] 试把 i 提到 for 外先声明（欲得 ORIG 槽序 i=-0xc/count=-0x18）：
    //   实测槽序被整体打乱（idx 落 -0x10、i 落 -0xc、count 落 -0x18 但 idx/i 的
    //   post-inc 形错位），残余反而增多 ⇒ 回滚。4.4 的 local 槽序不可由声明序单向控制。
    // [P4-P4OrderFix 2026-09-11] count 改 int：ORIG 尾测先载 i 再 cmp count（`setl`），
    //   unsigned count 令 4.4 先载 count 并反转成 `setg`。除法仍在 unsigned 域完成
    //   （size()/3 ≤ 2^32/3 < 2^31 ⇒ (int) 不溢出），探针实证魔数与槽位均不变。
    const int count = (int)(p.m_list.size() / 3);
    guard.put_byte((int)count);
    // [G1Frest 2026-09-10] ORIG 0x081d97ab `mov -0x1c(%ebp),%eax; add $0xc,%eax;
    //   mov %eax,-0x14(%ebp)` ⇒ p.m_list 的**具名引用/指针局部** -0x14（P0 槽往返）；
    //   后续每元素 `mov -0x14(%ebp),%eax; mov %eax,(%esp)` 直接传该槽，
    //   我方旧码每次重算 `p+0xc`（帧少 4B、槽号整体上移）。绑定点在 put_byte(count) 之后。
    std::vector<int>& list = p.m_list;
    unsigned int idx = 0;                       // -0x10
    for (int i = 0; i < count; ++i)             // -0xc（setl 有符号序）
    {
        guard.put_short(list[idx++]);
        guard.put_int(list[idx++]);
        guard.put_int(list[idx++]);
    }
    guard.finalize(true);
    user->Send(guard);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a0, (unsigned char)p.m_4);
    }
}