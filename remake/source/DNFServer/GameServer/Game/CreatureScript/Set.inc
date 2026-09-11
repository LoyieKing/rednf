// ORIG 0x833c8e8：从原生 CreatureScript* 拷贝字段。
// ORIG 0x833c8e8：从原生 CreatureScript* 拷贝字段。
// ORIG 无 src==0 检查（0x833c8e8 直接 _Znaj 后访问 src）；flags 以 0x833c90b
// 循环读（mov 0xc(%ebp),%eax; mov i,%ecx; add $0x14,%ecx; mov (%eax,%ecx,4),%eax
// = src + 4*(i+0x14) = src+0x50+4i，即 m_artifactSlotRed/Blue/Green 组）。
void CCreatureScript::Set(CreatureScript* src)
{
    // [b28d-solo-CreatureScript] 已修至 **NEAR（144=144 助记符全等）**：三处
    // 语义交换全清——m_evolutionMultyId 逐元素 size/ix/push_back（0x0833c955 段）、
    // m_eventEvlouteCreature 整体 operator=（0x0833caba 段）、m_usingExpItem 源字段
    // bool 化直传（0x0833ca53 段）。唯一残差 = flags 循环 RHS 的 base/index 装载序
    // （ORIG `mov i,%ecx; mov src,%eax` vs 4.4 恒 base 先 `mov src,%eax; mov i,%ecx`）
    // ⇒ N82/P75 封顶（探针 /tmp/probe_ord*.cpp r1-r9 九形均 base-first）。
    m_artifactSlotFlags = new bool[3];
    for (int i = 0; i < 3; ++i)
    {
        // [b28d-solo] P30 命中形：ORIG 0x0833c916 为 scaled index
        // （`add $0x14,%ecx; mov (%eax,%ecx,4),%eax`，基址无 disp）。
        // 4.4 仅「数组指针视图 `(*(int(*)[N])src)[i+K]`」保留该形
        // （int* 下标 [i+K] 恒折 shl+add，探针 /tmp/probe_idx2.cpp q2 实证）；
        // K=0x14 ⇒ +0x50 的 artifactSlotRed/Blue/Green 三连 int。
        typedef int SlotArray[0x100];
        m_artifactSlotFlags[i] =
            (*(SlotArray*)src)[i + 0x14] != 0;
    }

    m_field0 = src->m_evolutionQuestId;
    m_field4 = src->m_evolutionCreatureId;

    // [b28d-solo] ORIG 0x0833c955 段：m_evolutionMultyId **逐元素 size/ix/
    // push_back** 拷入 m_vec8（call size/operator[]/push_back 三连，槽 -0x1c/-0x10）。
    int nMulti = (int)src->m_evolutionMultyId.size();
    for (int i = 0; i < nMulti; ++i)
    {
        m_vec8.push_back(src->m_evolutionMultyId[i]);
    }

    m_field14 = src->m_evolutionLevel;
    m_field18 = src->m_maxLevel;
    m_field24 = src->m_field1c;
    m_field28 = src->m_field20;
    m_field2c = (unsigned char)(src->m_learnOverSkillLevel & 0xff);

    int nSet = (int)src->m_itemEvolCreatureIds.size();
    for (int i = 0; i < nSet; ++i)
    {
        m_set34.insert(src->m_itemEvolCreatureIds[i].m_creatureId);
    }

    m_field4c = src->m_selectiveGrow.m_usingExpItem;
    m_field50 = src->m_selectiveGrow.m_growLevel;
    m_field54 = src->m_selectiveGrow.m_growATypeCreature;
    m_field58 = src->m_selectiveGrow.m_growBTypeCreature;
    m_field5c = src->m_selectiveGrow.m_growATypeItem;
    m_field60 = src->m_selectiveGrow.m_growBTypeItem;
    m_field64 = src->m_selectiveGrow.m_growItemCount;

    // [b28d-solo] ORIG 0x0833caba 段：m_eventEvlouteCreature 走 **vector 整体
    // operator=**（`_ZNSt6vectorIiSaIiEEaSERKS1_` 单调用），无循环。
    m_vec68 = src->m_eventEvlouteCreature;
}