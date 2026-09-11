// [b28d-solo 2026-09-11 · 新口径 n1_v.py] ORIG 0x085c421e | DIFF orig 328 / our 328（gap 0；
// 本轮由旧口径 328/340 推进 13 条；旧 game_func_report 口径写 our 327）。负条件 goto 复测：
// DIFF 328/332（gap −4，帧 0x13c）⇒ 维持回滚。
// ① 语义轴真修 2 处：kind2/kind3 映射 —— ORIG 0x085c427c / 0x085c45e4 直证
//    `movl $0x0; cmp $0xffffffff,%eax; je →1; test %eax,%eax; jne →2`（Ghidra 同判
//    `(kind == -1) || (kind == 0) ? 1 : 2`），原码误作 `-1 → 0` ⇒ givePvPSkillTree /
//    SkillInitialize 的第三实参在 kind==SKILL_TREE_NONE 时错值（运行行为可见）。
// ② 消指令（P0/P329/P183）：PvP 分支 5 个实参局部全内联（ORIG `mov 0x14(%eax),%edi`
//    + grade→%esi / second→-0x110 / first→%edi / job→%ebx / script 直传）、B 分支
//    ch/second/first/expertJob/level/skillIdx/vector 全内联、`+0x14` 具名化为
//    m_characList（disp 折叠）、主循环补回 ORIG 两级具名拷贝（a/b，+4 条）、
//    got 消费侧配对 bool（P248/A06）。
// 残余 1 条 = ORIG `movzbl; xor $1; test; jne`（4 条）vs our `cmpb $1; jne`（2 条）的
// bool 条件发射差。已试并回滚：`if (!got) goto SkipGiveSkill;` 负条件形 ⇒ our 331、
// 帧 0x12c→0x13c（反劣）；`if (got == 1)` 为 4.4 直产形 ⇒ 封顶。
void CPremiumLetheManager::InitSkill(CUser* user, bool flag,
                                     ENUM_SKILL_TREE_KIND kind)
{
    // [L16 round3] P0/P183：ORIG 段为 `call getCurCharacSkillW; mov 0x14(%ebp),%edx;
    // mov %edx,0x4(%esp); mov %eax,(%esp); call clear_all_skills`（返回值 %eax 直传、
    // 无具名 slot 槽往返）⇒ 删具名缓存、逐点链式重取。
    ((SkillSlot*)user->getCurCharacSkillW())->clear_all_skills(kind);
    ((SkillSlot*)user->getCurCharacSkillW())->set_parent(user);
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        // ORIG 0x085c427c：`movl $0x0,-0x28(%ebp); cmp $0xffffffff,%eax; je →1`;
        // `test %eax,%eax; jne →2` ⇒ `kind==NONE(-1) || kind==1(0)` → 1，其余 → 2
        // （Ghidra 同判：`(param_4 == -1) || (param_4 == 0) ? 1 : 2`）。
        int kind2 = 0;
        if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
            kind2 = 1;
        else
            kind2 = 2;
        user->givePvPSkillTree(0, true, kind2);
        // [b28d-solo] P0/P329：ORIG 0x085c42cf..0x085c432b 五处实参表达式全内联、
        // 右→左求值并各自保活（grade→%esi / second→-0x110 槽 / first→%edi /
        // job→%ebx / script→`G_CDataManager()+0xa800` 直传），无任何具名局部。
        int point = G_CDataManager()->m_pvpSkillTree.getPvPSkillPoint(
            user->get_charac_job(),
            (signed char)user->getCurCharFirstGrowType(),
            (signed char)user->getCurCharSecondGrowType(),
            user->get_pvp_grade(), false);
        ((SkillSlot*)user->getCurCharacSkillW())->set_remain_sp_at_index(point, kind);
        if (flag)
        {
            user->send_skill_info();
            const char* name = user->getCurCharacName();
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0xb1, 0);
            trace("ONE_DAY_LETHE : SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
                  name);
        }
    }
    else
    {
        if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
        {
            addSkillOnCreateCharacter(
                (_Mastered_skill*)(user->getCurCharacSkillW() + 0x46),
                user->get_charac_job());
        }
        else
        {
            addSkillOnCreateCharacter(
                (_Mastered_skill*)(user->getCurCharacSkillW() + 0x1de),
                user->get_charac_job());
        }
        std::vector<std::pair<int, int> > vec;
        std::pair<int, int> p;
        // [b28d-solo] P0/P329：ORIG 0x085c442f..0x085c4481 三个实参全内联——
        // second→%esi、first→%ebx、receiver base 由 `G_CDataManager()+0x14`
        // 载入 %edi 后 `imul $0x7dc` + `lea` 直传（无 ch/second/first 具名槽）。
        // [b28d-solo] ① 裸偏移 `+0x14` 具名化为 `m_characList`（ORIG 0x085c4459
        // `mov 0x14(%eax),%edi` 单条 disp 折叠；裸偏移写法 4.4 多一条 `add $0x14`）；
        // ② 消费侧与 D06 已 bool 化的 `CCharacter::get_give_skill` 配对（P248/A06
        // 净账必配对律）：`bool got` 直存 1 字节槽（ORIG `mov %al,-0x31(%ebp)`）。
        // 注：CCharacList 在 CDataManager.h 仅前向声明（不完全类型不可取下标），
        // 故按 `job*0x7dc` 步长做指针算术（等价 `&m_characList->m_characs[job]`）。
        bool got = ((CCharacter*)((char*)G_CDataManager()->m_characList +
                                  user->get_charac_job() * 0x7dc))
                       ->get_give_skill(
                           (signed char)user->getCurCharFirstGrowType(),
                           (signed char)user->getCurCharSecondGrowType(), vec, 0);
        // ORIG 0x085c4486..0x085c4492：`movzbl -0x31(%ebp),%eax; xor $0x1,%eax;
        // test %al,%al; jne <vec dtor>` = bool 消费侧（P292/P174）的负极性条件。
        // [已试·回滚] `if (!got) goto SkipGiveSkill;` 负条件直跳：our 327→331
        // （帧 0x12c→0x13c，+6 EH/布局条），劣化 ⇒ 保留 4.4 直产 `cmpb $1; jne`。
        if (got == 1)
        {
            int i;
            for (i = 0; i < (int)vec.size(); ++i)
            {
                p = vec[i];
                // ORIG 0x085c44b8..0x085c44c7：`p`(-0x70/-0x6c) 之后另有两次
                // 具名 int 落槽（-0x3c/-0x38）再从槽装载实参 ⇒ 源码为两个具名
                // int 局部（4.4 若直传 p.first 会省掉这级拷贝，少 4 条）。
                int a = p.first;
                int b = p.second;
                ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                    user->get_charac_job(), a, b, kind);
            }
            // [b28d-solo] P0/P329：ORIG 0x085c451c..0x085c45d0 中 expertJob/level/
            // skillIdx/vector 指针全为内联右值（expertJob→%ebx、level→%esi、
            // skillIdx→%edi、vector 每处重发 `script+0xc`，无具名槽）；
            // 仅 `script` 具名（ORIG -0x30）。实参右→左求值序亦逐条对应。
            void* script = G_CDataManager()->GetExpertJobScript(
                user->GetCurCharacExpertJobType());
            if (script != 0)
            {
                unsigned int i2;
                for (i2 = 0;
                     i2 < ((std::vector<std::pair<int, int> >*)((char*)script + 0xc))->size();
                     ++i2)
                {
                    ((SkillSlot*)user->getCurCharacSkillW())->growtype_skill(
                        user->get_charac_job(),
                        (*(std::vector<std::pair<int, int> >*)((char*)script + 0xc))[i2].first,
                        user->GetCurExpertJobLevel(
                            user->GetCurCharacExpertJobExp()),
                        kind);
                }
            }
            WongWork::CSkillChanger changer;
            // ORIG 0x085c45e4：与 kind2 同形（`-0x2c` 槽）⇒ NONE(-1)/1(0) → 1，其余 → 2。
            int kind3 = 0;
            if (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
                kind3 = 1;
            else
                kind3 = 2;
            changer.SkillInitialize(user, kind3, false);
            if (flag)
            {
                user->send_skill_info();
                const char* name = user->getCurCharacName();
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0xf7, 0);
                trace("ONE_DAY_LETHE : SUCCESS_INIT_SKILL and SEND_SKILL_INFO, char(%s)",
                      name);
            }
        }
    }
}