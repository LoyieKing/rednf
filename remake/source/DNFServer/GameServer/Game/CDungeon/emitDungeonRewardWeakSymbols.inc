void emitDungeonRewardWeakSymbols()
{
    TournamentRewardItem item;      // C1/C2
    item.~TournamentRewardItem();   // D1/D2
    STNamedMonsterMapPos pos;       // C1/C2
    pos.~STNamedMonsterMapPos();    // D1/D2
}