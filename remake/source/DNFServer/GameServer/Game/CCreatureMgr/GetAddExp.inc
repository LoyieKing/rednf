float CCreature::GetAddExp(int artifactSlot, CUser* user)
{
    float result = 0.0f;
    // ORIG 0x8337732：entry `movl $0,-0x14`（inven 声明初始化 dead store，
    // 4.1 copy-prop 后 call 结果不再回写该槽）。
    CInventory* inven = 0;
    if (IsAvailableArtifact(artifactSlot))
    {
        (void)inven;
        CArtifact* artifact = reinterpret_cast<CInventoryCreatureMgrView*>(
                                  user->getCurCharacInvenW())
                                  ->m_creatureMgr.GetArtifact(artifactSlot);
        if (artifact != 0)
        {
            int minLevel = artifact->GetCreatureMinimumLevel();
            // [b28d-solo-CCreatureMgr P169/P59] ORIG 0x833778a/0x8337795 两条
            // 独立跳转同落赋值块（je/jge 833779c），fall 为早退
            // `mov -0x18(%ebp),%eax; jmp 83377ad` ⇒ 原码是「minLevel!=0 且
            // m_level<minLevel 早退」形；|| 合并形反产 jl 落共享尾（旧判词）。
            if (minLevel != 0)
            {
                if (m_level < minLevel)
                    return result;
            }
            result = artifact->GetExperienceAmount();
        }
    }
    return result;
