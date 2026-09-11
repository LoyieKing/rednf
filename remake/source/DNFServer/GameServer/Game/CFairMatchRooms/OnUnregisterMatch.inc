// OnUnregisterMatch ORIG T 0x85663cc6（README 行：0x08563cc6）报告同名 md
// [b28 P222-y10 DIFF→NEAR] 否定合并前置形已翻正块序（ndiff 20→6）；残余 6 行 =
// DelRoom 虚槽 call *%ecx vs *%eax = N7 封顶。
// ORIG 返回 int：条件不满足 → 0；执行 → 1。
// [b28 BM2 判词·N7] AE-ndiff=8（fresh .o 归一）：DelRoom/Remove 虚槽 call *%ecx
// vs call *%eax + 实参 %edx/%eax→%ecx/%edx 对调（N7 族）。其余逐条同。
int CFairMatch::OnUnregisterMatch(CUser* /*user*/, PvP_Room* /*room*/)
{
    // [b28 P59 变体·y10 探针 /tmp/l2_p5.cpp] 否定合并 `!=1 && !=2` return 0 前置形
    // 令 4.4 产 ORIG 0x08563cc6 布局：cmp$1;je body / cmp$2;je body（双 je 同目标）
    // / fall-through `mov $0;jmp` false 岛 / body 尾 `mov $1`。旧正极性共享体形产
    // `jne end` 反向+岛换位（DIFF 20 中真差 4 行）。goto 形带 nop 岛（y3）不用。
    // 语义不变：仅 f04∈{1,2} 时注销并返 1，否则 0。
    if (m_field04 != 1 && m_field04 != 2)
        return 0;
    m_field04 = 0;
    // ORIG +0x2d..0x48 无判空直调 vtable[+0x04]（忠实模拟 ORIG 未定义行为，P148）。
    // 残余 call *%ecx vs call *%eax = N7（4.4 间接 call 目标恒 %eax）。
    pWaitingRoomList_->DelRoom(this);
    _SendMatchStatePacket(5, 1);
    return 1;
}