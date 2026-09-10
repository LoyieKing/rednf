CRewardUserList::~CRewardUserList()
{
    // 成员按声明反序自动析构：deque(+0x18) → map(+0x00)（ORIG D1/D2 0x081bbffc 同序）
