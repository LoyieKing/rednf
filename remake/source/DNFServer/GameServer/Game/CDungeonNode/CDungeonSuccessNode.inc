// ============================================================================
// CDungeonSuccessNode::CDungeonSuccessNode() —— ORIG T 0x08484dda
// m_partyMap 预插 job 2..4 的 Party；m_jobMap 预插 job 0..0xa × sub 0..5 × grow 0..2
// 的 CSuccessJobNode（JobKey = {job, STGrowthType{sub, grow<<4}}）。
// ============================================================================
CDungeonSuccessNode::CDungeonSuccessNode()
    : m_partyMap(), m_jobMap()
{
    // [b28d-solo-CDungeonNode · N104 封顶 + 位域已修] ORIG 0x08484dda：循环变量 job/g/sub
    // 声明于函数头且先置 0（P191；ORIG -0x214/-0x10/-0xc 三条 movl $0，g/sub 声明序按 ORIG
    // 槽深排定）；两段循环各带原码死比较 `find != end()`（P171 照抄）；make_pair 第二参右值。
    // STGrowthType 位域（m_sub:4/m_grow:3/m_rsv:1）复现 ORIG RMW 掩码 0xfffffff0/0xffffff8f；
    // 底层类型改 signed char 复现 ORIG `shl$4; sar$4,%al` / `shl$5; sar$5,%al` 有符号窄化
    // （STGrowthType.h；探针 /tmp/b28d_s218_bf.cpp w2/w6 实证）⇒ 位域段 4 处 DIFF 已归零。
    // 残差 = N104（两循环 EH landing 相对序：4.1 集中置尾 vs 4.4 就地内联，且 4.4 多发
    //        块间 jmp；块内容逐条全等、槽位集全等、p167 33/33 CLEAR）；262 vs 263。
    int job = 0;
    int g = 0;
    int sub = 0;
    for (job = 2; job <= 4; job++)
    {
        if (m_partyMap.find(job) != m_partyMap.end())
        {
        }
        m_partyMap.insert(std::make_pair(job, Party()));
    }

    JobKey key;
    STGrowthType grow;
    for (job = 0; job <= 0xa; job++)
    {
        key.m_job = (ENUM_CHARACTERJOB)job;   // b28c-36: JobKey::m_job enum 化连带
        for (sub = 0; sub <= 5; sub++)
        {
            grow.m_growthType = 0;
            grow.m_sub = sub;
            for (g = 0; g <= 2; g++)
            {
                grow.m_grow = g;
                key.m_sub = grow.m_growthType;
                if (m_jobMap.find(key) != m_jobMap.end())
                {
                }
                m_jobMap.insert(std::make_pair(key, CSuccessJobNode()));
            }
        }
    }
}