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
            "\xbf\xc0\xb7\xf9 : \xbd\xba\xc6\xae\xb8\xb5 \xc5\xd7\xc0\xcc\xba\xed %d \xb4\xc2 \xb9\xfc\xc0\xa7\xb8\xa6 \xb9\xfe\xbe\xee\xb3\xb3\xb4\xcf\xb4\xd9. "
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
    // [Fn28Gap 2026-09-11 · 符号名轴] ORIG 0x08aa5935/0x08aa5941：
    //   `movl $0x9500be0,(%esp); call ss_sprintf<256>; mov $0x9500be0,%eax`
    // ⇒ not-found 分支用的是**第二个独立** 256B static，nm 直证
    //   `_ZZNK23RDARScriptStringManager10findStringEiPKcPbE6errStr_0`
    //   @0x09500be0（b,0x100；越界分支的 `errStr` @0x09500ae0 是另一符号）⇒
    //   原码显式声明了两个缓冲（同名 static 不可重定义，故第二者名为 errStr_0）。
    static char errStr_0[256];
    ss_sprintf(errStr_0, "%s == NULL, %s : ",
               toTString(key), fileList_.GetFileName(table));
    return errStr_0;
}