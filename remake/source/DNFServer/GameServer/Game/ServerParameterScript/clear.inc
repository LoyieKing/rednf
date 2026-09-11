void ServerParameterScript::clear()
{
    // b28d-solo-ServerParameterScript: DIFF→修（P191 循环变量双初始化 + 复用）
    // ORIG 0x0836f38c 帧 0x28：`movl $0x0,-0x18` 出现两次（函数头 `int i=0;` +
    // 首个 `for (i=0;…)`），此后 6 个一维循环与 0x1a 循环**复用同一 -0x18**；
    // emblem 循环独占 -0x14、二维循环 -0x10/-0xc。旧代码每循环各声明新 i（帧 0x48、
    // 槽 -0x30..-0x1c）。
    m_358 = 0;
    int i = 0;   // ORIG 在 m_358=0 之后（双 movl $0,-0x18 的第二条为 for-init）
    for (i = 0; i < 4; ++i)
    {
        m_versionBytes[i] = 0;
    }
    m_expRate360[0] = 1.0f;
    m_expRate360[1] = 1.0f;
    for (i = 0; i < 4; ++i)
    {
        m_expGroup368[i] = 1.0f;
    }
    for (i = 0; i < 4; ++i)
    {
        m_expGroup378[i] = 1.0f;
    }
    for (i = 0; i < 4; ++i)
    {
        m_expGroup388[i] = 1.0f;
    }
    // ORIG clear 循环 lea 0xe4 disp 8 只复位前 4 个（第 5 个由 import 写入）。
    for (i = 0; i < 4; ++i)
    {
        m_monsterExpTable398[i] = 1.0f;
    }
    for (i = 0; i < 5; ++i)
    {
        m_expBonusWeight3ac[i] = 1.0f;
    }
    m_vec57c.clear();
    m_vec588.clear();
    m_598 = 0;
    m_59c = 0;
    m_assaultLevelPenalty = 0.0f;
    m_float5a4.clear();
    m_508[0] = 0;
    m_508[1] = 0;
    m_508[2] = 0;
    m_508[3] = 0;
    m_508[4] = 0;
    m_508[5] = 0;
    m_508[6] = 0;
    m_508[7] = 0;
    m_508[8] = 0;
    m_508[9] = 0;
    m_508[10] = 0;
    m_508[11] = 0;
    m_508[12] = 0;
    m_508[13] = 0;
    m_508[14] = 0;
    m_508[15] = 0;
    m_508[16] = 0;
    m_508[17] = 0;
    m_508[18] = 0;
    m_508[19] = 0;
    m_558 = -1;
    for (i = 0; i < 0x1a; ++i)
    {
        m_55c[i] = 0;
    }
    m_578 = 0;
    m_burning48c.clear();
    m_luck48.clear();
    m_luckDed54.clear();
    m_luckDed60.clear();
    m_vec6c.clear();
    m_78 = 0;
    m_80 = 0;
    m_7c = 0;
    m_84 = 0;
    m_49e8 = 0;
    m_76c = 0;
    m_map88.clear();
    m_veca0.clear();
    m_ac = 0;
    m_mapb0.clear();
    m_unusableEC.clear();
    m_f8 = 0;
    m_unusableItemFC.clear();
    m_map114.clear();
    m_dungeonInoutc8.clear();
    m_dungeonOpenE0.clear();
    m_bloodReward794.m_vecc.clear();
    m_bloodReward794.m_vec0.clear();
    m_bloodReward794.m_vec18.clear();
    m_7b8 = 0;
    m_7bc = 0;
    for (int k = 0; k < 7; ++k)
    {
        m_emblemCompound[k].m_cost = -1;   // ORIG 0x836f7c0：cost=0xffffffff（EMBLEM 无此等级）
    }
    for (int i = 0; i < 32; ++i)
    {
        for (int j = 0; j < 32; ++j)
        {
            m_arr868[i][j] = 0;
            m_arr1868[i][j] = 0;
        }
    }
    m_49ec = 0;
    m_49f0 = 0;
    m_488c = 0;
    m_4890 = 0;
    m_vec4954.clear();
    m_4960 = 0;
    m_4964 = 1;
    m_4968 = 0;
    m_496c = 0;
    // ORIG 0x836f8f4：0x4970..0x4977 八个独立 movb（非循环）
    m_schoolMatchSeal[0] = 0;
    m_schoolMatchSeal[1] = 0;
    m_schoolMatchSeal[2] = 0;
    m_schoolMatchSeal[3] = 0;
    m_schoolMatchSeal[4] = 0;
    m_schoolMatchSeal[5] = 0;
    m_schoolMatchSeal[6] = 0;
    m_schoolMatchSeal[7] = 0;
    m_schoolMatchDailyPlayLimit = 0;
    m_map497c.clear();
    m_map4994.clear();
    m_pvp49ac.clear();
    m_map49c4.clear();
    m_vec49dc.clear();
    m_chaosWeight4e4.clear();
    m_ghostLevel4f0.clear();
    m_ghostChaos4fc.clear();
    // ORIG 0x836f9cd：clear 末尾 memset(this+0x4bc, 0, 4)
    // （+0x3c0/+0x400/+0x468/+0x480/+0x498/+0x4a8/+0x4c0/+0x4d8/
    //  +0x3c/+0x18/+0x24 在 ORIG clear 中不处理，故不调用其 clear）
    memset(m_weekendBouns4b4.m_dropRoutineByParty, 0, 4);
}