bool CCreature::GainItemExpCreature(int exp, CUser* user)
{
    // ORIG 0x83379c4..0x83379cc：char levelUp(-0x11) 与 int newLevel(-0x10) 均函数头
    // 声明并预清（P198①），newLevel 为具名 int 局部（非表达式直传）。
    char levelUp = 0;
    int newLevel = 0;
    if (m_isGrow != 0)
        return false;
    if (m_script->validItem_AType(exp))
    {
        // ORIG 0x8337a0x：`mov %al,-0xa; addb $1,-0xa; cmpb $0xa,-0xa` ⇒ char
        // 局部就地自增（P320 字节版；int 局部会拆 movzbl/add/mov 三段）。
        char value = (char)m_field4d;
        ++value;
        if (value > 10)
            value = 10;
        m_field4d = (unsigned char)value;
    }
    if (m_script->validItem_BType(exp))
    {
        char value = (char)m_field4d;
        --value;
        if (value < 0)
            value = 0;
        m_field4d = (unsigned char)value;
    }
    // ORIG 0x8337a5x：低字节写 m_field4d + 高半字 +1 均为 union 字节/半字视图
    // （P197① 拆半；显式移位/掩码链恒多 5 条）。
    union ExpPack { int all; unsigned short h[2]; unsigned char b[4]; };
    ExpPack packed;
    packed.all = m_exp;
    packed.b[0] = m_field4d;                       // 低字节 ← m_field4d
    packed.h[1] = (unsigned short)(packed.h[1] + 1); // 高 16 位进度 +1（16 位回绕）
    m_exp = packed.all;
    newLevel = GetExpLevel((int)(short)packed.h[1]);   // cwtl 符号扩展
    if (m_level < newLevel)
        levelUp = 1;
    m_level = newLevel;
    // P0：ORIG 0x8337b0x 的 notify 判定无槽往返 ⇒ 表达式内联。
    if (m_level < m_script->GetMaxLevel() || levelUp != 0)
    {
        CPacketRespondent pkt(user, 0, 0);
        pkt.MakeNotipacketGainExpCreature(m_exp, m_level, IsGrowCreature());
    }
    // P226 条件向 + 嵌套 if：ORIG 0x8337b2x 对 `levelUp && IsAbleSelectEvolute`
    // 出物化岛（无槽），外层 `canEvolute && !SelectEvolute` 为直跳。
    if (({ levelUp != 0 && IsAbleSelectEvolute(user); }))
    {
        if (!SelectEvolute(user))
            return false;
    }
    m_saveFlag = 1;
    return true;
