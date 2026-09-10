void DB_Update::makeSlotString(std::string& out, const _Slot* arr, int count)    // [objdump 0x813f442]
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        // ORIG 0x0813f4ef fmt="%d%s%d%s%d%s%d%s"，"," ×3(0xc/0x14/0x1c esp)+
        // "|"@0x24 为独立 rodata 实参（9 实参槽 outgoing ⇒ 帧 0xc0）。同
        // makeRewardString 配方（batch-13 帧差根因；实证装载序列 0x0813f483..0x0813f4fd，
        // 值序 byte0,int1,byte5,int6 与现 cast 一致）。
        // 【§6.2 标注】gap 5→1（o=73 n=68→72）：残余 1 = ORIG int1 装载两段
        // `mov 0x1(%eax),%eax; mov %eax,%ecx`（0x0813f4a5/4a8，4.1 先载后转），
        // 4.4 直出目标寄存器单段，寄存器分配形态差 N82，不可用 C++98 表达。
        std::sprintf(buf, "%d%s%d%s%d%s%d%s",
                     static_cast<int>(arr[i].m_byte0), ",", arr[i].m_int1, ",",
                     static_cast<int>(arr[i].m_byte5), ",", arr[i].m_int6, "|");
        out += buf;
    }
