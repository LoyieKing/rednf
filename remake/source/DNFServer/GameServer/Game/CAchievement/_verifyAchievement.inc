// [b28d-solo-CAchievement N95/N82 封顶] ORIG 0x0828d092 非 static 成员：
// loadData 0x0828c956 压 this + in->m_saveTime 两实参，体内 0x8(%ebp)=this
// ⇒ 解引用 +8=m_user。旧 D68「static + self 参数」版把 m_saveTime 当 this
// 解引用 = 真 UB，已修（真语义 bug）。
// ①删 ach 具名局部：ORIG 每处重读 0x8(%ebp)（P0 表达式直用）；
// ②删 list 引用缓存：ORIG 内层条件与体各自 `it->second`（每轮 2 次
//   operator->() const，P157）；③empty()→size()!=0（ORIG 0x0808e1c0
//   `_ZNKSt6vectorIiSaIiEE4sizeEv`），quest 改 const Quest* 使
//   m_triggerData[0] 走 const operator[]（ORIG 0x080ea574）；
// ④special bool 改内联复合条件（ORIG mov $1/$0 物化岛）；
// ⑤删 level 缓存：ORIG 两臂各自 get_charac_level()（P48）；
// ⑥isClearMission 实参 0x83/0x87/0x19c0 用具名局部（ORIG -0x48/-0x44/-0x1c）。
// [残差，2026-09-09 复测] 首 continue 改正向包裹后 185=185、岛数与 ORIG 逐点
// 对齐（P223：平铺三 continue 时 4.4 把 achievementIdx==-1 也岛化，多 2 条）。
// 余下残差 = ①槽位前段整体深 8（it -0x40 vs -0x38，4.4 恒预留 outgoing 对齐区
// 的版本常数 N115/N126）②实参先于 this 装载（ORIG `mov -0x2c(%ebp),%ebx;
// call getCurCharacQuestR; add $0x4,%eax` vs 4.4 后读）③N107 成员偏移取址
// add-vs-lea。P277 具名 receiver 形实测更劣（槽再深 4）⇒ 保持当前形。
// [L5cBattleMonAch 2026-09-11] ④resetIds 移到 if 之外（ORIG 0x0828d235/0x0828d23c
// 两条 movl 在 get_charac_level 调用**之前**无条件发射；原写在 if 内使 4.4 把
// 初始化排在等级判断之后）⇒ 发射序与 ORIG 逐条对齐、our_insns 186→185=ORIG。
// 余差仍为 ①②③（N82/N95 槽基 +8 平移 + P75 实参装载时刻），N 封顶；语义等价
// （p167 calls 全等，仅寄存器/槽号差）。
void CAchievement::_verifyAchievement(unsigned int self)
{
    // ORIG 0x828d09e：不缓存 elements 引用，begin 一次、end 每次循环重新取
    std::map<int, std::vector<stTitleElement> >::iterator it =
        GetTitleElements(G_CDataManager()).begin();
    unsigned int achievementIdx = 0;
    unsigned int clearQuestIdx = 0;
    // ORIG 0x0828d0bd..0x0828d0d2：-0x28/-0x24/-0x20 = 0,0,4 三个死局部（后续无读，
    // P171 死写入照抄；仅复现帧槽布局与 5 条初始化序列）。
    unsigned int unusedA = 0;
    unsigned int unusedB = 0;
    unsigned int unusedC = 4;
    for (; it != GetTitleElements(G_CDataManager()).end(); ++it)
    {
        for (unsigned int i = 0; i < it->second.size(); ++i)
        {
            achievementIdx = (unsigned int)it->second[i].m_achievementIdx;
            // ORIG 0x0828d111 `je +0x139` 直接落循环尾 addl（无岛）；另两个
            // continue 才各带独立 nop 岛（+0x135/+0x138）。平铺三 continue 在
            // 4.4 恒岛化三个 ⇒ 首条件改正向包裹，让 4.4 直落循环尾（P223）。
            if (achievementIdx != 0xffffffff)
            {
                const Quest* quest =
                    G_CDataManager()->find_quest((int)achievementIdx);
                if (quest == 0)
                {
                    continue;
                }
                if (!((quest->m_type == 7) && (quest->m_triggerType == 0xc) &&
                      (quest->m_triggerData.size() != 0)))
                {
                    continue;
                }
                clearQuestIdx = (unsigned int)quest->m_triggerData[0];
                // ORIG 0x0828d1ca：getCurCharacQuestR() + 4 为 WongWork::CQuestClear
                // 子对象（UserQuest 容器 +0 m_user / +4 CQuestClear），调 isClearedQuest。
                // [b28d-solo N95/N82 封顶] ORIG 实参先于 this 装载（`mov -0x2c(%ebp),%ebx;
                // call getCurCharacQuestR; add $0x4,%eax; mov %ebx,0x4(%esp)`）+ 槽基比
                // 我方浅 8（it -0x38 vs -0x40）；P277 具名 receiver 形实测更劣
                //（槽再深 4、内层 i 由 -0x14 漂到 -0x18），已回退。
                if (((WongWork::CQuestClear*)((unsigned int)m_user
                                                  ->getCurCharacQuestR() +
                                              4))
                        ->isClearedQuest(clearQuestIdx))
                {
                    _forceClear(achievementIdx);
                }
            }
        }
    }

    // ORIG 0x0828d235/0x0828d23c：resetIds 两条 movl 在 get_charac_level 调用**之前**
    // 发射（无条件初始化）⇒ 数组声明在 if 之外。
    const unsigned int resetIds[2] = { 0x1983, 0x198b };
    if (m_user->get_charac_level() <= 0x45)
    {
        for (int i = 0; i <= 1; ++i)
        {
            _forceReset(resetIds[i]);
        }
    }
    else if (m_user->get_charac_level() > 0x45)
    {
        _forceClear(0x1983);
    }

    int missionA = 0x83;
    int missionB = 0x87;
    unsigned int clearId = 0x19c0;
    CMissionList_Charac* missionList = (CMissionList_Charac*)
        m_user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    if (missionList->isClearMission(missionA) ||
        missionList->isClearMission(missionB))
    {
        _forceClear(clearId);
    }
}