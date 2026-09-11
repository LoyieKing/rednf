// ============================================================================
// CVillageMonsterMgr::OnStartVillageAttacked —— ORIG 0x86b4a20
//   未开战（m_field04 != 1）时创建怪物并置 m_field04=1。
// ============================================================================
void CVillageMonsterMgr::OnStartVillageAttacked()
{
    // [b28-L14] ORIG 0x86b4a20：`cmp $1; je <nop 岛>; body; jmp <leave>` ⇒
    // P223 形 `if (c) goto DONE; body; DONE:;`（旧 L11 N 判词为负例，本轮翻案）。
    if (m_field04 == 1)
        goto DONE;
    OnCreateVillageMonster();
    m_field04 = 1;
DONE:
    ;
}