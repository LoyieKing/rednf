// ============================================================================
// DisJoint::_GetAdditionalItem @ 0x084732b4（ORIG mangled ...EPK5CItemRiS3_：
//   outIdx/outCount 为 int& 引用参数，非指针）
//   outIdx = _GetAdditionalItemInfo(item->get_rarity())；非零时按
//   m_additionalConst[rarity].m_item*100 权重二选一分母浮点
//   （m_additionalWeight / m_additionalRate[rarity]），
//   outCount = (int)(grade / denom)（ORIG fistpl 前 fldcw 置 RC=truncate，
//   截断非四舍五入），最小 1。
//   分支内 get_grade/get_rarity 各自独立再调用（O0 无 CSE，P48）。
// ============================================================================
void DisJoint::_GetAdditionalItem(const CItem* item, int& outIdx, int& outCount)
{
    // [b26N1] m_additionalWeight[rarity] 大偏移寻址：ORIG(4.1) 拆 add $0x150 进
    // scaled idx + disp 0x10；4.4 恒折 add $0x154 单步（N1 族折叠方向不可翻），
    // 其余全等，ndiff 12 封顶。
    outIdx = _GetAdditionalItemInfo(item->get_rarity());
    // ORIG 0x084732de `test %eax;setne %al;test %al;je`：outIdx!=0 物化为
    // bool 局部（P194 setne 形）。[b28c-21] 清 register asm 存量绑定（P0/N96），
    // 改普通 bool 自然形。
    // [N1Free91to105 2026-09-11] 具名 bool 局部版已实测：帧 0x48（ORIG 0x38）、
    // 多发 `mov %al,-0x19; cmpb $0x0,-0x19` ⇒ 违反 P0（ORIG 无该槽）。改为直接
    // 条件后 **帧 0x38/槽集与 ORIG 全等**，105→101；残余仅 ORIG 独有 2 条
    // `setne %al; test %al,%al`（4.1 -O0 对 `if(bool_expr)` 恒物化 bool，4.4 fold
    // 为直 je）——N 类跨版本，无源码形可复现（试过具名 bool/无局部两形）。
    // 另一处 `add $0x150,%edx; flds 0x10(%eax,%edx,4)` vs `add $0x154,%edx; flds (%eax,%edx,4)`
    // 为等价地址（0x150*4+0x10 == 0x154*4 == 0x550）的 disp 折叠差（N1，方向不可翻）。
    if (outIdx != 0)
    {
        // ORIG：m_item 先落 -0x18 再乘 100 落 -0x14，随后才 call get_rand_int。
        int w = m_additionalConst[item->get_rarity()].m_item;
        int weight = w;
        weight *= 100;
        if (get_rand_int(10000) < weight)
        {
            // ORIG 中间 float 槽：fstps/flds 往返后 fistpl 截断。
            const float v = (float)item->get_grade() /
                            m_additionalWeight[item->get_rarity()];
            outCount = (int)v;
        }
        else
        {
            const float v = (float)item->get_grade() /
                            m_additionalRate[item->get_rarity()];
            outCount = (int)v;
        }
        if (outCount < 1)
        {
            outCount = 1;
        }
    }
}