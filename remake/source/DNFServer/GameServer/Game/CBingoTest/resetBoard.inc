 // ORIG 反汇编：if (day>0 && day<=0x1f) 短路布局（jg/jle 同跳 body）。
int CBingo::resetBoard(int day)
{
    // [b28d-7] P171 原码死分支照抄：ORIG `test; jg BODY; mov day; cmp $0x1f;
    // jle BODY; mov$0` = `day<1 && day>0x1f`（永假，永不早退）。旧版德摩根
    // 改写 `!(day>0 && day<=0x1f)` 引入真语义偏差，已修正为原码形。
    if ((day < 1) && (day > 0x1f))
    {
        return 0;
    }
    // [b28d-7] P183/P236：ORIG 0x080ca9xx 每处完整重读 G_CDataManager()->m_bingo.m_map18
    // （lea 0xb580(%eax) 直接入参，无 map 地址局部槽）⇒ 删具名 map 引用。
    std::map<int, std::vector<int> >::iterator it =
        G_CDataManager()->m_bingo.m_map18.find(day);
    // [b28d-7] ORIG `je SKIP_RET0; mov$0; jmp END; SKIP_RET0: 取值` ⇒ 早返 0 形。
    if (it == G_CDataManager()->m_bingo.m_map18.end())
    {
        return 0;
    }
    m_boards.clear();
    m_boards = it->second;
    return 1;
}