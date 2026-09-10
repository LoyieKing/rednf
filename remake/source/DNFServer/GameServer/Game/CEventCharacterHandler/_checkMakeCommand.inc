bool CEventCharacterHandler::_checkMakeCommand(CUser* user, int level, int& cmdIdx)
{
    // ORIG 形（P0 律）：两次 getCurSec 均无 now 槽（call 后直接 cmp %eax）；
    // 判断臂 mov$1/mov$0 汇聚；各 early-return 的 fall-through 为 return 块
    // （P137 方向：if(cond>imm) return false;）；growType 双段式（P197③
    // movl $0,-0xc 声明预置 + 独立赋值）；return true 为函数末条 fall-through。
    // 复合短路 || 直接作 if 条件（probe pr5.cpp f4 实证：4.4 出 mov$1/jmp/
    // mov$0 无槽汇聚 + test %al 直测，与 ORIG 判断树逐条同形；具名 bool
    // 赋值则落槽= N3 不可取）。
    if (GlobalData::s_systemTime_.getCurSec() <= 0x4b055d6f ||
        GlobalData::s_systemTime_.getCurSec() >= 0x4b3bbf51)
        return false;
    if (level > 0x46)
        return false;
    int growType = 0;
    growType = user->getCurCharacEventCharacterGrowtype();
    if (growType > 6)
        return false;
    cmdIdx = user->get_charac_job() * 10 + growType;
    return true;
