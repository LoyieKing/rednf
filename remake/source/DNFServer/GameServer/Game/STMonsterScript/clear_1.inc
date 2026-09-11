// ---- stEnamyDropAvatarInfo_t 成员（ORIG W 0x088cc10c / op= W 0x08366ea8）----
// [b28d-solo-STMonsterScript] 修法（P30 反例解 / P291 具名数组成员视图）：第二循环索引
// `m_field08[i+4]` 在 4.4.6 恒折 `mov i,%eax; lea 0x4(%eax),%r`（this 装载被排到 lea 后），
// 与 ORIG 的 `mov i,%edx; mov this,%eax; lea 0x4(%edx),%ecx` 反序。改走头内匿名 union 的
// 二维视角 `m_field08v[1][i]`（行宽 4 ⇒ 行偏移 add/lea $0x4）后，clear 第二循环与 ORIG
// 逐条全等（探针 /tmp/solo_probe1.cpp clear_v2，4.4.6）。i=4 时 [1][4] 越界至 +0x28 第 9 个
// float —— ORIG 原码即如此写（P171 照抄），内存仍在 union 36B 内。
// ORIG 0x088cc10c
void stEnamyDropAvatarInfo_t::clear()
{
    m_field00 = 0;                 // movl $0,(%eax)
    m_field04 = 0;                 // movl $0,0x4
    for (int i = 0; i <= 3; ++i)
        m_field08v[0][i] = 0;      // [base+8+4i]
    for (int i = 0; i <= 4; ++i)   // [base+8+4(i+4)]：ORIG `lea 0x4(%edx)` 形
        m_field08v[1][i] = 0;
    m_field2c = 0;
    m_elements.clear();            // vector<element>::clear(+0x30)
}