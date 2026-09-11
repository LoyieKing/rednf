// [b28d-29] N78：ORIG 手写 0x400 字节对齐清零展开（and/test/字节+字+双字三段），4.4 恒 call memset；语义等价  (ORIG 0x0860fdd2 insn 106->74)
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 74（ORIG 106 / ours 74），残差=N7/N82 帧与 callee-saved 角色/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
// [b28d-TakeDispatchCore] **P312 翻案（N78 突破口，原「4.4 恒 call memset 不可翻」作废）**：
// `char key[0x400] = {0};`（原 `memset(key,0,sizeof(key))`）复现 ORIG 0x0860fddd 的
// 对齐剥离+rep stos 慢路 ⇒ ours 77→111、ndiff 58→19、gap 31→−2；
// p167 11/11 CLEAR（余 StatisticsPacket C1↔C2 同址别名对=豁免）。
void add(CUser* user, const char* tableName, const char* fieldName,

         unsigned int value, const char* fmt, ...)
{
    char key[0x400] = {0};          // P312：ORIG 0x0860fddd 内联对齐剥离+rep stos 慢路
    va_list args;
    va_start(args, fmt);
    vsprintf(key, fmt, args);   // ORIG 无上限包装，忠实转写
    va_end(args);
    if (strlen(tableName) > 0x20 || strlen(fieldName) > 0x20 || strlen(key) > 0x38)
    {
        return;
    }
    StatisticsPacket packet;
    strcpy(packet.m_tableName, tableName);
    strcpy(packet.m_fieldName, fieldName);
    strcpy(packet.m_key, key);
    packet.m_value = value;
    // [Lane5] P36/P52：ORIG 无 proxy 具名局部（`call GetServerGroup; mov mgr,%edx;
    // call GetServerProxy; mov %eax,(%esp); call SendPacket`），且 `lea packet,%ebx`
    // 提前到 GetServerGroup 之前 ⇒ 链式单语句（&packet 跨调用由 ebx 承载）。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x89);
}
