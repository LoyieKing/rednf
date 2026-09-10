CWorldMapList::~CWorldMapList()
{
    destroy();  // ORIG D1 0x837481e：destroy() 后析构 map
