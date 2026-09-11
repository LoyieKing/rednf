// b28d-solo-InitScriptProducers_1: DIFF→AE（0x206=0x206，指令数全等）。修：外层
// slot 循环由逗号条件 `while ((slot=ScanInt(&ok), ok==true))`（4.4 旋转成底部条件）
// 改为 `while (true) { slot = ScanInt(&ok); if (ok != true) break; ... }`，复现
// ORIG 条件块置顶 + 体尾无条件回跳（P25/P283；ORIG 0x08902364 读 slot →
// 0x08902372 `movzbl ok; xor $1; test; je 体`）。
// ORIG T 0x089022be。[world drop] 段：slot 步长 0x14 寻址；槽内首值写 +0x00，
// 总量清零后成对读 (itemIdx, rate)：rate 累加 +0x10、chunck 入 +0x04 vector；
// 段内所有出口统一执行 +0x00 = +0x10。itemIdx==-1 即终止本槽。slot 无越界检查（忠实）。
bool ImportWorldDrop(WorldDropInfo* out, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        // 字面量直传（不建具名 static 数组）：ORIG nm 无
        // `_ZZ15ImportWorldDropP13WorldDropInfoPKcE8kFailMsg`，字符串为匿名 .rodata
        // （ORIG 0x08e0fafc），仅 `__FUNCTION__` 局部静态符号存在（§4.1 仅NEW 清理）。
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp", __FUNCTION__, 0xd36,
                              "CRITICAL ERROR : \xbf\xf9\xb5\xe5\xb5\xe5\xb7\xd3 "
                              "\xbd\xba\xc5\xa9\xb8\xb3\xc6\xae \xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0!");
        return false;
    }

    std::string token;                         // ORIG local_1c
    int dead18 = 0; (void)dead18;              // ORIG local_18 prologue $0 写
    bool ok = false;                           // ORIG local_1d

    // ORIG 0x08902342：`call ScanType; xor $1; test; jne` ⇒ bool callee 取反早退形。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[world drop]")
        {
            int slot;
            // b28d-lane25 复测：ORIG 条件块在循环体**顶部**（+0xa6 读 slot → +0xb4
            // `movzbl ok; xor $1; test; je 体`），体尾 +0x1ca 无条件回跳条件；4.4 对
            // 逗号 while 恒旋转成底部条件。改 while(true)+`if (ok != true) break;` 复现。
            while (true)
            {
                slot = ScanInt(&ok);
                if (ok != true)
                    break;
                out[slot].m_first = ScanInt(&ok);       // +0x00（无 ok 检查，忠实）
                out[slot].m_total = 0;                  // +0x10
                int n = 0;                              // ORIG local_10
                WorldDropItemChunck chunk;              // ORIG local_28/local_24 相邻槽
                // ORIG 0x08902449：`cmpl $0xff,-0xc; setle %al; test %al,%al; jne` ⇒
                // 边界条件独立 while + 内层 itemIdx 早退，无具名 bool 局部（P0）。
                while (n <= 0xff)
                {
                    chunk.m_itemIdx = ScanInt(&ok);
                    if (chunk.m_itemIdx == -1)
                        break;
                    chunk.m_rate = ScanInt(&ok);        // ORIG local_24
                    out[slot].m_total += chunk.m_rate;  // ORIG 每次重算 slot*0x14（无缓存）
                    out[slot].m_chunks.push_back(chunk);
                    if (ok != true)
                    {
                        break;
                    }
                    n = n + 1;
                }
                out[slot].m_first = out[slot].m_total;  // 收敛写回
            }
        }
    }
    return true;
}
