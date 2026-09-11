// ORIG 0x088bd381
// ORIG 0x088bd381：switch(tok) 树（cmp $0x3 为根；DECIMAL=2/FLOAT=4/STRING=7）。
bool scanScriptValue(ScriptValue& v)
{
    // getToken() 返回 DNFLEX_TOKEN（ORIG 底层 int）；本 TU 不引入该枚举类型
    // （见顶部 P338 说明），用 int 承载——二进制与枚举形相同。
    int tok = g_dnfLex.getToken();
    switch (tok)
    {
    // [b28d-ScriptValue P324] 叶块物理序 = 书写序：ORIG 0x3ca(STRING)/0x4e1(DECIMAL)/
    // 0x509(HEXADECIMAL)/0x531(FLOAT) ⇒ 书写序照此排列（决策树按值域，与本序无关）。
    case DNF_TOK_STRING:
        {
        std::string key = toTString(g_dnfLex.getString());
        // [b28d-ScriptValue P333/P177] ORIG 0x088bd381 STRING 段 = 「首建表、之后查表」
        // 状态机（ORIG 指纹：0x610/0x6e5/0x808 三处 `movzbl __inited; xor $1; test; je`；
        // insert 尾 `jmp` 下一 case 体头；`movb $0x1,__inited` 后 `jmp 0x427` 回跳重查）。
        // retry 标签置于 static 三件套声明之前 ⇒ 每轮穿 guard（P177）；id==0 为建表哨兵，
        // case 0 与 case 0x674 合并体 + fallthrough 链一次调用插完两键。符号名照 ORIG
        // （_ZZ…E8__lookup / E11__init_lock / E8__inited，P187 双侧核对）。
    retry:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int id;
        if (__inited)
        {
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(key.c_str());
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();
            if (__inited)
            {
                __init_lock.leave();
                goto retry;
            }
            id = 0;
        }

        switch (id)
        {
        case 0:
        case 0x674:               // "skill level info"
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("skill level info", 0x674));
                // 不 break：首轮建表贯穿到下一 case 继续插键（ORIG fallthrough 链）
            }
            else
            {
                v.type = 1;
                if (!ScanFloat(&v.value[0])) return false;
                if (!ScanFloat(&v.value[1])) return false;
                if (!ScanFloat(&v.value[2])) return false;
                return true;
            }
        case 0x686:               // "object number"
            if (!__inited)
            {
                __lookup.insert(std::pair<const char* const, int>("object number", 0x686));
                break;
            }
            else
            {
                v.type = 2;
                if (!ScanStr(&key)) return false;
                // ORIG 0x088bd76b：零扩展 64 位（`mov $0,%edx`）→ fildll → fstps
                v.value[0] = static_cast<float>(
                    static_cast<long long>(getTargetToEnumValue(key)));
                if (!ScanFloat(&v.value[1])) return false;
                if (!ScanStr(&key)) return false;
                v.value[2] = static_cast<float>(getActiveStatusToEnumValue(key));
                if (!ScanFloat(&v.value[3])) return false;
                return true;
            }
        default:
            break;
        }

        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            goto retry;
        }
        else
        {
            return false;
        }
        }
    case DNF_TOK_DECIMAL:
        v.type = 0;
        v.value[0] = static_cast<float>(g_dnfLex.getDecimal());
        return true;
    case DNF_TOK_HEXADECIMAL:
        v.type = 0;
        v.value[0] = static_cast<float>(g_dnfLex.getHexDecimal());
        return true;
    case DNF_TOK_FLOAT:
        v.type = 0;
        v.value[0] = g_dnfLex.getFloat();
        return true;
    default:
        return false;
    }
}