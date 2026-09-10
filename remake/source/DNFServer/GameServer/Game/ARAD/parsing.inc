int ExceptionItem::parsing(char* data)
{
    Arad_Script script;
    if (!script.Load(data, "material_exception_item"))
        return 0x27;
    if (script.GetRowNum() == 0)
        return 0x2a;
    // ORIG 循环每轮重调 GetRowNum
    // b28d-60：P283 4.1「begin 后无条件 jmp 到底部条件块」布局——原 while(true)+
    // break 使 4.4 条件置顶（setle/jne end）；改 goto 尾标签复现 setg/jne body。
    int r = 0;
    int idx;   // b28d-solo-ARAD：P212 全头部声明（槽序试探）
    goto check;
loop_body:
    {
        idx = script.GetInt(r, "material_exception_item_index");
        // b28d-solo-ARAD：具名 sret 槽试探（ORIG insert sret 在 -0x28、.second -0x24）。
        std::pair<std::map<int, int>::iterator, bool> res =
            m_exceptionItemIndex.insert(std::make_pair(idx, idx));
        if (!res.second)
            return 0x33;   // ORIG insert 失败（.second==0）→ 0x33
        ++r;
    }
check:
    if (script.GetRowNum() > r)
        goto loop_body;
    return 0;
