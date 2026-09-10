void GameWorld::GetPowerUpDomain(std::vector<CUser*>& listA, std::vector<CUser*>& listB)
{
    // m_powerParameter(+0xa628) + 0x50
    double threshold =
        (double)(G_CDataManager()->m_powerParameter.m_powerUpAssaultVictoryRatio) / 100.0;
    for (std::map<unsigned short, CUser*>::iterator it = m_UserInWorld.begin();
         it != m_UserInWorld.end(); ++it)
    {
        CUser* user = it->second;
        if (user->isJoinPowerWar())
        {
            // [b28d-solo-GameWorld] _ZN9GameWorld16GetPowerUpDomainERSt6vectorIP5CUserSaIS2_EES5_
            // verdict=DIFF 104/103（b28d-solo 本轮；唯一残差 = x87 比较 lowering 1 处）。
            // 修法：①ORIG 0x086cf274 `test %eax,%eax; sete %al; test; jne 尾` ⇒ ==0 早退；
            // ②**本轮 P248 档①**：ORIG 0x086cf28c/0x086cf2a8 为 `mov $0,%edx;
            // mov %eax,-0x40; mov %edx,-0x3c; fildll`（unsigned 32→double 零扩展 64 位装载）
            // ⇒ 两处 `(double)(unsigned int)call()`（旧 `(double)` 恒产 fildl；`(long long)`
            // 探针无效）；③P252/P243：具名 double assaultCount/victoryCount 按 ORIG 求值序
            // （分母 -0x20 先、分子 -0x18 后，`fldl -0x18; fdivl -0x20`）；④比较操作数序
            // `threshold >= winRate`（ORIG `fldl -0x28(threshold); fldl -0x10`）。
            // 残余 N85：ORIG `fucompp; fnstsw %ax; sahf`（3 条）vs 4.4 `fucomip; fstp`（2 条），
            // 跨版本 x87 比较 lowering；P230 补的 -march=i486 属 TU 级旗标（禁改路由）。
            if (user->GetPowerWarAssaultCount() == 0)
            {
                goto L_NEXT;
            }
            {
                // ORIG 0x086cf28c..0x086cf2bf：分母先 fildll+fstpl -0x20、分子
                // fildll+fstpl -0x18，再 `fldl -0x18; fdivl -0x20` ⇒ 两个具名
                // double 局部按 ORIG 求值序声明（P252 槽往返 + P243 第一操作数）。
                double assaultCount =
                    (double)(unsigned int)user->GetPowerWarAssaultCount();
                double victoryCount =
                    (double)(unsigned int)user->GetPowerWarAssaultVictoryCount();
                double winRate = victoryCount / assaultCount;
                if (threshold >= winRate)
                {
                    if (user->getPowerSide() == 1)
                    {
                        listA.push_back(user);
                    }
                    else
                    {
                        listB.push_back(user);
                    }
                }
            }
        L_NEXT:;
        }
    }
