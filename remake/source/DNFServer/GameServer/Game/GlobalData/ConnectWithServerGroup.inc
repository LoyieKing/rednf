// [b28d-solo-GlobalData] 归零 67=67：group 内联为实参表达式复现 R2L 装载序
// （先 &second 后 m_tail；具名/register 局部均提前，P52/P157）；p167 CLEAR | ORIG 0x080ec8f8
bool CDBConnector::ConnectWithServerGroup(DBInfoMap infoMap)
{
    // ORIG 0x080ec8f8：遍历连接表；idx==0 跳过；以 +0x168 处 server group
    // 为组参数调 Open；失败即 false。
    for (DBInfoMap::iterator it = infoMap.begin(); it != infoMap.end(); ++it)
    {
        // ORIG 0x080ec91d/0x080ec93b/0x080ec949/0x080ec954：每次字段访问
        // 重新调用 operator->（4 次 ptEv，P157 逐字段重读，禁引用缓存）。
        // ORIG 0x080ec926 `sete %al; test; jne <跳过>` = 负极性守卫（P222）。
        if (it->first == (ENUM_DB_HANDLE_IDX)0)
        {
            continue;
        }
        // ORIG 080ec946（ConnectWithServerGroup.md）：`mov 0x168(%eax)`（eax=pair 基址）
        // = second+0x164 = STDBConnInfo::m_tail（存 server group；Open 第 2 实参 %ebx）。
        // 原写 +0x160 系偏移误读（pair/second 基混淆），此处以具名字段修正为 +0x164。
        // ORIG 0x080ec954 起装载序（R2L）：先 `lea 0x4(%eax),%esi`（第3实参
        // &second），再 `mov 0x168(%eax),%eax; mov %eax,%ebx`（第2实参 m_tail），
        // 最后 `mov (%eax),%eax`（第1实参 first）⇒ group 必须内联为实参表达式
        // （具名/register 局部都会把 m_tail 装载提前）。
        if (!Open(it->first,
                  static_cast<ENUM_SERVER_GROUP>(it->second.m_tail),
                  it->second))
        {
            return false;
        }
    }
    return true;
}