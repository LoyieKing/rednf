// [b28d-18 N7] 修：register seed 使 seed 先载（P231）；余纯寄存器名轮换（N7 封顶） | ORIG 0x085f0d64
// [b28d-solo-GlobalData] NEAR 封顶 N7：p167 逐调用同形（imul/lea/seed 段一致）；
// 残差=callee-saved 角色轮换（ORIG eax/edx/ecx vs 4.4 ebx/eax/edx，P230 定稿：
// 4.4.7 分配固定，源形不可择）+ 首条 vptr 存的重定位显示伪差（`$&<L>` vs `<L>`）。
random_option::CRandomOptionItemHandle::CRandomOptionItemHandle()
{
    // ORIG 0x085f0d64：new[0x7f28] 13×CMTRand，逐元素 seed(time(0))；
    // 字段清零；+0x14 = GetRandomOption()+0x148 的指针。
    m_rand = new CMTRand[13];
    unsigned long t = (unsigned long)time(0);
    for (int i = 0; i < 13; ++i)
    {
        // [b28c-lane17] P0 清绑复测：ORIG 0x085f0d64 有 t 的**槽往返**
        // （`call time; mov %eax,-0x20` → 循环内 `mov -0x20(%ebp),%eax`），
        // 故具名局部 t 正确；但旧版在循环内另设 `unsigned long tv = t;`
        // 是**无槽依据的额外局部**（ORIG 无第二个 seed 槽）⇒ 反产 +2 条
        // store/load。直传 t 后指令数回到 ORIG 的 69。
        // 残余 = seed 实参与地址计算的**装载时刻序**：ORIG 4.1 在地址
        // imul/lea 之前把 t 载入 %eax（`mov -0x20,%eax` 先行），4.4 恒按
        // 「this→下标→实参」序装载 ⇒ P75/N86（实参装载时刻跨版本调度），
        // 语义等价（seed 实参同一槽值），保持可读源形。
        // [b28d-solo-GlobalData] P248 裸 register 实测：删 register 反生 seed 二段
        // 拷贝（69→71，多 mov -0x24→-0x1c），保留 register 回到 69=69；余纯寄存器
        // 名轮换（ORIG eax/edx/ecx vs 4.4 ebx/eax/edx）⇒ N7 封顶（P230：4.4.7 分配固定）。
        // [G7-sub] 本轮复核：ORIG 69 / 我方 69、帧 0x2c==0x2c；p167 仅
        //   `CMTRand` C1↔C2 别名。唯一残差是 seed 实参装载时刻：
        //   ORIG 0x085f0de2 `mov -0x20(%ebp),%eax` 值先载 %eax 再算地址；
        //   我方 0x…`mov -0x20(%ebp),%ebx` 落 callee-saved ⇒ P75/N96/N7 封顶。
        register unsigned long seed = t;
        m_rand[i].seed(seed);
    }
    m_field08 = 0;
    m_disabled = 0;
    m_field10 = 0;
    // ORIG 0x85f0e2d..0x85f0e3b：m_optionSet = GetRandomOption() + 0x148
    // （m_randomOption 内嵌 provider 对象基址 m_providerNormal148；虚槽0/1 见 RandomOptionScript.h）。
    m_optionSet =
        &reinterpret_cast<RandomOptionScript*>(GetRandomOption())->m_providerNormal148;
}