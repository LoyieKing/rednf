bool StrikerScript::checkSkillIndex(short firstGrowType, short secondGrowType,
                                    int skillIndex)
{
    // b28d-36（修法，P212/P191）：ORIG 0x08a9e8xx 函数头连发 `movl $0x0,-0x10;
    // movl $0x0,-0xc` ⇒ vec/pass 均**全头部声明**且各带零初始化（vec 深 -0x10、
    // pass 浅 -0xc，4.1/4.4 全头部同为「先声明深」）；旧形把 vec 放 for 块内
    // 使其占 -0xc、pass 反占 -0x10（槽互换 + 帧 0x48→0x38）。
    std::vector<StrikerSkill>* vec = 0;
    int pass = 0;
    // b28d-solo-LC_Misc 判词 [AE]（修法，P25/P28；旧 b28d-36 N82/P112 帧差判词
    // 作废）：ORIG 尾部 `cmpl $0x1,-0xc; setle %al; test %al,%al; jne` + 入口
    // `jmp <cond>` = 4.1 while 形（for 恒 `cmpl; jle` 直跳、无 setcc 物化），且
    // 增量段调 `ppEi`（后置 `it++` 带独立 sret 临时槽，P28 增量侧推论；原 `++it`
    // 走 `ppEv` 且是 ORIG 无的符号）。原 for 头 `pass = 0` 另生一条重复
    // `movl $0,-0xc`。改 while + it++ 后帧 0x38→0x48、形参 spill 槽与迭代器
    // 槽组同 ORIG（-0x2c/-0x30、-0x1c/-0x18/-0x14），p167 7=7 CLEAR。
    while (pass <= 1)
    {
        if (pass == 0)
            vec = getStrikerSkillVector(firstGrowType, secondGrowType);
        else
            vec = getStrikerSkillVector(firstGrowType, 0);
        if (vec == 0)
            return false;

        for (std::vector<StrikerSkill>::iterator it = vec->begin();
             it != vec->end(); it++)
        {
            if (it->m_field00 == skillIndex)
                return true;
        }
        ++pass;
    }
    return false;
