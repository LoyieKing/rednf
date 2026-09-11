// ORIG execute 0x084a9900。
// b28d-21: 已修 P157/P183 — 两次 growtype_skill 调用点各自重取 skillW/curJob
// 并重发 operator[]（删 skillIdx/skillLevel/curJob/skillW 四个具名局部）⇒
// p167 调用点差集 CLEAR（39=39，原 MISSING 6）；ndiff 108→54。余 N82 帧/槽
// （178 vs 182）。ORIG 0x084a9900。
void CChangeExpertJobCmd::execute()
{
    // b28d-22：P0/P48 — ORIG 0x084a9900 无 jobType 槽，两次独立直读 m_jobType
    // （各自入 %ebx 跨 call 保活）。
    GetUser()->SetCurCharacExpertJobType(m_jobType);

    STExpertJobScript* script =
        static_cast<STExpertJobScript*>(G_CDataManager()->GetExpertJobScript(m_jobType));
    if (script != 0)
    {
        for (std::vector<std::pair<int, int> >::size_type i = 0;
             i < script->m_growSkillVec.size(); ++i)
        {
            // b28d-22：P296 — ORIG 用正向包裹（test;setne;test;je 落增量块），
            // 旧 `if(...==0) continue;` 使 4.4 多出 jmp+nop 中继岛（+2 条）。
            if (GetUser()->getCurCharacR() != 0)
            {
                // b28d-21：P167 差集（ORIG 39 calls vs 重建 33）证明 ORIG 在两次
                // growtype_skill 调用点各自重取 skillW/curJob 并重发 operator[]
                // （P157/P183 禁缓存）⇒ 全部内联，删 skillIdx/skillLevel/curJob/skillW
                // 四个具名局部。占位声明返回 char*，ORIG 实为 SkillSlot*。
                reinterpret_cast<SkillSlot*>(GetUser()->getCurCharacSkillW())
                    ->growtype_skill(GetUser()->get_charac_job(),
                                     script->m_growSkillVec[i].first,
                                     script->m_growSkillVec[i].second,
                                     (ENUM_SKILL_TREE_KIND)0);
                reinterpret_cast<SkillSlot*>(GetUser()->getCurCharacSkillW())
                    ->growtype_skill(GetUser()->get_charac_job(),
                                     script->m_growSkillVec[i].first,
                                     script->m_growSkillVec[i].second,
                                     (ENUM_SKILL_TREE_KIND)1);
            }
        }
        // b28d-22：P0/P183 — ORIG 无 expand 具名局部，GetCharacExpandData 结果
        // 直接作 this 调用 InitScript。
        reinterpret_cast<expert_job::CCharacExpertJob*>(
            GetUser()->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0))
            ->InitScript(script);
        GetUser()->SetCurCharacExpertJobExp(0);
    }

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    GetUser()->make_basic_info(reinterpret_cast<char*>(&packet), 0);
    packet.finalize(true);
    G_GameWorld()->send_all(packet, GetUser());
}