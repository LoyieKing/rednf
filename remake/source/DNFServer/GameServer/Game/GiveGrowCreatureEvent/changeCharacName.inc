void GiveGrowCreatureEvent::changeCharacName(
    CUser* user, int key, char* old_, int new_)     // 0x081b717a
{
    // ORIG 汇编语义：user!=0 时 key=m_characNo、new_=新名字指针；
    // serverGroup 取 target 用户 GetServerGroupToPvP。元素命中条件为
    // m_serverGroup 相同 && m_characNo==key；若现名与新名 strcmp 不同，
    // strncpy(m_name,new_,0x1d) 并发 DB_EventUpdateGiveGrowCreature。
    // [b28d-28] ORIG 0x081b7180 只判 `user != 0`（无 new_ 判空）；serverGroup
    // 为 int 槽（`mov %eax,-0x1c`）；mine 只判 `empty()`（无 `mine == 0`，P171）。
    if (user == 0)
        return;

    int serverGroup = user->GetServerGroupToPvP();
    std::vector<GrowthCreatureEvent::eventCharacInfo>* mine =
        static_cast<std::vector<GrowthCreatureEvent::eventCharacInfo>*>(
            user->getGrowthCreatureEventdata());
    if (mine->empty())
        return;

    // ORIG 槽序 -0x14/-0xd/-0xc（foundIdx/found/i）：命中即置 found+foundIdx 并
    // break，循环后统一发 DB 请求（只处理首个命中项）。
    int foundIdx = 0;
    // P158 反向：ORIG `cmpb $0,-0xd(%ebp); je`（无 xor 物化）⇒ found 是 uchar。
    unsigned char found = 0;
    // P287：正写 `size() > idx` 优于 `idx < size()`（后者引 %ebx 缓存 +2 条）。
    for (unsigned int i = 0; mine->size() > i; ++i)
    {
        // P157/P183：ORIG 每个字段读取都重新 `operator[](i)`，禁缓存引用 e。
        // ORIG `movzwl + cwtl`（两条）⇒ 比较处显式 (short) cast（字段为 unsigned short）。
        if (static_cast<short>((*mine)[i].m_serverGroup) == serverGroup &&
            (*mine)[i].m_characNo == key &&
            strcmp((*mine)[i].m_name, old_ /*new 名*/) != 0)
        {
            strncpy((*mine)[i].m_name, old_, 0x1d);
            found = 1;
            foundIdx = i;
            break;
        }
    }
    // P223 负例边界：found 守卫若写早返会多产第 3 个尾岛（ORIG 仅 2 岛）⇒ 正向包裹。
    if (found != 0)
    {
        SIG_GIVE_GROWTH_CREATURE_EVENT::eventCharacInfo sigInfo;
        sigInfo.m_serverGroup = (*mine)[foundIdx].m_serverGroup;
        sigInfo.m_characNo = (*mine)[foundIdx].m_characNo;
        sigInfo.m_job = (*mine)[foundIdx].m_job;
        strncpy(sigInfo.m_name, (*mine)[foundIdx].m_name, 0x1d);
        DB_EventUpdateGiveGrowCreature::makeRequest(user->get_acc_id(), sigInfo);
    }
