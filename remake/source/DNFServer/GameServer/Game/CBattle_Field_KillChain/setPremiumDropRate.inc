void CBattle_Field::setPremiumDropRate(WongWork::stGenerateRefData_t& ref,
                                       int premiumRate)
{
    // ORIG 0x830b848：短路链单表达式（无 bool 槽），链尾 mov$1/mov$0 双岛+
    // test；首条件 test+jg（有符号 char >0）。+0x40 低字节单条 disp 读
    // （movzbl 0x40(%eax)）。
    // ORIG 0x830b85f test%al+jg：源码按有符号 char>0（P22/P3 证据形态）；
    // 次条件 ORIG test%al：调用侧按单字节比较收窄（P3）。
    if (static_cast<signed char>(m_pDungeon->get_blood_dungeon_type()) > 0
        || static_cast<unsigned char>(getItemDropDungeonType()) != 0
        || m_pDungeon->isTowerOfDespairDungeon()
        || ref.m_field40_byte != 0)
        ref.m_premiumDropRate = 0;
    else if (premiumRate == 0)
        ref.m_premiumDropRate = -1;
    else
        ref.m_premiumDropRate = premiumRate;
