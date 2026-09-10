int CBingo::resetQuiz(int day)
{
    // [b28d-7] P171 原码死分支照抄：ORIG 0x080caa6d `test; jg BODY; mov day;
    // cmp $0x1f; jle BODY; mov$0` = `day<1 && day>0x1f`（永假）。旧版德摩根
    // 改写引入真语义偏差，已修正为原码形。
    if ((day < 1) && (day > 0x1f))
    {
        return 0;
    }
    // [b28d-7] P183/P236：ORIG 0x080caa85 每处完整重读 m_map0（lea 0xb568(%eax) 直入参）。
    std::map<int, int>::iterator it = G_CDataManager()->m_bingo.m_map0.find(day);
    // [b28d-7] ORIG `je SKIP_RET0; mov$0; jmp END; SKIP_RET0: 取值` ⇒ 早返 0 形。
    if (it == G_CDataManager()->m_bingo.m_map0.end())
    {
        return 0;
    }
    m_fieldC = it->second;
    return 1;
