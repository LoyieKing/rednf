int Arad_Dispatcher_Login_Impl::pad_certifiaction_check(SIG_LOGIN_DATA* data)
{
    // [SweepARADLoginSecurity 2026-09-10 判词·NEAR(N82/N93)] ORIG 0x0818f4a6：
    // 158=158、p167 CLEAR。已修：P214③ 两缓冲 `={0}` 复现 rep stos
    // （inputPadPwd -0x60 / encryptedPwd）；day[0x14] `={0}` 复现 ORIG 5×movl；
    // nowSec 单变量复用
    // （ORIG -0x3c 二次写=`getCurSec() - shift` 初始化式）；savedHash/failCnt
    // 具名指针局部（ORIG -0x10/-0xc，失败分支经指针自增）；strncmp `!= 0` 走
    // 失败 fall-through（ORIG `je <成功块>` 布局）；`failCnt > 9` 早返（ORIG
    // `jle` 落 errCode=5 块）。残余=帧 0xc0 vs 0xa0 + 全帧槽号平移：ORIG 的
    // brokTm(-0x8c, 0x2c) 与 encryptedPwd(-0x8c, 0x24) **复用同槽**（4.1 互斥块
    // 复用），4.4 在本函数不复用（brokTm -0xb0 / encryptedPwd -0x84）。
    // 本轮复核（判词维持 N，附实测）：① brokTm 提前到函数头、或置于 if 块首，
    // 两形均不改变 brokTm/encryptedPwd 槽位（帧仍 0xc0）；② 4.4 本身**会**复用
    // 互斥块槽（探针 /tmp/p_arad/t16.cpp f1-f3、t17.cpp f4-f6 含取地址实参均
    // `sub $0x30/$0x48` 单槽复用）⇒ 本函数的不复用源于 brokTm 的地址逃逸
    // （localtime_r/strftime 实参）与 encryptedPwd 的 `={0}` rep stos 组合，
    // 非声明序可控；③ 显式 union 复用会改 rep stos 清零形（N78 memset 退化）。
    // ⇒ N82/N93 封顶。
    static const char kFunc[] =
        "static int ARAD::Arad_Dispatcher_Login_Impl::pad_certifiaction_check("
        "SIG_LOGIN_DATA*)";
    if (data->m_passPadVersion == 2)
    {
        // 已通过二级认证转化：执行当日 06:00 格式化的 Goblin 奖励复查
        char day[0x14] = {0};

        int shift = 0;
        time_t nowSec = GlobalData::s_systemTime_.getCurSec();
        struct tm brokTm;
        localtime_r(&nowSec, &brokTm);

        if (brokTm.tm_hour < 6)
        {
            shift = 0x15180;   // 回退到前一日 06:00（86400 秒）
        }

        nowSec = GlobalData::s_systemTime_.getCurSec() - shift;
        localtime_r(&nowSec, &brokTm);

        strftime(day, sizeof(day), "%Y-%m-%d 06:00:00", &brokTm);

        cMyTrace trace(kFunc, 0x11a, 0);
        trace("[Goblin Check Time(%s)]]", day);

        return DATABASE::LOGIN_QUERY::arad_sp_check_goblin_reward(data, day);
    }

    // 二级口令校验主流程
    data->m_passPadVersion = 1;

    char inputPadPwd[0x24] = {0};   // ORIG -0x60(%ebp) 输入缓冲（rep stos 清零）
    char encryptedPwd[0x24] = {0};  // ORIG -0x8c(%ebp) 输出缓冲（rep stos 清零）

    strncpy(inputPadPwd,
            data->m_passPadPassword + 0x24, 0x24);   // +0x3905

    ARAD_CRYPT::encryptAES(GlobalData::s_pcryptRijndael_MousePassword_,
                           inputPadPwd, encryptedPwd);

    // ORIG 0x0818f633/0x0818f63e：两个具名指针局部（-0x10 savedHash / -0xc failCnt）
    const char* savedHash = data->m_passPadPassword;   // +0x38e1
    int* failCnt = &data->m_passPadFailCnt;            // +0x3914

    if (strncmp(savedHash, encryptedPwd, 0x24) != 0)
    {
        *failCnt = *failCnt + 1;
        DATABASE::LOGIN_QUERY::arad_sp_update_fail_record(data);
        if (*failCnt > 9)
        {
            data->m_errCode = 6;
            return 0x49;
        }
        data->m_errCode = 5;
        return 0x48;
    }

    data->m_passPadFailCnt = 0;
    data->m_passPadCancelCnt = 0;
    if (DATABASE::LOGIN_QUERY::arad_sp_update_history(data, 7) != 0)
    {
        data->m_errCode = 6;
        return 1;
    }
    if (DATABASE::LOGIN_QUERY::arad_sp_delete_member_mouspass(data) != 0)
    {
        data->m_errCode = 6;
        return 1;
    }
    data->m_passPadEnable = 0;
    return 0;
