const char* RDARScriptStringManager::findString(int table, const char* key,
                                                bool* flag) const
{
    // ORIG 0x8aa57fe
    // b28d-21: DIFF/N 封顶 — ORIG 0x8aa57fe 中一处实参为 `movl $<A>,(%esp)` 直压
    // 立即数（rodata/全局），重建为 `mov this; add $0x18` 取成员地址（对象/调用
    // 形态差）+ best 指针槽位差 4（N82）。语义/调用序全等。
    if (key == 0)
    {
        return "RDARScriptStringManager::ERROR::input NULL string";
    }

    static char errStr[256];
    if (table >= static_cast<int>(dictionaries_.size()))
    {
        return "RDARScriptStringManager::ERROR::requested fileIndex isn't in string table";
    }

    // ORIG 0x8aa583d：第二处范围检查取**全局对象** g_scriptStringManager_ 的
    // vector（`movl $&g_scriptStringManager_+0x18,(%esp)`），非 this 成员。
    if (table < 0 || table >= static_cast<int>(g_scriptStringManager_.dictionaries_.size()))
    {
        LogManager::logFormat(
            0, "../RDARScriptStringManager.cpp", __FUNCTION__, 0x11c,
            "오류 : 스트링 테이블 %d 는 범위를 벗어납니다. "
            "(index out of range reading string table %d)",
            table);
        ss_sprintf(errStr, "String table index out of range %d", table);
        return errStr;
    }

    const char* result = dictionaries_[table]->get(toTString(key));
    if (flag != 0)
        *flag = (result != 0);
    if (result != 0)
        return result;

    // ORIG 0x8aa5918：GetFileName 结果存 callee-saved %ebx 跨 toTString 调用
    // （无槽往返）⇒ 实参内联（P0/P23）。
    ss_sprintf(errStr, "%s == NULL, %s : ",
               toTString(key), fileList_.GetFileName(table));
    return errStr;
