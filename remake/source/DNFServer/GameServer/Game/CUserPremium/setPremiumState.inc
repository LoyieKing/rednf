// [b28d-solo-CUserPremium] N107/N7 封顶（b28d 复测；旧注释地址 0x0830ed48
// 系误录，权威地址 nm：0x086b084c）。ORIG 14 条：前两条 `mov 0xc(%ebp),%edx;
// mov 0x8(%ebp),%ecx`（type 先载/this 后载），尾段 `lea (%ecx,%eax,1),%eax;
// lea 0x10(%eax),%edx; mov 0x10(%ebp),%eax; mov %eax,(%edx)`。
// 指针算术形（下方实现）13 条：4.4 折 `lea (%ecx,%eax,1),%edx` + disp store
// `mov %eax,0x10(%edx)`，且 this 先载。
// 本波穷举读数：①`m_entries[(int)type]` 下标形 16 条（push %ebx）；
// ②`*(&m_entries[(int)type].m_state)` 取址形 16 条（push %ebx + add $0x10）；
// ③`((int*)(m_entries+(int)type))[4]` 整型数组视图 15 条（add $0x10 + this
// 中转）——均劣于指针算术形。两段 lea（N107：4.4 对成员常量偏移恒折
// disp/add）+ 形参装载序（4.4 this-first vs 4.1 type-first）双封顶；
// P167 CLEAR、gap 1/14 ⇒ 语义等价。
// 本波复测：clean/AE ndiff=7、14 vs 13（唯一缺口 = ORIG `lea 0x10(%eax),%edx;
// mov %eax,(%edx)` 两段取址 vs 4.4 折 disp），N107 六形探针结论不变 ⇒ 维持封顶。
// [L3CUserPremDBInven 本波实测（ORIG 0x086b084c）] 追加第 5 形：去 `(int)state`
//   cast（`->m_state = state`，enum 同型直赋）——实测汇编逐条不变（同 13 条、
//   同 `lea (%ecx,%eax,1),%edx; mov %eax,0x10(%edx)`）⇒ 回滚；N107 维持（P167
//   CLEAR、per-字段语义等价、gap 1/14）。
void CUserPremium::setPremiumState(ENUM_PREMIUM_TYPE type, ENUM_PREMIUM_STATE state)
{
    (m_entries + (int)type)->m_state = (int)state;
}