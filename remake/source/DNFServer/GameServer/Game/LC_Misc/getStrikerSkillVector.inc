std::vector<StrikerSkill>* StrikerScript::getStrikerSkillVector(short first,
                                                                short second)
{
    // b28d-36（修法）：ORIG 0x08a9e7f4 段把 key 槽（int @-0x10）用**两次 16 位
    // store** 填成 `低半=first / 高半=second`：
    //   movzwl -0x1c,%eax; mov %ax,-0x10 ; movzwl -0x20,%eax; mov %ax,-0xe
    // 然后 find 传 &key（int）。旧形 `ushort uw + int key=(int)uw` 使 4.4 多出
    // uw 槽并 32 位直存 key（`mov %eax,-0x14`，+3 条、全帧浅 4）。
    // 用 union 的 short 视图逐半字写入即逐条复现（P197① union 拆半同族；
    // union 为函数局部 POD，零符号发射）。
    union StrikerSkillKey
    {
        int key;
        struct { unsigned short lo, hi; } parts;
    };
    StrikerSkillKey k;
    k.parts.lo = static_cast<unsigned short>(first);
    k.parts.hi = static_cast<unsigned short>(second);
    std::map<int, std::vector<StrikerSkill>*>::iterator it =
        m_skillMap.find(k.key);
    if (it != m_skillMap.end())
        return it->second;
    return 0;
