int CParty::getDungeonTypeKind(CDungeon const* dungeon)
{
    if (dungeon == 0)
        return 8;
    // m_89c（CDungeon.h:777，unsigned char）：ORIG movzbl 0x89c(%eax);test %al
    if (dungeon->m_89c != 0)
        return 1;
    // ORIG 0x85bdd40 test %al;setg —— 有符号字节 >0 比较（(signed char) 先例同
    // CPartyDepsB2:550/CPartyOnlyOrig:881）；uchar 直 >=1 会折 setne。
    if ((signed char)dungeon->get_dimension_possible() > 0)
        return 2;
    if (dungeon->getDungeonKind() == 1)
        return 3;
    if (dungeon->get_blood_dungeon_type() == 1)
        return 4;
    if (dungeon->get_blood_dungeon_type() == 2)
        return 5;
    if (dungeon->isTowerOfDespairDungeon())
        return 6;
    if (dungeon->isTournamentDungeon())
        return 7;
    return 0;
