// OnCreateRoom ORIG T 0x8562d08 报告 OnCreateRoom.md
// 装备检查通过 → 自增授权号、复位双队与计数；失败返回错误码 0x13
// （ORIG +0x18 mov $0x13 / +0xb7 mov $0x0；IMatch 基类返回 int）
int CFairMatch::OnCreateRoom(CUser* user)
{
    if (!CheckUnusableEquipmentInUser(user))
        return 0x13;
    ++nextAuthValue_;
    m_matchNotifyId = nextAuthValue_;
    m_field04 = 0;
    m_teams[1].reset();          // ORIG 先 reset +0x64 队再 reset +0x1c 队
    m_teams[0].reset();
    m_teams[0].add(user);
    m_fieldB0 = 0;
    m_fieldB4 = 0;
    m_leaderSeat[0] = 0;
    m_leaderSeat[1] = 0;
    m_fieldC0 = 0;
    return 0;
}