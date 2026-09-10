char* CStringManager::getStr(int id)
{
    // ORIG _ZZN14CStringManager6getStrEiE8errorMsg：bss 0x20 字节 TLS 形式
    // 访问（asm `mov %gs:0x0,%eax; lea disp(%eax)` 两段、参数位与 return 各
    // 重算一次——__thread 局部静态，无共享槽缓存，P183）。
    static __thread char errorMsg[32];
    if (!is_sr_loaded)
    {
        // ORIG @0x30-0x48 双岛：return 0 在 err 作用域内 → 失败路径与正常路径
        // 合流到共享析构 cleanup（ebx=0/esi=0 vs esi=1，test %esi 决定走向）。
        {
            CharString err;
            if (loadString(&err))
            {
                is_sr_loaded = true;
            }
            else
                return 0;
        }
    }

    const CharString* entry;              // ORIG 无初始化（直接取址传参）
    if (app::stringResource()->get(id, &entry))
        return const_cast<char*>(entry->c_str());

    ss_sprintf(errorMsg, "getStr(%d) == none", id);
    return errorMsg;
