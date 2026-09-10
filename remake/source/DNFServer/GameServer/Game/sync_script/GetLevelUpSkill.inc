void CSyncScript::GetLevelUpSkill(
    STEquipmentScript* script,
    EquipmentParameterInfo::ENUM_SKILL_UP_VECTOR_INDEX vectorIndex)
{
    // [b28d-solo-sync_script] 判词：**AE/IDENTICAL**（check_tu routed；178=178）。
    //   归位要点：allocator 改 string ctor 实参处临时（full-expression 末 dtor）、
    //   `compare("")==0`（int 直测+sete）、int 循环变量保 at(i) 值先/add 两步寻址、
    //   m_field8 内联实参升格 %ebx（P0 右值直传）。ORIG @0x0861d472。
    static std::string level_up_skill;
    level_up_skill.clear();
    // ORIG 0x0861d472：无 vectorIndex 值域检查——直接 m_levelUpSkills[idx] 下标
    // 寻址（param_3*0xc 每次 shl 重算，0x40c 基址；idx=2 落 +0x264 即权威头
    // m_avatar264 邻域，ORIG 行为如此，前批“保守 return”系过度防御，已按原码
    // 照抄）。size 落 int 槽、(int)i<n 有符号比较；at(i) 每处独立重读（P48/P157
    // 无 CSE）；每轮 memset(tmp,0,0x400)；job 串经 allocator+string(PKc,alloc)
    // 临时（EH landing pad），非裸 compare。
    int skillCount = (int)script->m_param1c0.m_levelUpSkills[(int)vectorIndex].size();
    // ORIG 0x0861d529：`int i` + 有符号尾测 `setl`；且 at(i) 处 ORIG 为
    // `mov i,%ecx`（值先）+ `add $0xc,%eax` 两步寻址 ⇒ 下标必须是 int 局部
    //（P218：unsigned 下标令 4.4 翻成 this 先 + lea 折叠）。
    for (int i = 0; i < skillCount; ++i)
    {
        level_up_skill += "|";
        // ORIG 0x0861d549..0x0861d5be：allocator 是 string ctor 实参处的**临时**
        // 对象（full-expression 末即 dtor，EH landing 0x861d5a3）；具名局部会把
        // dtor 推迟到循环体末并多出整段 cleanup。
        std::string jobStr(GetUserJob((ENUM_CHARACTERJOB)
            script->m_param1c0.m_levelUpSkills[(int)vectorIndex].at(i).m_job),
            std::allocator<char>());
        // ORIG 0x0861d5d7：`call compare(""); test %eax,%eax; sete %al; test %al,%al`
        //（operator== 形产 bool 直判，缺 sete 段）。
        if (jobStr.compare("") == 0)
        {
            level_up_skill += "\xb0\xf8\xc5\xeb";   // 공용
        }
        else
        {
            level_up_skill += jobStr;
        }
        char tmp[0x400];
        memset(tmp, 0, sizeof(tmp));
        // ORIG 0x0861d62c：m_field8 读入 %ebx 跨第二次 at() call 保活（无槽往返
        // ⇒ P0 右值内联）；具名/register 局部反生 `mov mem,%eax; mov %eax,%ebx`
        // 两段（N18a 向）。
        sprintf(tmp, "| %d| %d",
                script->m_param1c0.m_levelUpSkills[(int)vectorIndex].at(i).m_field4,
                script->m_param1c0.m_levelUpSkills[(int)vectorIndex].at(i).m_field8);
        level_up_skill += tmp;
    }
    level_up_skill.c_str();
