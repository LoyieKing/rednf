// pad_certifiaction_check @ 0x0818f4a6（0x29f 指令，static 成员）。
// data+0x38e0(byte)/+0x38e1[0x24]/+0x3905[0x24]/+0x390c(byte)/+0x3914(int)/
// +0x3918、Goblin 检查窗口格式 "%Y-%m-%d 06:00:00"（+0x15180=当日 06:00 对齐秒偏移）。
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
    // [2026-09-11 N4Free191to210.S6 复核·骨牌已翻] 上述「不复用」的**真因**是作用域
    // 层级：旧形把主流程放在函数体（`if {…return}` + 平铺主流程），brokTm 在 if 子
    // 作用域、encryptedPwd 在函数作用域 ⇒ 互为嵌套，4.4 的 stack-slot 复用只在**兄弟
    // 作用域**间生效。把主流程移入 `else` 后（ORIG 0x0818f5c3 即 else 块），
    // **brokTm 与 encryptedPwd 立即共槽 -0x8c、帧 0xc0→0xa0 = ORIG**，
    // 且 if 块内槽区紧跟 ORIG（day/cMyTrace 等位置只余镜像）。
    // 残差（N82/P112 封顶，实测）：if 块内 nowSec/day/trace 三槽相对深度镜像 ——
    // ORIG nowSec=-0x3c、day=-0x38..-0x25、trace=-0x24、shift=-0x14；我方
    // trace=-0x3c、nowSec=-0x2c、day=-0x28..-0x15、shift=-0x14。已试并回滚：
    // `time_t nowSec;` 声明提前到块首（赋值留原位，执行序不变）⇒ nowSec 反落 -0x18、
    // day 落 -0x2c..-0x1c，镜像未复现 ⇒ 4.4 块内分配序与声明序非线性，不可翻。
    // [R2PrettAClass 2026-09-11 · prett A 类落地] 手写签名串 → __PRETTY_FUNCTION__。
    //   ORIG .rodata @0x08b98f20 size 0x56 = "static int ARAD::Arad_Dispatcher_Login_Impl::"
    //   "pad_certifiaction_check(SIG_LOGIN_DATA*)"；本函数是类内 static 成员（.h:168）⇒
    //   GCC 4.4 宏展开自带 `static ` 前缀（换宏三条件②通过：ORIG 串同样含 static）。
    //   定义侧拼写 = `int Arad_Dispatcher_Login_Impl::pad_certifiaction_check(
    //   SIG_LOGIN_DATA* data)`（.inc:1）⇒ 形参拼写与 ORIG 一致（条件①③通过）。
    if (data->m_passPadVersion == 2)
    {
        // 已通过二级认证转化：执行当日 06:00 格式化的 Goblin 奖励复查
        char day[0x14] = {0};

        // [2026-09-11 已试并回滚] 把 `time_t nowSec;` 声明提前到块首（赋值语句留原位）
        // ⇒ 4.4 的块内槽序**不按声明序**平移：nowSec 反落 -0x18（更浅）、day 落
        // -0x2c..-0x1c，ORIG 的 nowSec=-0x3c / day=-0x38..-0x25 / trace=-0x24 镜像未复现
        // ⇒ 与 M1S5 在 updateSSOExpireTime 的 N82/P112「标量/数组相对深度不可翻」同结论。
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

        cMyTrace trace(__PRETTY_FUNCTION__, 0x11a, 0);
        trace("[Goblin Check Time(%s)]]", day);

        return DATABASE::LOGIN_QUERY::arad_sp_check_goblin_reward(data, day);
    }
    else
    {
        // 二级口令校验主流程（ORIG 0x0818f5c3 分支体，与 if 分支互斥；
        // 4.1 互斥块槽复用要求两分支为兄弟作用域：brokTm(-0x8c,0x2c) 与
        // encryptedPwd(-0x8c,0x24) 在 ORIG 共槽即此结构之证据）。
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
    }
}