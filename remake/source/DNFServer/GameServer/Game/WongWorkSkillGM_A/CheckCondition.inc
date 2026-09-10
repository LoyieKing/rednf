bool CSkillChanger::CheckCondition(const CUser* user)
{
    // ORIG 0x08609d10：get_charac_level ×2、G_CDataManager ×2、
    // getCurCharacExp 两条件各取一次（P48 无 CSE）；结果槽 -0xc/-0x10。
    unsigned int levelExp = static_cast<unsigned int>(
        G_CDataManager()->get_level_exp(user->get_charac_level()));
    unsigned int nextLevelExp = static_cast<unsigned int>(
        G_CDataManager()->get_level_exp(user->get_charac_level() + 1));
    // 双物化岛：bVar1 = (c1||c2) 经 mov$1/mov$0，再 `return !bVar1` 二次物化
    // （+0x76 test %al;je / +0x7a mov$0 / +0x81 mov$1）。
    // N96 清绑：原 `register bool b __asm__("al")` 已删。ORIG b 无槽 ⇒ 按
    // P226扩展/P316① 用语句表达式包住短路条件。清绑前后 44=44、与 ORIG 逐条全等
    // （仅 .o 重定位地址差）⇒ 残差 0；具名 bool 局部形为 45（+1 槽物化）。
    if (({ (user->getCurCharacExp() < levelExp)
            || (nextLevelExp < user->getCurCharacExp()); }))
        return false;
    return true;
