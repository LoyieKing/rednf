// ORIG get_reseal_cost @0x08900b02（T）。见 CDataManager.h 声明注释。
// [b28d-solo-CDataManagerScripts] DIFF→N：p167 CLEAR（3/3）。有符号除 10 已翻正
// （`int idx=(param_2-1)/10` ⇒ ORIG magic 0x66666667）；本轮交换第二项乘法操作数
// ⇒ 消 push %ebx、帧 0x38=ORIG、idx 槽 -0xc=ORIG（判词旧读数 -0x10 作废）；
// 残余 2 条 = ①`cmp -0xc(%ebp),%eax` mem 直比 vs 4.4 `mov -0xc,%edx; cmp`（P209）
// （P209 unsigned 比较恒载寄存器）②`mov %eax,%ecx` 单段 regmove（N18a）；ORIG 0x08900b02。
// [F2d-C05② 甄别] 非命中形：比较对象之一是跨调用返回值 `m_vec.size()`（非两具名局部）；
//   S1 一次性旋钮本件已用尽（idx 改 unsigned 实测劣化回滚）；show_func_diff orig=69 our=71
//   gap=-2 ndiff=8 OURS-only={mov×2}。判 N18b/P209 + N18a。
// [GW20Run 2026-09-11 复核] fresh 69/71；ORIG 0x08900b2 段 `cmp -0xc(%ebp),%eax`
// mem 直比（P209 附，unsigned 比较改寄存器须联判符号性，禁单点）与
// `mov 0x4(%eax,%ecx,4),%ecx` 单段（N18a）均与判词一致；p167 3=3 CLEAR ⇒ 维持。
int reseal_table_t::get_reseal_cost(unsigned char param_1,
                                    unsigned char param_2,
                                    unsigned char param_3)
{
    // param_1 强化等级索引 0..5；param_2 等级 1..99；param_3 选项 0..6
    if (param_2 == 0)              // -0x20(%ebp)==0
        return -1;
    if (param_1 > 5)              // -0x1c(%ebp)>5 → jbe 未命中时
        return -2;
    if (param_2 > 99)             // -0x20(%ebp)>99
        return -3;
    if (param_3 > 6)              // -0x24(%ebp)>6
        return -4;

    // (param_2-1)/10（0x66666667 乘法魔数 → 无符号除 10 的 32 位无符号截断）
    // [本波实测] S1 一次性旋钮：idx 改 unsigned（与 size() 同型）**劣化**——
    // 除法由有符号魔数（`imul $0x66666667; sar $0x2; sar $0x1f` 修正链）退化为无符号
    // `mul`，且新增 `mov %eax,-0x2c` 中转槽（帧 0x38→0x48）；已回滚。
    // ⇒ 残余 2 条（`cmp -0xc(%ebp),%eax` mem 直比 vs 4.4 `mov -0xc,%edx; cmp %edx,%eax`）
    // 归 N（P209：4.4 对 unsigned 比较恒载寄存器；ORIG 0x08900b86 为无符号 mem 直比）。
    int idx = (param_2 - 1) / 10;
    if (m_vec.size() <= idx)      // setbe → vector::size() <= idx（ORIG 0x08900b86 mem 直比 dst=eax）
        idx = (int)m_vec.size() - 1;   // 取末元素
    // return m_vec[idx]*m_arr1[param_1] + param_3*m_arr2[param_1]
    // （ORIG imul 为 32 位模 2^32，此处用无符号累加再一次性转 int 保持位模式）
    // [b28d-J10 复核] 交换第三项乘法操作数为 `param_3 * m_arr2[param_1]` 实测
    // 劣化（our 71→73：40 字节新增中转 + 帧漂）⇒ 已回滚，维持 m_arr2 在前的形。
    return (int)(m_vec[idx] * (unsigned int)m_arr1[param_1]
                 + (unsigned int)m_arr2[param_1] * (unsigned int)param_3);
}