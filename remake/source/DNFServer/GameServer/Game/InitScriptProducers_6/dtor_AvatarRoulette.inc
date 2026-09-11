AvatarRoulette::~AvatarRoulette()
{
    // ORIG D1：逆序析构 m_stikers[10]..[0]、再 m_avatars[10]..[0]
    // （编译器生成的数组析构等价于此）。
}