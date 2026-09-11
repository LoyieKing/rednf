// ---- CheckUsableStackableItem（报告，ORIG T 0x085d18e2 sz 0x96）----
// [IG1 2026-09-10 S2 授权] bool 化：ORIG 调用点 xor $1/test/je 直证返回 bool
// （P248 档①/P292；返回类型不入 mangled、不改布局；定义侧机器码/尺寸 0x97 实测不变）。
bool CAlchemist::CheckUsableStackableItem(CUser* user, CItem const* item)
{
    // ORIG：CStackableItem::get_sub_type()==2（APC 强力药水）时检查全队
    // GetUseAPCPotionTime 未到者存在则禁用；其余放行。
    if (static_cast<const CStackableItem*>(item)->get_sub_type() == 2)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = party->get_user(i);
                if (member != 0)
                {
                    // b28：清 register asm 绑定（N96/P0 全域禁），直测内联。
                    if (member->GetUseAPCPotionTime() >
                        GlobalData::s_systemTime_.getCurSec())
                        return false;
                }
            }
        }
    }
    return true;
}