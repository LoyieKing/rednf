// [b28d-62] N18b：cmp 操作数序（写 `m_pAreas == pEnd` 亦不可翻）+ N112 对齐
//   nop；39=39，p167 CLEAR。ORIG 0x086c3348
// [C4A0Nine 2026-09-11 复验] 交换书写序 `pEnd == m_pAreas` ⇒ 4.4.7 仍产
//   `cmp %ebx,%eax`（39/41 不变）；`(*pEnd).~Area()` deEv 形亦不翻 ⇒ N18b/N112
//   双封顶。ORIG 0x086c3383 `cmp %ebx,%eax`（= m_pAreas-pEnd 减序）为 4.1 独有形。
void Village::destroy()
{
    if (m_pAreas)
    {
        if (m_pAreas)
        {
            // §4.3 具名化：Area 步长由真实尺寸给出（sizeof(Area)==0x98 探针
            // 实证，Area.h 权威布局），不再写字面量 0x98。
            // [b28d-62] 去 count 具名局部（P0/P183）：ORIG 0x086c3366 直接
            //   `mov p; mov p; sub $4; mov (%eax),%eax; imul $0x98; lea (%edx,%eax,1),%ebx`
            //   —— 只有 pEnd 一个 callee-saved 缓存，count 是内联右值。
            register Area* pEnd = m_pAreas + ((int*)m_pAreas)[-1];
            // [b28d-62] ORIG 0x086c337d 布局为「cond 在头 + body 尾 jmp 回 cond」，
            //   4.4 对 while 旋转成前置 jmp ⇒ 改 for(;;)+break 复现（P283 反向）。
            for (;;)
            {
                if (m_pAreas == pEnd)
                    break;
                pEnd = pEnd - 1;   // §4.3（同上，负向遍历）
                pEnd->~Area();
            }
            ::operator delete[]((char*)m_pAreas - 4);
        }
        m_pAreas = 0;
    }
}