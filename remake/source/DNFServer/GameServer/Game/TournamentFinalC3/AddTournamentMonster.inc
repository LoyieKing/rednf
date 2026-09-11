// ---------------------------------------------------------------------------
// CBattle_Field::AddTournamentMonster @ 0x08304718
// 报告要点：i<=1 循环（cmpl $1/setle）；每次迭代先构造 _mapMonster（ctor
// 0830e4fc）再判 info.m_monster[i].m_mobId!=0（param+i*8+4），命中按 ORIG
// 写入顺序填字段：flag04(字节)=1 → field08=0 → levelAdd=1 → field14=1 →
// spawnType=0 → roleType 以 if/else 双分支直接存储（isUltimate?3:0）→ champion(字节)=0 → posX=0 →
// orderIdx=0 → hpPct=100 → flag30(字节)=0（注意：ORIG 不写 m_level(+0x24)，
// 保持 ctor 默认），push_back 后析构局部 —— 空槽同样走 ctor/dtor 对。
void CBattle_Field::AddTournamentMonster(const TournamentInfo& info,
    std::list<_mapMonster>& mobList, bool isUltimate)
{
    for (int i = 0; i <= 1; ++i)
    {
        _mapMonster mob;
        if (info.m_monster[i].m_mobId != 0)
        {
            mob.m_mobId = info.m_monster[i].m_mobId;
            mob.m_flag04 = 1;
            mob.m_field08 = 0;
            mob.m_levelAdd = 1;
            mob.m_field14 = 1;
            mob.m_spawnType = 0;
            if (isUltimate)
                mob.m_roleType = 3;
            else
                mob.m_roleType = 0;
            mob.m_champion = 0;
            mob.m_posX = 0;
            mob.m_orderIdx = 0;
            mob.m_hpPct = 100;
            mob.m_flag30 = 0;
            mobList.push_back(mob);
        }
    }
}