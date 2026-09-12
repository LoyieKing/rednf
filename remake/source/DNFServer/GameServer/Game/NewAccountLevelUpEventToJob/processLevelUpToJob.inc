// ORIG T 0x081b82a4 sz 0x515。
// 新账号升级转职奖励：查奖励表（growup 聚合 / 单项）→ 账号级 once-gift 多重
// 映射去重（同 job 区间内已存在同 level 则中止）→ 插入 (level, job) → DB
// 上报 → 逐条 create_item + 邮寄（标题 game_server_msg_285，正文
// snprintf(game_server_msg_286, level)），失败走 logFormat。成功返回 1，
// 前置失败返回 0。
// b28d-31: N 封顶（N82/P197）——ORIG 0x081b82a4；351 vs 353（帧/槽）。
bool NewAccountLevelUpEventToJob::processLevelUpToJob(CUser& user, bool growup)
{
    // ORIG：全部 getter 在实参位内联求值（P24），不设持久局部 —— key 构造
    // firstGrow→esi(movsbl)、job→ebx 跨调用缓存、level→eax 直传；去重循环
    // 的 ==level 与 makeRequest/snprintf 的 level/job 各使用点重新调用 getter。
    LevelUpRewardItemTableKey key(user.get_charac_level(),
                                  user.get_charac_job(),
                                  user.getCurCharFirstGrowType());
    std::vector<LevelUpRewardItemData> rewards;    // vector C1 0x081b8c00
    if (growup)
    {
        // [SemA01 2026-09-12 真语义修正] ORIG 0x081b830a `call _Z14G_CDataManagerv`
        // 后 0x081b830f `lea 0xb5f8(%eax),%edx`。0xb5f8 = m_newAccountLevelUp(+0xb5e0)
        // + 0x18 = stNewAccountLevelUpToJobScript::m_levelUp18（CDataManager.h:3951/3007）。
        // 旧译写成角色级 m_levelUpReward(+0x50ac) ⇒ 读错表（真语义差异）。
        if (!G_CDataManager()->m_newAccountLevelUp.m_levelUp18
                 .get_data_when_growup(key, rewards))
            return false;                          // ebx=0 出口
    }
    else
    {
        LevelUpRewardItemData data;                // C1 0x081b88bc
        if (!G_CDataManager()->m_newAccountLevelUp.m_levelUp18
                 .get_data(key, data))
            return false;
        rewards.push_back(data);                   // push_back 0x081b8a52
    }
    // getCharacLevelUpGift_AccountOnce（T 0x08691e5c）返回 &m_levelUpGift
    // （+0x8ec04，+0x00 即 multimap<int,int>）；empty/lower_bound/upper_bound/
    // insert 均以该指针为 this。迭代器经默认构造 + 赋值（临时中转拷贝，
    // P28），upper_bound 结果缓存，循环体内 it!=end() 防御检查。
    std::multimap<int, int>& gifts =
        *static_cast<std::multimap<int, int>*>(
            user.getCharacLevelUpGift_AccountOnce());
    if (!gifts.empty())
    {
        // [P1T5 修法 i4 · 已落] ORIG 循环尾 0x81b8477 调 `ppEi`（后置 ++，实参 0 +
        // 返回临时 -0x44），非 `ppEv` ⇒ 改 `it++`（丢弃返回值，语义等价）。
        // 四态实测（均以「引用缓存 + 本件」为底，ORIG=351/46 块/p167 差集 4 项）：
        //  ①两步声明+前置(基线) 353/gap −2/blocks 46/p167 3 项，TU identical=42/missing=22；
        //  ②两步声明+后置（**本落地形**）357/gap −6/**blocks 34/p167 0 项**，identical=42/missing=21；
        //  ③声明即初始化+前缀 348/gap +3/blocks 46，identical=**41**/missing=22；
        //  ④声明即初始化+后置 352/gap −1/blocks 30，identical=**41**/missing=20。
        // ⇒ ③④ 的 C1Ev 消失会让「`_Rb_tree_iterator C1Ev` 与 ORIG identical」那一项
        // 从账上掉（n1_v 单件实测 base=IDENTICAL、形B=MISSING_IN_OURS）⇒ 取 ②：
        // 四数不劣化（identical 42/ae 73/near 0/diff 2 同基线，missing 22→21 改善），
        // 且 blocks 46→34、p167 差集 4→0；代价是 gap −2→−6（C1Ev 调用 3 条 +
        // ORIG 确有的中转拷贝 2 条，均为符号级对齐所需）。
        // 残余：8 字节栈槽整体位移（我方 body −0x1ad vs ORIG −0x1b5）。
        std::multimap<int, int>::iterator it;
        it = gifts.lower_bound(user.get_charac_job());
        std::multimap<int, int>::iterator upper =
            gifts.upper_bound(user.get_charac_job());
        while (it != upper)
        {
            if (it != gifts.end() && it->second == user.get_charac_level())
                return false;                      // 该等级已领取 → 中止
            it++;   // [P1T5] ORIG 0x81b8477 为后置 `ppEi`（实参 0 + 返回临时 -0x44）。
        }
    }
    gifts.insert(std::make_pair(user.get_charac_job(),
                                user.get_charac_level()));
    DB_EventAccountCharacterLevelUpOnceGift::makeRequest(
        user.get_acc_id(), static_cast<short>(user.GetServerGroupToPvP()),
        user.getCurCharacNo(), static_cast<short>(user.get_charac_job()),
        static_cast<short>(user.get_charac_level()));
    char body[256] = { '\0' };                     // -0x1b5 起 0x100 清零（P4 内联 memset）
    OS_API::snprintf(body, 0xff,
                     g_scriptStringManager_.findString(4, "game_server_msg_286",
                                                       (bool*)0),
                     user.get_charac_level());
    for (std::vector<LevelUpRewardItemData>::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        // [P1T5 · p167] ORIG 255 只调一次 `__normal_iterator::de` 并把返回地址
        // 缓存到槽 -0x1c（`mov %eax,-0x1c(%ebp)`，0x81b8632），随后两处经
        // `mov -0x1c(%ebp),%eax` 访问 ⇒ 源码为循环内引用缓存形（非 `it->`），
        // 且该引用**声明早于 Inven_Item item**（ORIG de 调用 0x81b862d 先于
        // Inven_Item::C1Ev 0x81b863e）。
        const LevelUpRewardItemData& d = *it;
        Inven_Item item;
        item.m_addInfo = d.m_field0;               // -0xb3 ← data.f0
        G_CDataManager()->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0,
                                                  item, d.m_field4);
        // findString("game_server_msg_285") 在循环体内实参位（ORIG 每次迭代调）。
        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_285",
                                                  (bool*)0),
                item, 0, user.getCurCharacNo(), body,
                static_cast<int>(strlen(body)), 0,
                static_cast<ENUM_SERVER_GROUP>(user.GetServerGroupToPvP()), 0,
                0) != 0)
        {
            LogManager::logFormat(
                1, "NewAccountLevelUpEventToJob.cpp",
                __PRETTY_FUNCTION__,
                0x7f, "NewAccountLevelUpEventToJob Event Start!");
        }
    }
    return true;                                   // ebx=1
}