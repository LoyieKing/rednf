bool DB_LoadActionPoint::Select(_SIG_LOAD_ACTION_POINT& sig)
{
    // 伪 helper 归一（P49-P50）：ORIG 直接调 DBMgr::GetDBHandle，勿经 inline 中继
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    int dailyHour =
        G_CEnvironment()->m_serverEnvir.m_fatigueTime;   // +0x37c
    // [b28d-solo-APSystemOnlyOrig] ORIG：getDailyInitializeTime 结果直落 -0x2c
    // 并作为 localtime_r 实参（无 tt 副本；旧版多 `mov -0x20,%eax; mov %eax,-0x2c`）。
    time_t dailyTime = getDailyInitializeTime(dailyHour, 0, 0);
    struct tm tmc;
    localtime_r(&dailyTime, &tmc);
    char date[16];
    sprintf(date, "%04d-%02d-%02d", tmc.tm_year + 1900,
            tmc.tm_mon + 1, tmc.tm_mday);

    db->set_query("select ap_sum, is_reward_medal, is_reward_item_1, "
                  "is_reward_item_2, is_reward_item_3, is_reward_item_4, "
                  "ap_clear_state  from charac_action_point  where charac_no = %u "
                  " and occ_date = '%s' ",
                  sig.m_characNo, date);
    if (!db->exec(true))
    {
        return false;
    }
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123c88..0x08123c8f：行数落具名局部
    // -0x1c 后 `cmpl $0x1,-0x1c; jne`（无 setne 物化）。
    int nRows = db->get_n_rows();
    // [round5-C05 2026-09-10] ORIG 0x08123c8f `cmpl $0x1,-0x1c; jne 08123f49`
    // ⇒ nRows 的失败出口是**函数尾部独立块** `mov $0,%eax`+（落 leave），而
    // 其余早返都是块内 `mov $0,%eax; jmp <epilogue>`。探针 /tmp/tail_probe.cpp
    // fA/fB 实证：平铺 `if(nRows!=1)return false;` 恒产 `je <cont>; mov $0;
    // jmp`（旧 DIFF 根因），把后续主体嵌套进 `if (nRows == 1) { …return true; }`
    // 并在其后 `return false;` 才产 ORIG 的 `jne <尾部 fail 块>` 形。
    if (nRows == 1)
    {
        if (!db->fetch())
        {
            return false;
        }

        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123cf0..0x08123e37：列号是具名局部
        // -0x18（初值 0，`mov S,%eax; addl $0x1,S` 后置自增），5 次 get_ubyte 复用
        // **同一 flag 槽** -0x39（仅首次 `movb $0x0` 初始化），apSum 直接传
        // `&sig.m_apSum`（ORIG `lea 0x4(%eax),%edx`，无 apSumTmp 临时）。
        // [round5-C05 2026-09-10] T1–T4 对 col 无直证（仅作实参，无符号性消费指令）
        // ⇒ 按 S1 一次性旋钮试 int：ORIG 0x08123e69 段为「先载 blobLen→%eax→
        // 立即 store 0xc(%esp)，再载 col」，我方 unsigned 形 4.4 预计算
        // （`mov blobLen,%edx; mov col,%eax` 后才 store）⇒ 探针 /tmp/arg_probe.cpp
        // t1(unsigned)/t3(int) 逐条实证该差异由实参类型决定；int 形与 ORIG 全等。
        int col = 0;
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123cb8：col 之后另有
        // `movl $0x0,-0x14` 死局部（全函数无读点，P171 照抄）。
        int deadSlot = 0;
        (void)deadSlot;
        // packed 字段不能直接绑定到 unsigned int&（GCC 报错，且 reinterpret_cast
        // 自 &sig.m_apSum 仍被追踪）⇒ 经 char* 基址 + offsetof 转写；codegen 仍为
        // ORIG 的 `lea 0x4(%eax),%edx`。
        if (!db->get_uint(col++, *reinterpret_cast<unsigned int*>(
                                     reinterpret_cast<char*>(&sig) + 4)))
        {
            return false;
        }
        unsigned char flag = 0;
        if (!db->get_ubyte(col++, flag)) return false;
        sig.m_isRewardMedal = (flag != 0);
        if (!db->get_ubyte(col++, flag)) return false;
        sig.m_isRewardItem[0] = (flag != 0);
        if (!db->get_ubyte(col++, flag)) return false;
        sig.m_isRewardItem[1] = (flag != 0);
        if (!db->get_ubyte(col++, flag)) return false;
        sig.m_isRewardItem[2] = (flag != 0);
        if (!db->get_ubyte(col++, flag)) return false;
        sig.m_isRewardItem[3] = (flag != 0);

        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123e3a：`cmpl $0x7ff,-0x10; jbe`
        // （无符号比较）⇒ 局部类型 unsigned。
        unsigned int blobLen = db->get_binary_length(col);   // 第 7 列 ap_clear_state；>0x7ff 拒绝
        if (!(blobLen <= 0x7ff))
        {
            return false;
        }
        memset(g_szZipAP_CLEAR_STATE, 0, ConstMaxZipAP_CLEAR_STATE);
        if (!db->get_binary(col, g_szZipAP_CLEAR_STATE, blobLen))
        {
            return false;
        }
        unsigned int raw = *reinterpret_cast<unsigned int*>(g_szZipAP_CLEAR_STATE) &
                           0x3fffffff;
        if (!(raw <= 0x7ff))
        {
            return false;
        }
        memset(g_szStrAP_CLEAR_STATE, 0, ConstMaxStrAP_CLEAR_STATE);
        unsigned int outLen = raw;
        if (!uncompress_zip(g_szStrAP_CLEAR_STATE, outLen, g_szZipAP_CLEAR_STATE,
                            static_cast<unsigned int>(blobLen)))
        {
            return false;
        }
        // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123f2f..0x08123f4e：`call; xor $1;
        // test; je ok; mov $0,%eax; jmp ret; ok: mov $1,%eax; jmp ret; fail: mov
        // $0,%eax`（ok 路径的 jmp 跳过尾部 fail 块）。
        if (!ParseActionClearState(g_szStrAP_CLEAR_STATE, sig.m_actionPoints, 300))
        {
            return false;
        }
        return true;
    }
    return false;
