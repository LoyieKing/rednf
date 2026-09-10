bool isExistNPCScript(const char* name)
{
    // ORIG 0x089fae70：call loadRDARScriptFile 之后
    // `xor $0x1,%eax; test %al,%al; je +7; mov $0x0,%eax; jmp end; +7: mov $1,%eax`
    // ——je 目标岛是 `mov $1`：load 返 1 ⇒ xor 后 0 ⇒ je ⇒ 返回 1。即 **ORIG 恒等
    // 透传 load 返回值**（早期转写判成 `return !call` 系极性问题，真 bug 修正）。
    // P0/N96 清绑：改 bool callee 裸形（P168a① `if (!call()) return false;`）。
    if (!loadRDARScriptFile(g_npcScriptBaseDirectory, name))
        return false;
    return true;
