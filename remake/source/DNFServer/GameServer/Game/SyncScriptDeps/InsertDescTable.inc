// [b28d-L8→b28d-SymRemnant P326 翻案] N82 作废，本件 **AE(248/248 全行一致)**。
// 胜出配方（逐条机器码证据，非沿用旧封顶）：
//  ① G_CDataManager 结果不落槽（0x0812269d 直传参）→ 链式表达式；
//  ② 视图 struct 必须 packed：groupIndex ORIG 非对齐直读 `mov 0x2(%eax)`
//     (0x08122757)、name `add $0x24`(0x0812278d)——对齐版会推到 +0x4/+0x28；
//  ③ query 两步 `std::string query; query = "…";`（C1 后 ssaSEPKc 0x081226d7）；
//  ④ ORIG 0x081226be 构造**第二个未用 string**(槽 -0x4c)、尾部 0x08122a0d 先于
//     query 析构 ⇒ 原码遗留声明（legacyRow），它决定深层槽链与帧 0x4fc；
//  ⑤ 槽序配方：声明序 refs→query→legacyRow→total→**db(裸声明)**→**it(出环
//     具名)**→row ⇒ total -0x28 / db -0x24 / row -0x20 / it -0x50；
//  ⑥ row=0 须在 beginEv 之后发射 ⇒ 迭代器初始化提出 for-init（`for(;cond;)`）；
//  ⑦ 循环步进 **postfix `it++`**（0x08122dfa `_ZNSt23_Rb_tree_const_iterator…ppEi`）；
//  ⑧ 逗号判形 `row < total - 1`（写 `total-1 > row` 被 4.4 规范化成 add/cmp/jae）；
//  ⑨ name 缓冲用 `char name[1024] = { 0 };`（0x08122774 常量 rep stosl；
//     显式 memset 退化成 call memset@plt）；idx/grp/name 声明**交错**在
//     各自 sprintf 前（0x08122746/-0x98、0x08122766/-0xdc、0x08122774/-0x4dc）；
//  ⑩ 错误分支 = tuple 声明后、单语句临时 `cMyTrace(…)(…)`（三串实参先求值
//     %edi/%esi/%ebx 0x081227b1..）+ **return false**（非 ok+break：ORIG 无
//     ok 槽，%esi 是共享清理尾路径标志）；
//  ⑪ 收尾 `if (!db->exec(true)) return false; return true;`（0x081229e3 xor/test/
//     je 形，P101/P84 先例；`== 1` 会被折叠）。
// 并行试过无效的形：具名 mgr 槽；memset；ok+break；`total-1>row`；`++it`；
// size_t total/row；row 声明先于 total（db/total 槽互换）；db 提前 `=0` 初始化
// （多发一条 movl）。P326 的“第二迭代器右值临时”形**不适用**本件：ORIG
// begin/end 皆 `_ZNKSt3map…`（const 直出），无 const_iterator 转换临时。
bool APSystem::CSyncScript::InsertDescTable()
{
    // ORIG 0x08122698..0x081226b0：G_CDataManager 结果不入具名槽，直接
    // `mov %eax,(%esp)` 传参 ⇒ 原码无 mgr 局部（槽平移根因之一）。
    const std::map<unsigned short, const APSystem::CActionPointEx*>& refs =
        G_CDataManager()->GetActionPointEtcParameter(false)->m_actionPointRefs;

    // ORIG 0x081226b3..0x081226d7：`_ZNSsC1Ev` 后走 `_ZNSsaSEPKc` ⇒ 原码是
    // 默认构造 + operator= 两步（写 copy-init 会直接发 C1PKc，调用形不符）。
    std::string query;
    // ORIG 0x081226be：**第二个** string 默认构造于 -0x4c(%ebp)，全函数无任何
    // 赋值/读取点，尾部 0x08122a0d 先于 query 析构 ⇒ 原码遗留的未用局部声明。
    // 它决定 -0x4c 起全部深层槽位（it -0x50/tuple -0x54/idx -0x98/name -0x4dc）
    // 与 0x4fc 帧尺寸 [推断自本函数 ORIG 汇编]，按布局证据恢复。
    std::string legacyRow;
    query = "inSert into charac_action_point_desc "
            "(action_index, action_group_index, action_group_name) values ";

    // 有符号证据：ORIG 尾比较 `mov -0x28(total); sub $1; cmp -0x20(row); jle`
    // （jle 有符号）⇒ total/row 是 int。判形取 `row < total - 1`（Ghidra
    // local_24 < local_2c + -1）。ORIG 槽序 total -0x28 / db -0x24 / row -0x20
    // ⇒ 4.4 槽分配按“后注册更浅”：声明序 total → db → row 复现
    // total -0x28 / db -0x24 / row -0x20。db 仅**声明**（-O0 无初始化 ⇒
    // 不发指令，仅占槽），循环后赋值 [推断自 ORIG 槽布局 + Ghidra 变量表]。
    const int total = (int)refs.size();
    MySQL* db;
    // ORIG 指令序：beginEv(0x05e) → `sub $4`(0x070) → **row=0(0x073)** → jmp cond。
    // ⇒ it 先具名初始化、row 初始化在其后：`for(;;)` 空 init 形复现。
    std::map<unsigned short, const APSystem::CActionPointEx*>::const_iterator it = refs.begin();
    int row = 0;
    for (; it != refs.end(); it++, ++row)
    {
        const APSystem::CActionPointEx* ex = it->second;
        if (ex == 0)
        {
            continue;
        }

        // ORIG 0x08122726：tuple **默认构造**（_ZNSsC1Ev，位于 ex 检查后、
        // 两个 sprintf 前），"(" 走首个 `operator+=(char const*)`（0x08122830）。
        std::string tuple;

        // ORIG sprintf 目标序：idx(-0x98) → grp(-0xdc) → name(-0x4dc)，
        // name 清零为**常量尺寸内联 rep stosl**（0x08122774..0x08122788，
        // 0x100 dwords）⇒ 原码形如 `char name[1024] = { 0 };`（同 TU
        // getItemValueStirng `char conv[0x100] = {0}` 产同款 rep stos；
        // 显式 memset 会退化成 memset@plt call）。
        char idx[68];
        sprintf(idx, "%u", (unsigned int)ex->m_ap.m_actionNo);
        char grp[68];
        sprintf(grp, "%u", (unsigned int)ex->m_ap.m_groupIndex);
        char name[1024] = { 0 };
        if (!CodePage::script2Database(
                const_cast<char*>(ex->m_str24.c_str()), name))
        {
            // ORIG 0x081227b1..0x08122821：c_str/database/script 三实参先求值
            // （%edi/%esi/%ebx），其后才 cMyTrace C1 ⇒ 原码是单语句临时对象调用。
            // 随后 ebx=0 + 公共清理尾（0x08122930 D1(tuple) → 0x08122a0d）
            // ⇒ 错误分支为 `return false`（非 ok+break，ORIG 无 ok 栈槽）。
            cMyTrace(__PRETTY_FUNCTION__, 0x409, 5)(
                "Error!!! %s->%s [%s][%s][%d]",
                CodePage::script(), CodePage::database(),
                ex->m_str24.c_str(),
                "static bool APSystem::CSyncScript::InsertDescTable()",
                0x409);
            return false;
        }

        tuple += "(";
        tuple += idx;
        tuple += ", ";
        tuple += grp;
        tuple += ", ";
        tuple += "'";
        tuple += name;
        tuple += "'";
        tuple += ")";
        // Ghidra 0x081228e1：`if (local_24 < local_2c + -1)` ⇒ 原码判形
        // `row < total - 1`（写 `total - 1 > row` 会被 4.4 规范化成
        // `add $1 / cmp / jae` 反向形，ORIG 是 `sub $1 / cmp / jle`）。
        if (row < total - 1)
        {
            tuple += ",";
        }
        query += tuple;
    }

    db = GlobalData::s_db_mgr->GetDBHandle(
        (ENUM_DB_HANDLE_IDX)2, (ENUM_SERVER_GROUP)0);
    db->set_query(query.c_str());
    // P101/P84 同款（见 TruncateDescTable 注）：ORIG 0x081229e3 走
    // `xor $1; test; je` 早返回形，`== 1` 会被 4.4 折成 cmp。
    if (!db->exec(true))
        return false;
    return true;
}