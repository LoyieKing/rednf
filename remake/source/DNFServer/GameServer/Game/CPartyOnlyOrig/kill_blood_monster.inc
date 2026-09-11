// ORIG T 0x085a3b6a（kill_blood_monster.md）：血轮击杀转发战场层。
void CParty::kill_blood_monster(CUser* user, int monsterIdx,
                                unsigned short* param)
{
    reinterpret_cast<CBattle_Field*>(&m_battleField)
        ->kill_blood_monster(user, monsterIdx, param);
}