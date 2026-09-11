// ---- killMonsterErase（ORIG 0x0814d214）----
// find(uid) → const_iterator 判 end() → mob=it->second、outCount=(uchar)+0x14、
// erase(by key) → true；未找到 false。
// [b28d-62] N82：erase key 改内联 `(int)uidParam`（新临时槽）后 68=68；残差仅
//   uid 与 find-sret 槽互换（P112 槽序不可推）。p167 CLEAR。ORIG 0x0814d214
// [b28d-solo-CBossStage] AE：found 具名 iterator 中转（P50 复活）消 uid/find-sret
//   槽互换；erase key 保持内联 (int)uidParam。ndiff 8→0。
bool CBossStage::killMonsterErase(unsigned short uidParam, map_monster& mob,
                                  int& outCount)
{
    int uid = uidParam;                           // ORIG -0x18 key 槽（int 承接）
    // [b28d-62] erase 的 key 用内联 `(int)uidParam`（ORIG 0x0814d2d3 从 -0x2c 形参
    //   截断槽重算并落**新槽** -0xc），非复用 uid 槽；find/end 保持直构形
    //   （先声明后赋值实测 68→78 劣化，P50 复活不适用本件）。
    std::map<int, map_monster>::iterator found =
        m_stageData.m_monsterMap.find(uid);
    std::map<int, map_monster>::const_iterator it = found;
    if (it != m_stageData.m_monsterMap.end())
    {
        mob = it->second;                             // map_monster::operator=
        outCount = (unsigned char)it->second.m_level;   // ORIG pair+0x14 = mob+0x10
        m_stageData.m_monsterMap.erase((int)uidParam); // by-key erase
        return true;
    }
    return false;
}