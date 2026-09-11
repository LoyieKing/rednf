// [b28d-solo-PvP_Room] N100 已解（Main 代落 P216）：MatchingSystem.cpp:33 + PvPTypes.h:250
// 给 CMatchingSystem::CMatchingSystem() 加 throw() ⇒ 4.4 判 nothrow 不布 landing，
// EXTRA _ZdlPv 消失（116 vs 130）。本轮再把 fill 实参改 `&m_field22c[0]/&m_field22c[15]`
// ⇒ 复现 ORIG 单条 `lea 0x268(%eax),%ecx`（`m_field22c + 15` 形 4.4 拆 add+lea），
// 116 vs 130 → 116 vs 119。残余 3 条：P211（m_matching int 字段 + call 返回值
// (int) 强转 ⇒ RHS-first，ORIG LHS-first 直存）+ N121 中继 jmp ×2；P167 CLEAR、
// 语义全等。
// [L10b 2026-09-10] P211 残差**已解**：`CMatchingSystem::GetMatchingSystem` 返回类型
// void*→int（PvPTypes.h/MatchingSystem.cpp 同步，mangled 不变）⇒ 调用点复现 ORIG
// 0x085d610a/0x085d610d `mov 0x8(%ebp),%edx; mov %eax,0x6e4(%edx)` LHS-first 直存，
// 本件 119→118（ORIG 116）。残余 = N104 尾块相对序 + N121 中继 jmp ×2。
PvP_Room::PvP_Room()
{
    int local_14 = 0;
    std::fill<int*, int>(&m_field22c[0], &m_field22c[15], local_14);
    for (int local_10 = 0; local_10 < 8; local_10 = local_10 + 1)
    {
        P_SEATS(local_10) = 0;
    }
    m_pMatchingSystem = new CMatchingSystem;
    // [L10b P211 档③] GetMatchingSystem 返回类型已 int 化 ⇒ 删冗余 (int) cast，
    // 复现 ORIG 0x085d610d `mov %eax,0x6e4(%edx)` LHS-first 直存。
    m_matching = m_pMatchingSystem->GetMatchingSystem(ENUM_PVP_MATCHING_TYPE_0);
    m_field61c = 0;
}