// ORIG _ZN13user_creature12CCreatureMgr11ItemEvoluteEi T 0x0833d6c2（ItemEvolute.md）：
// 装备宠存在则**直通透传** CCreature::ItemEvolute(m_user, itemIdx) 的 %eax
// （call 后无 test/movzbl，直接 jmp 尾 leave;ret），否则尾块 mov $0。
// [R11fixE7 2026-09-12] int→bool：ORIG 调用点 0x8228def `call; test %al,%al; je`
// （P292 无 setcc ⇒ bool）+ CCreature::ItemEvolute 侧 ORIG prett 为 `bool` ⇒ 整链
// bool 化（P315）；`return callee();` 在 4.4 下 bool→bool 直通，复现 ORIG 尾形。
bool CCreatureMgr::ItemEvolute(int itemIdx)
{
    if (m_equippedCreature != 0)
        return m_equippedCreature->ItemEvolute(m_user, itemIdx);
    return 0;
}