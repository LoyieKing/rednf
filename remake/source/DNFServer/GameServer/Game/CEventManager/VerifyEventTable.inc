bool CEventTable::VerifyEventTable(std::vector<int>& out)
{
    // b28d-solo-CEventManager: [N 封顶] ORIG 0x0811a1e0 实证——局部全函数头声明（P212）：
    // ok(-0x1d,=false)/id(-0x48,=0)/name[0xff](-0x16f,={0})/dup(-0x49,=false)/
    // idx(-0x50,=-1)/found map(-0x68)；i 为函数级变量，两个 for 复用（P191 双初始化
    // `int i=0; for(i=0;…)` 两次清零）；fetch 失败 break。name 清零为 4.1 内联
    // rep stos（N78 封顶）。
    // 本轮修：P293 第二循环改具名迭代器 `iterator it = found.find(i); if (it ==
    // found.end())`（ORIG 0x0811a427 先 find sret -0x70 后 end sret -0x28；旧单表达式
    // 形 4.4 反序且令 id/dup/idx/found/i 全组深 4）⇒ 全部槽位归位（-0x48/-0x49/
    // -0x50/-0x68/-0x6c/-0x70/-0x2c/-0x34/-0x3c/-0x44 与 ORIG 逐一对齐）。
    // b28d-solo 本轮修（DIFF→AE）：唯一实质残差 = ORIG 0x0811a395 `mov -0x50(%ebp),
    // %eax; not %eax; shr $0x1f,%eax; test %al,%al; je`（4.1 对 `idx >= 0` 的寄存器
    // bool 物化）vs 4.4 `test %eax,%eax; js` 直跳。旧 `(unsigned int)~idx >> 31`
    // 表达式形被折叠（无效）；**改 `while (idx >= 0) { …; break; }` 后 4.4 精确复现
    // not/shr/test/je 四段**（P225 推广：while+break 对 4.1 寄存器 bool 物化同样有效，
    // 不只 setne 族），217→220 与 ORIG 219 在 compare_tu clean 口径下 AE（数据地址
    // 伪差豁免，P264）。槽位/帧 0x17c 与 ORIG 全等。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)1, E_SERVER_GROUP_NONE);
    bool ok = false;
    int id = 0;
    char name[0xff] = {0};
    bool dup = false;
    int idx = -1;
    std::map<int, int> found;
    db->set_query("seLect event_id, event_name from dnf_event_info");
    ok = db->exec(true);
    if (!ok)
        return false;

    const int rows = db->get_n_rows();
    int i = 0;
    for (i = 0; i < rows; ++i)
    {
        ok = db->fetch();
        if (!ok)
            break;
        ok = db->get_int(0, id);
        if (!ok)
            return false;
        ok = db->get_str(1, name, 0xff);
        if (!ok)
            return false;
        idx = _FindEventTable(id, name, dup);
        // P197④ 镜像：ORIG 0x0811a395 `not %eax; shr $0x1f,%eax; test %al,%al; je`
        //（4.1 对 `idx >= 0` 的寄存器 bool 物化）vs 4.4 `test %eax,%eax; js` 直跳。
        // 等价位形 `(unsigned int)~idx >> 31` 实测仍被 4.4 折叠为 js（无收益），
        // 保留可读正向形；2 条差归 N93附/P197④ 封顶。
        while (idx >= 0)
        {
            if (dup)
                return false;
            // P167：ORIG 用 map::insert(make_pair(idx,1))（首值保留），非 operator[]
            found.insert(std::make_pair(idx, 1));
            break;
        }
    }

    for (i = 0; i < GetEventTableCount(); ++i)
    {
        // P293：ORIG 0x0811a427 先 find（sret -0x70）后 end（sret -0x28）⇒ 原码
        // 为具名迭代器形；单表达式 `find(i)==end()` 在 4.4 恒反序（end 先）。
        std::map<int, int>::iterator it = found.find(i);
        if (it == found.end())
            out.push_back(i);
    }
    return true;
