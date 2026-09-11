int BlueMarble::intoBoardZone(int zone)
{
    setDungeonIndex(zone, getRandomDungeon(zone, (BlueMarbleTileScript::Type)14));
    tileType(zone);
    setVisitZone(m_users[zone].getBoardZone());
}