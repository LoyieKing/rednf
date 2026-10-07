void Secu_DungeonAverageCheck::setKillMonsterInfo(const MSG_MONSTER_DIE* info,
                                                  map_monster* monster)
{
    // ORIG 0x82794d6。ROUND 为控制字截断（round toward zero），用 (int) 强转等价。
    // [P2S5] 本波战果：gap −6 → **0**（316=316），指令序列逐条同形；语义修正 1 处
    // （ORIG 四处 fistpll+`mov -0x68,%eax; mov -0x64,%edx` ⇒ 原码 double→unsigned
    // 直转，我方旧写 (unsigned int)(int)(…) 产 32 位 fistpl，负数行为不同）。
    // 残余：栈槽整体差 4 字节（我方 it −0x50 / avg1 −0x30 vs ORIG −0x4c / −0x2c，
    // 且 ORIG 独有 −0x44）⇒ 属 GCC 4.4 槽分配/复用细节。已试负结论：
    // ① it 两段式声明（先声明后赋值）→ 322（gap −6，劣化，回滚）；
    // ② 整体嵌套 `if (m_party != 0) {…}` → 315（gap +1，劣化，回滚）。
    // [P2S5] 负结论：ORIG 反编译虽呈 `if (*(int *)this != 0) { …全部… }` 嵌套形，
    // 但实测改嵌套后 316→315（gap +1）⇒ early-return 形才是 316=316 gap 0 正解。
    if (m_party == 0)
        return;
    // [P2S5] ORIG 0x082794e6 形为 `call isNamedMonster; xor $0x1,%eax; test
    // %al,%al; je …` + `movzbl 0x8(%eax),%eax; test; jne …` + `mov $1/$0,%eax;
    // test %al,%al; je <主体>`（bool 物化后 if）⇒ 原码 `if (!(isNamed ||
    // roleType != 0)) { …全部主体… }`，非 early-return（我方早退形少 1 条
    // xor、多 1 条 jmp）。
    if (!(monster->isNamedMonster() || monster->m_roleType != 0))
    {
        // [P2S5] ORIG 0x0827952e/0x0827990c 两处均**重算** `mov 0x10(%ebp),%eax;
        // mov 0xc(%eax),%eax`（无具名 monsterId 槽）⇒ P48 内联表达式。
        std::vector<SecKillMonsterInfo>::iterator it =
            findKillMonsterInfo((unsigned int)monster->m_mobIndex);
        if (it != m_killInfos.end())
        {
            if (4 < (unsigned int)it->m_count)
            {
                // [P2S5] ORIG 0x08279562/0x0827956c 与 0x082795d9/0x082795e3 有
                // `mov $0x0,%eax; mov %eax,<avg槽>`（float 局部 int-0 初始化）⇒
                // 原码为「先 0 初始化、后赋值」两段式；adj1 同样（0x08279627）。
                float avg1 = 0;
                float avg2 = 0;
                avg1 = (float)(unsigned int)it->m_sumValue1 /
                       (float)(unsigned int)it->m_count;
                avg2 = (float)(unsigned int)it->m_sumValue2 /
                       (float)(unsigned int)it->m_count;

                float adj1 = 0;
                if ((int)avg1 < 10)
                    adj1 = 1.0f;
                else
                    adj1 = 0.1f * avg1;

                // [P2S5] ORIG 0x082795f6/0x08279648/0x0827969c/0x082796ef 用
                // `fistpll`（64 位）+ `mov -0x68(%ebp),%eax; mov -0x64(%ebp),%edx`
                // ⇒ 原码是 double→unsigned 直转（非经 (int)，fistpl 为 32 位形）。
                // [bughunt13] ORIG 0x08279660 `flds avg; fadds adj; flds 0.5; faddp` ⇒ (avg+adj)+0.5f
                //   （旧码 (0.5f+avg)+adj 结合序反，扩展精度舍入路径不同）。
                unsigned int upper1 = (unsigned int)(avg1 + adj1 + 0.5f);
                unsigned int lower1 = (unsigned int)(avg1 - adj1);
                float adj2 = 0.05f * avg2;
                unsigned int upper2 = (unsigned int)(avg2 + adj2 + 0.5f);  // ORIG 0x082796a6 同上
                unsigned int lower2 = (unsigned int)(avg2 - adj2);

                if (m_party->m_mapHitCount > 0 &&
                    lower1 <= info->m_killValue1 && info->m_killValue1 <= upper1)
                {
                    it->m_hackCntE6 += 1;
                    for (int i = 0; i < 4; ++i)
                    {
                        // [P2S5] ORIG 0x08279615 `test %al,%al; je <增量块>`（无
                        // xor/jmp）⇒ 正条件 if；0x08279637 `mov %eax,%ebx` ⇒ user 在
                        // 寄存器（register）；getHackAnalyzer 的 this 直接是第二次
                        // get_user 结果（链式，无形参局部）。
                        if (m_party->checkValidUser(i))
                        {
                            register CUser* user = m_party->get_user(i);
                            // [P2S5] ORIG 0x08279658 `mov %eax,(%esp); call`（analyzer
                            // 无具名槽）⇒ 链式直传。
                            m_party->get_user(i)->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0xe6, 1, 0, 0);
                        }
                    }
                }
                if (m_party->m_mapHitCount > 0 &&
                    lower2 <= info->m_killValue2 && info->m_killValue2 <= upper2)
                {
                    it->m_hackCntE7 += 1;
                    for (int i = 0; i < 4; ++i)
                    {
                        if (m_party->checkValidUser(i))
                        {
                            register CUser* user = m_party->get_user(i);
                            m_party->get_user(i)->getHackAnalyzer()->addServerHackCnt(
                                user, (WongWork::ENUM_HACKTYPE)0xe7, 1, 0, 0);
                        }
                    }
                }
            }
            it->m_count += 1;
            it->m_sumValue1 += (int)info->m_killValue1;
            it->m_sumValue2 += (int)info->m_killValue2;
        }
        else
        {
            m_killInfos.push_back(SecKillMonsterInfo(
                (unsigned int)monster->m_mobIndex, 1,
                (unsigned int)info->m_killValue1,
                (unsigned int)info->m_killValue2));
        }
    }
}