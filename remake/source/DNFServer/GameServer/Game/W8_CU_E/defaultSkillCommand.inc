// ---------------------------------------------------------------------------
// 4. bool CUser::defaultSkillCommand() @0x086550ee
bool CUser::defaultSkillCommand()
{
    // ORIG 0x086550ee：R=(cmds==0||empty) 常量岛 mov$1/mov$0 落 eax 直测
    // （test %al; je body），无具名槽；空/无 → return false，非空 →
    // clear+enableSaveSkill+return true（P56 岛形直测）。
    std::map<int, STSkillCommandData>* cmds =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(
            const_cast<char*>(getSkillCommand()));
    // N96 清绑：原 `register bool emptyOrNone asm("eax")` 已删。ORIG 该 bool 无槽
    // （cmpl $0,cmds;je L1;call empty;test %al;je L2;L1:mov$1;L2:mov$0;test %al;je）
    // ⇒ 按 P226扩展/P316① 用语句表达式包住短路条件。清绑前后 30=30、与 ORIG 逐条
    // 全等（仅 .o 重定位地址差）⇒ 残差 0；具名 bool 局部形为 31（+1 槽物化）。
    if (({ (cmds == 0) || cmds->empty(); }))
        return false;
    cmds->clear();
    enableSaveSkill();
    return true;
}