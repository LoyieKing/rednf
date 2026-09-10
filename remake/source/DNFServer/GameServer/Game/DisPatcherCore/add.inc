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
