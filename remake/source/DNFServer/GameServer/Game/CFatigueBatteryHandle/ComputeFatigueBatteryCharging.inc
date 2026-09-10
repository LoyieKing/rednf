int CFatigueBatteryHandle::ComputeFatigueBatteryCharging(int charge, int level,
                                                         int specialCnt)
{
    // [b28d-solo-CFatigueBatteryHandle] verdict = N82/N112 + 跨版本截断读 封顶。
    // 指令序列与 ORIG 逐条同形（imul / movzbl×2 / idivl / fildl×2 / flds / fmulp /
    // fld1 / faddp / fmulp / fnstcw / fldcw / fistpll / fldcw 全等），p167 CLEAR
    // （1 call G_CDataManager 命中）。残差二项，均无语义影响：
    // ① 帧 0x48 vs ORIG 0x38 + 全槽号错位（4.4 逆首次使用序分配，N98/P212补）：
    //    4.4 divisor 临时 -0x3c / x87 cw -0x2c,-0x2a / sp -0x20 / truncated -0x18；
    //    ORIG 分别 -0x2c / -0x20,-0x1e / -0x10 / -0x28。b28d-solo 探针
    //    /tmp/s99/p4.cpp(A-D)+p5.cpp(E-H) 八形实测：具名 divisor 帧回 0x38 但多一条
    //    `mov sp,%eax` 且除数装载序反（ORIG 的 -0x2c 是表达式内联临时槽，非具名）；
    //    low 提前声明使 sp 落 -0x10 却把 quotient/low 槽互换；指针/两步赋值变体同槽。
    // ② ORIG 尾 `mov -0x28(%ebp),%eax; mov -0x24(%ebp),%edx` 64 位读，4.4 对
    //    `(unsigned int)long long` 恒只读低 32 位（跨版本截断读；高 32 位不入返回，
    //    gap=1 即此条）。
    // b28d-20: ORIG 0x084a8004 — sp 引用（add $0x68，字段 0x5c0/0x5c4/0x5c8）；
    // 无 long double 具名局部：ORIG x87 链 `fildl quotient; fildl specialCnt;
    // flds rate; fmulp; fld1; faddp; fmulp` 即 quotient 先压栈的
    // `quotient * (specialCnt*rate + 1.0L)` 内联实参形（P243 书写序定 flds 序：
    // 先 fildl specialCnt 再 flds rate，故 specialCnt 写在乘法左侧）。
    // 残余 ndiff 16 封顶 N82：①帧 0x48 vs ORIG 0x38、long long truncated 槽方向
    // 相反（4.4 -0x18 浅于 sp/-0x20；ORIG -0x28 深于 sp/-0x10；声明序两变体
    // truncated-first/后置实测槽位集不可翻，P112/N98）；②ORIG 尾
    // `mov -0x28(%ebp),%eax; mov -0x24(%ebp),%edx` 64 位读，4.4 对
    // `(unsigned int)long long` 恒只读低 32（跨版本截断读）。quotient/-0x1c、
    // low/-0xc、除数临时/-0x2c 语义位点与 ORIG 逐条同序。
    ServerParameterScript& sp = G_CDataManager()->m_serverParameter;

    int quotient = (charge * level) /
                   static_cast<int>(static_cast<unsigned int>(sp.m_fatigueTable5c0.m_chargeDivisor8));

    long long truncated = static_cast<long long>(
        static_cast<long double>(quotient) *
        (static_cast<long double>(specialCnt) *
             static_cast<long double>(sp.m_fatigueTable5c0.m_chargeRate4) +
         1.0L));  // fistpll RC=truncate

    unsigned int low = static_cast<unsigned int>(truncated);
    if (static_cast<unsigned int>(sp.m_fatigueTable5c0.m_capLimit) < low)  // 无符号比较
        low = sp.m_fatigueTable5c0.m_capLimit;
    return static_cast<int>(low);
