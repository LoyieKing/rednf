// ORIG STSecondGrowTypeData::clear W 0x08367a28（objdump 转写）：双状态块 +
// 双技能表 + 双 etc 域。
void STCharacterScript::STSecondGrowTypeData::clear()
{
    // [b28d-solo-ScriptDepsA 判词] N4/N18a 封顶（35/36，唯一残差 2 条）：
    // ORIG 首元素地址 `imul $0x57; add 0x8(%ebp),%eax`（基址内存加），4.4 恒
    // `mov 0x8(%ebp),%edx; imul; lea (%edx,%eax,1)`（先载 this 再 lea）。
    // 本波探针复验 6 形（单成员指针算术/数组下标/`*ptr`/reinterpret_cast/
    // 数组引用/具名指针缓存）全部产 lea 或加槽，无源形可产 `add mem` ⇒ 封顶。
    for (int i = 0; i < 2; ++i)
    {
        // ORIG imul $0x57 直接寻址 ⇒ 数组下标形（CCharacter.cpp 同款先例）
        m_statusA[i].Clear();
        m_skills[i].clear();
        m_etc[i].clear();
    }
}