// ----------------------------------------------------------------------------
// 5. ORIG T 0x08663cc0 sz 0x3b9。
// 升级奖励发放：repeat 事件 0x5d IsEventing 门控后按 (level, job, growType)
// 查 stLevelUpRewardItemScript（growup 时聚合收集，否则单项），逐条 create_item
// 并邮寄（标题 game_server_msg_187 / 正文 game_server_msg_188）；邮寄成功回执
// 为 0 时补写 MSG_MAILBOX_SEND 历史（+0x11 标题/+0x39 item idx/+0x3d add_info）
// 经 cUserHistoryLog::SendMail 落日志。
void CUser::processLevelUpEventReward(bool growupFlag)
{
    // b28d-solo-CUserDepsB: 语义闭合（本轮 256→246，p167 归零）——①P23/P200 key
    // 构造实参内联（grow→%esi/job→%ebx/level→%eax，去三槽）；②P214③
    // `char body[256]={0}` 复现 ORIG 0x086640c0 rep stos 内联清零（memset 恒 call）；
    // ③[本轮] P167 调用点差集归零：`(*it)` 具名引用局部（ORIG 0x08663e92 deEv 存
    // -0x1c 槽）+ `G_CDataManager()->m_itemList` 成员直读（原 getItemList() 多 1 call）；
    // ④[本轮] P0 四值实参内联（serverGroup/bodyLen/characNo/mailTitle 原各占 4 槽，
    // 致全帧 +0x10；ORIG 走 %esi/%edi/%ebx/%eax 无槽）⇒ 帧 0x2fc 与 ORIG 全等；
    // ⑤history 写入：权威类型 MSG_MAILBOX_SEND 局部（POD，sizeof 0x145）复现
    // 常量 disp 直存。
    // [L29 2026-09-09 翻正 DIFF→AE] 旧判「N107 成员取址 lea-vs-add 封顶」**作废**：
    // 真因是**实参求值序**——旧形 union + `reinterpret_cast<MSG_MAILBOX_SEND*>(
    // history.raw)` 的 cast 令 4.4 先物化实参（`lea -0x2c2(%ebp),%eax`）再以
    // `add $0x79700,%edx` 算 this；改真实类型局部 + `&history` 直传（无 cast、
    // 类型精确匹配形参）后 4.4 与 ORIG 同为 `mov 0x8(%ebp),%eax;
    // lea 0x79700(%eax),%edx`（this-first + lea）。探针 /tmp/l29_p3.cpp m6
    // （无 cast）=ORIG 形 / m10（含 cast）=旧形 逐条对照。配套：TU 顶部
    // `#include "PacketTypes.h"`（完整类型；m_name11@+0x11/m_itemId39@+0x39/
    // m_extra3d@+0x3d 经权威头核对）。ORIG 0x08663cc0。
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x5d);
    if (event->IsEventing(0) != true)              // vtable+0x34（槽 13）==1 继续
        return;
    // P23/P200 实参内联右到左求值：ORIG 0x0866407f 段 grow→%esi、job→%ebx、
    // level→%eax 直压（无具名槽）。
    LevelUpRewardItemTableKey key(get_charac_level(), get_charac_job(),
                                  getCurCharFirstGrowType());  // W 0x081b8896
    std::vector<LevelUpRewardItemData> rewards;    // vector C1 0x081b8c00
    if (growupFlag)
    {
        if (!G_CDataManager()->m_levelUpReward.get_data_when_growup(key, rewards))
            return;                                // !=1 清理后出口
    }
    else
    {
        LevelUpRewardItemData data;                // C1 0x081b88bc
        if (!G_CDataManager()->m_levelUpReward.get_data(key, data))
            return;
        rewards.push_back(data);                   // push_back 0x081b8a52
    }
    char body[256] = {0};                          // P214③：ORIG 0x086640c0 段
                                                   // rep stos 内联清零（memset 恒 call）
    strncpy(body, g_scriptStringManager_.findString(
                      4, "game_server_msg_188", (bool*)0),
             0xff);
    for (std::vector<LevelUpRewardItemData>::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        // P167 新判据：ORIG 0x08663e92 调 `operator*`(deEv) 并把结果存 -0x1c 槽
        // （P0 具名引用局部），随后两次字段读取均从该槽解引用；我方旧写
        // `it->m_field0` 发 2 次 `operator->`(ptEv) ⇒ MISSING deEv/EXTRA ptEv。
        LevelUpRewardItemData& data = *it;
        Inven_Item item;                           // local_81（ctor 0x080cb854）
        item.m_addInfo = data.m_field0;            // -0x7b ← data.f0（create_item 前）
        // ORIG 0x08663eb3 段：`call G_CDataManager; mov 0xc(%eax),%eax` 是**成员直读**
        // （CDataManager+0xc = m_itemList），非 getItemList() 调用（P167 EXTRA）。
        G_CDataManager()->m_itemList->create_item(
            ENUM_ITEM_CREATE_TYPE_0, item, data.m_field4);  // T 0x08511fb8
        // P0：ORIG 0x08663ed6..0x08663f64 四值全走 callee-saved 直传（serverGroup
        // →%esi、bodyLen→%edi、characNo→%ebx、mailTitle→%eax 末位），无任何栈槽
        // ⇒ 原码是实参内联右到左求值（具名局部各占 4 槽并致全帧 +0x10）。
        if (WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, "game_server_msg_187",
                                                  (bool*)0),
                item, 0, getCurCharacNo(), body,
                static_cast<int>(strlen(body)), 0, GetServerGroup(),
                0, 0) == 0)
        {
            // [L29 翻正] ORIG 0x08663f76..0x08664005 实证 history 是
            // **MSG_MAILBOX_SEND 局部**（sizeof 0x145，POD：memset 后无 ctor，
            // MSG_BASE 仅 char m_head[0xd]）：①`memset(&history,0,0x145)`；
            // ②strncpy 目标 `lea -0x2c2(%ebp); add $0x11`（成员数组名退化）；
            // ③+0x39/+0x3d 常量 disp 直存（m_itemId39/m_extra3d）；
            // ④`lea 0x79700(%eax),%edx`（this-first）+`lea &history` 直传。
            // 旧 union+reinterpret_cast 形令 4.4 先算实参（lea）再 `add $0x79700`
            // 算 this ⇒ 与 ORIG 反序并折 add。
            MSG_MAILBOX_SEND history;              // 基址 -0x2c2（pack(1)，0x145）
            memset(&history, 0, sizeof(history));
            strncpy(history.m_name11,              // +0x11
                    g_scriptStringManager_.findString(4, "game_server_msg_187",
                                                      (bool*)0),
                    0x14);
            history.m_itemId39 = item.m_addInfo;   // +0x39
            history.m_extra3d = item.m_addInfo2;   // +0x3d
            m_historyLog.SendMail(&history, 0);    // T 0x086847ea
        }
    }
}