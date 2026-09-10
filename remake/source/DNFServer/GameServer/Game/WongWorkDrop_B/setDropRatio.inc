void CMonsterDrop_Hell::setDropRatio(float ratio)
{
    // [b28d-solo-WongWorkDrop_B] AE：51=51 逐条同形（残差仅 .o 重定位标签，
    // 见 P162/P264；compare_tu clean + p167 CLEAR）。修法 = 头内 w[5] 视图
    // 复现 ORIG disp=8 折叠（P30），POD 零布局/零符号影响。
    // [b28d-38] ORIG 逐条：vector op= 后双层循环（P165 两份物理独立块，禁 helper
    // call——4.4 -O0 对 inline 模板仍发 out-of-line call，always_inline 展开则形参槽
    // 物化 +6 条）。内层缓存 `e` 引用（ORIG -0x10 槽 + `0x8(%eax,%edx,4)` 直读）。
    m_probsSub = m_probs;                       // ORIG stl_vector::operator=
    for (size_t i = 0; i < m_probsSub.size(); ++i)
    {
        stGenItemProb_t& e = m_probsSub[i];
        for (int j = 0; j < 5; ++j)
        {
            // ORIG: fildl 整数槽 → fmuls ratio → fstps → call roundFloat → 存回
            // w[j] = v[2+j] 视角（CItemGenerator.h 匿名 union；ORIG disp=8 折叠）。
            e.w[j] = static_cast<int>(roundFloat(static_cast<float>(e.w[j]) * ratio));
        }
    }
