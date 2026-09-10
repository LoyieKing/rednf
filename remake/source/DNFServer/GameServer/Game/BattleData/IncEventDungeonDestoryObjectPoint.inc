// [b28d-L5a 2026-09-11 落地 + 残差封顶] ORIG 0x085be6e2（71 条，每臂 25 条）：
// push edi/esi/ebx 三 callee-saved；每臂 `movzwl %ax,%esi` + `movzwl %ax,%edi`
// 两次**独立**零扩展（读/写地址各一份），索引链全 add，两处 this 各自装载
// （`mov 0x8(%ebp),%ebx` 读 / `mov 0x8(%ebp),%ecx` 写）。
// 落地形 = P231 纯 `register`（非 asm 绑定、P0 合规）三旋钮：
//  `register BattleData* self = this;` + 两枚 `register int rIdxN = st.m_roomIndex;`
//  ⇒ 三 callee-saved（push edi/esi/ebx）、两次独立零扩展、3p1 全 add、写地址
//  `add %esi,%eax; add $0xc4,%eax` 与 ORIG 逐条同构；**69 vs 71**（原 67）。
// 残差（N18a/lea + 复用族，源形不可择）：
//  ①读地址 `lea (%eax,%edi,1),%ecx` vs ORIG `add %edi,%eax`（4.4 lea 折合，探针
//    /tmp/l5/p1..p6 共 18 形全试不解）；
//  ②4.4 复用 %ebx 承担两处 this ⇒ 每臂少 1 条 `mov 0x8(%ebp),%ecx`；**双 self 形**
//    实测 74 条（+5，超出 ORIG）已回滚（探针/真实 TU 双向复证）。
// 其他形读数（防回退）：无 register `+=` 67；单 register `+=` 63；全写形与 `+=` 逐条同形。
// P167 callsite CLEAR ⇒ 语义等价。
void BattleData::IncEventDungeonDestoryObjectPoint(
    int param_1, const CMDPacketStruct::_STReqEventDungeonDestoryObject& st)
{
    if (st.m_roomIndex >= 3)
    {
        return;
    }
    // b28d-40: P183 去 idx 缓存（ORIG 0x085be711/0x085be761 两臂各一次全量重算索引）。
    if (st.m_destroyType == 1)
    {
        register BattleData* self = this;
        register int rIdx1 = st.m_roomIndex;
        register int rIdx2 = st.m_roomIndex;
        self->m_eventClear[param_1 * 3 + rIdx2].m_monsterIdx =
            (unsigned short)(self->m_eventClear[param_1 * 3 + rIdx1].m_monsterIdx +
                             st.m_point);
    }
    else
    {
        register BattleData* self = this;
        register int rIdx3 = st.m_roomIndex;
        register int rIdx4 = st.m_roomIndex;
        self->m_eventClear[param_1 * 3 + rIdx4].m_roomIdx =
            (unsigned short)(self->m_eventClear[param_1 * 3 + rIdx3].m_roomIdx +
                             st.m_point);
    }
}
