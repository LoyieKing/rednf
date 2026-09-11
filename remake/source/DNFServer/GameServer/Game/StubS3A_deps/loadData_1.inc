// [b28d-62] 修：memcpy→movzwl 直读（P167），去 memcpy call。ORIG 0x08545202
// [b28d-63 本 TU] P0/P48：ORIG 0x0854521a 把 pData 拷入具名局部 -0x10（源码有 `char* buf`），
//   循环体对源**两次独立直读**（if 与 else 各一次，无 v 槽）；改为视图 scaled 索引
//   `0x4(%eax,%edx,2)` 复现 ORIG 寻址（原 add 链 + v 槽 = 我方多 2 条）。
// [b28d-63 判词] DIFF→**NEAR 37=37**（gap 1→0）：视图字段 + `(short)` 比较点 cast
//   复现 `movzwl; cmp $0x3e7,%ax; jle`，else 直落 %ecx；唯一残差 = _reset 虚调
//   `call *%eax` vs ORIG `call *%edx`（N7）。
bool CItemMakingSkill::loadData(CUser* pUser, char* pData)
{
    (void)pUser;
    _reset();   // ORIG 0x08545218：虚调 vtable slot7
    // ORIG 0x0854521a 槽 -0x10 存 pData 指针；具名为视图指针以复现 `i` 先于基址装载。
    const ItemMakingSkillReqView* buf =
        reinterpret_cast<const ItemMakingSkillReqView*>(pData);
    for (int i = 0; i <= 0xa; ++i)
    {
        // ORIG 0x08545229：两次独立直读（P48）。
        // (short) cast 复现 ORIG 有符号 16 位比较（0x08545234 `cmp $0x3e7,%ax; jle`）。
        if ((short)buf->m_skills[i] > 0x3e7)
            m_skillLevels[i] = 0x3e8;
        else
            m_skillLevels[i] = buf->m_skills[i];
    }
    return true;
}