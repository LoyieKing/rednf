// [b28d-solo-W8LT_EventTail] DIFF→NEAR(N7) | ORIG 0x081b717a | P167 CLEAR（18/18）。
// 修法①（真语义 bug）：ORIG 命中后置 found/foundIdx 并 break、循环后统一发 DB 请求
// （只处理首个命中项），原码在循环体内直接发请求；②比较处显式 `(short)` cast 复现
// ORIG `movzwl+cwtl` 两条；③P158 反向：found 声明 uchar + `==0` 复现 `cmpb $0; je`
// （bool 恒 xor 物化）；④P287 `size() > i` + unsigned i 复现 mem 直比 seta；
// ⑤P223 负例边界：found 守卫改正向包裹消第 3 个尾岛（ORIG 仅 2 岛）⇒ 130=130。
// 残余 24 行 = 6 处 `(*mine)[i]` 的**实参装载序**（N 封顶）：ORIG 4.1 恒
// `mov this,%eax; mov idx,%edx`（this-first，idx 走 edx 中转），4.4.6 恒 idx-first
// （`mov idx,%eax; mov %eax,0x4(%esp); mov this,%eax`）。探针 /tmp/probe_cn（路由同款
// 4.4.6 后端）穷举 8 形：uint/int/size_t/ushort 下标、`(*p)[i]`/`p->operator[](i)`/
// 显式 cast/局部引用 receiver 全部 idx-first ⇒ 源形不可择，记 N（P75 跨版本实参装载
// 时刻，T5：装载序非类型旋钮）。
// [M1S5 2026-09-11 第 9 形试证·P307 `register vector*` receiver] our 130→123、
// ndiff 96→151 ⇒ 劣化回滚；P339 不适用（槽序已全等 -0x14/-0xd/-0xc/-0x1c/-0x18）；
// 语义轴复核：命中条件三段 disp 全等（m_serverGroup+0/m_characNo+4/m_name+0xc）、
// sigInfo 打包 +0/+4/+8/+0xc 与 strncpy len 0x1d 全等；§6.5.1 enc_kr_scan 全库
// mismatch=0（本 TU 无非 ASCII 字面量）⇒ N 维持。
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
}