// 读 MSG_COMBO_SKILL：m_pad0d 树种类、树数量(≤6)、每树 {kind, 技能数(≤6),
// skillIdx...}；checkComboSkillIndex 校验通过才收技能列表；逐项插入
// m_comboMap(kind → vector<int>)。任何 get 失败按行号走 LineFunc；
// 外层 get_short 失败/循环耗尽均归 LineFunc(0x30c)。
// 【batch-17 X1 池收口（§6.2）】orig 213 / ours 210（gap 3）：调用点差集
//   25↔25 **归零**（残差 8/8 全为 pair/vector 的 C2/D2 ↔ C1/D1 同体符号名
//   对称差）；残 3 为迭代器/临时对象落槽步序（N82）⇒ 语义完成。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x0825ef78 orig/ours=213/210 ndiff-blocks=31
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movw   $0x0,-0x42(%ebp) | movb   $0x0,-0x43(%ebp) | movb   $0x0,-0x44(%ebp)
int Dispatcher_ComboSkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_COMBO_SKILL& req = reinterpret_cast<MSG_COMBO_SKILL&>(msg);
    std::map<short, std::vector<int>, std::less<short>,
             std::allocator<std::pair<short const, std::vector<int> > > >&
        comboMap = req.m_comboMap;
    std::vector<int> skills;
    // [G6·read_24 2026-09-11] ORIG 0x0825ef86 函数头 4 条初始化
    //   `movw $0x0,-0x42; movb $0x0,-0x43; movb $0x0,-0x44; movw $0x0,-0x46`
    //   （P212/P339 全头部声明+定值）。探针确认 4.4 按类型分槽、发射序=声明序，
    //   故声明序取 kind/treeCount/skillCount/skillIdx，与 ORIG 逐条对应。
    short kind = 0;          // -0x42
    char treeCount = 0;      // -0x43
    char skillCount = 0;     // -0x44
    short skillIdx = 0;      // -0x46
    comboMap.clear();
    if (!packet.get_byte(req.m_pad0d))
    {
        return LineFunc(0x301,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(treeCount))
    {
        return LineFunc(0x304,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < (int)treeCount && i < 6; ++i)
    {
        if (!packet.get_short(kind))
        {
            break;  // 归 LineFunc(0x30c)
        }
        if (checkComboSkillIndex(kind))
        {
            if (!packet.get_byte(skillCount))
            {
                return LineFunc(0x312,
                                __PRETTY_FUNCTION__,
                                0, 0);
            }
            skills.clear();
            for (int j = 0; j < (int)skillCount && j < 6; ++j)
            {
                if (!packet.get_short(skillIdx))
                {
                    return LineFunc(0x319,
                                    __PRETTY_FUNCTION__,
                                    0, 0);
                }
                skills.push_back((int)skillIdx);
            }
            comboMap.insert(std::make_pair(kind, skills));
        }
    }
    return LineFunc(0x30c,
                    __PRETTY_FUNCTION__,
                    0, 0);
}