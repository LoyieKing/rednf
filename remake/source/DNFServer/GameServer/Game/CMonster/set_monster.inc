void CMonster::set_monster(STMonsterScript& script)
{
    // ORIG 0x0834f650：逐字段拷贝（script → this），非整体赋值：
    //   this+0x04 ← script+0x00(m_index)；this+0x08.. ← script+0x28 的 4 个
    //   specialItemLists；this+0x38 ← script+0x88；this+0x3c ← script+0x8c；
    //   this+0x58 ← script+0x3ec；this+0x5c ← script+0x3f0；
    //   this+0x1d24 ← script+0xc4c；this+0x54 ← script+0x3a8(byte)；
    //   this+0x48 ← script+0x58；最后 STMonsterScript::operator=(this+0x60, script)。
    m_index = script.m_index;
    for (int i = 0; i < 4; ++i)
        m_specialItemLists[i] = script.m_field028[i];
    m_specialItemWeight = script.m_field088;
    m_dropItems = script.m_field08c;
    m_independentDropCount = script.m_levelByDiff[0];
    m_catchItemCount = script.m_levelByDiff[1];
    m_weight = script.m_fieldc4c;
    m_deathTowerFlag = script.m_flag3a8;   // ORIG movzbl 0x3a8(%e) → %dl 直存 +0x54（bool 成员单指令 disp）
    m_deathTowerItems = script.m_field058;
    m_script = script;   // 隐式 op=（ORIG W 0x08377994 编译器合成版）
