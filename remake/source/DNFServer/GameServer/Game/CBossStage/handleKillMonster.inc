// ---- handleKillMonster（ORIG 0x08150388）----
// 击杀：killMonsterErase → partyType==1（PowerWar）时 0x26 广播即返回 →
// KillMonsterGetExp → KillMonster 日志 → killMonsterDropItem。
// [b28d-62] N82：p167 EXTRA getParty/get_party_type 各 1（ORIG 缓存结果），
//   188/193。ORIG 0x08150388
// [b28d-solo-CBossStage] AE：m_party->m_battleField.m_bloodState 直读（消
//   getParty/get_party_type call）；内联 curExp/roleType（P0）；KillMonsterGetExp
//   返回类型 int→bool（P248 档①，.h 同步，mangled 不变）。ndiff 131→0。
bool CBossStage::handleKillMonster(CUser* user, unsigned short a,
                                  unsigned short b, const MSG_MONSTER_DIE& die,
                                  int* out)
{
    (void)out;   // ORIG 未使用第 5 参
    map_monster mob;
    int eraseCnt = 0;
    if (!killMonsterErase(a, mob, eraseCnt))
        return 0;

    // P0：ORIG 0x081503f5 `mov 0x4(%eax),%eax; mov 0xcd8(%eax),%eax` ⇒ 原码为
    //   m_party 直访问（getParty/get_party_type 均未发射调用）。
    if (m_party->m_battleField.m_bloodState == 1)   // PowerWar
    {
        PacketGuard packet;
        packet.put_header(0, 0x26);
        packet.put_short((int)a);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 1;
    }

    unsigned int exp = 0;
    if (!KillMonsterGetExp(mob, exp))
        return 0;

    // cUserHistoryLog 为 CUser 嵌套成员 m_historyLog（ORIG user+0x79700）
    user->m_historyLog.KillMonster(mob.m_mobIndex,
                      die.m_killValue1,   // +0xa1e（耗时类）
                      die.m_killValue2,   // +0xa1a（经验类）
                      die.m_killPairA24,
                      die.m_killPairA26,
                      die, mob.m_roleType, mob.m_roleType == 5,
                      user->getCurCharacExp(), (int)exp,
                      (eExpAddReason)0xb);

    if (!killMonsterDropItem(mob, eraseCnt, a, b))
        return 0;
    return 1;
}