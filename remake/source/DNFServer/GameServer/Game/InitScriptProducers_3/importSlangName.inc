// ============================================================================
// importSlangName —— ORIG 0x089081a3
// 报告 _lt_global_gt_/importSlangName.md；调用点 init.md +0x21fd：
//   importSlangName("Etc/slangname.etc", this+0x86a0)。
// [SLANG]/[one byte skip char]/[two byte skip char] 三段分别回填
// SlangNameData 的 vector<string>(+0x00)/vector<uchar>(+0x0c)/
// vector<ushort>(+0x18)。
// ============================================================================
bool importSlangName(char* path, SlangNameData* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string text;
    int value;   // ORIG [ebp-0xc]：两段共用的具名 int 中转（ScanInt 结果先落此槽）

    // ORIG 循环头在函数前部（cond 前置、body 后置、尾部 `jmp` 回边）：
    // `while` 在 4.4 恒旋转为「body 前置 + cond 尾部」，故按 P283/P164 改
    // goto 循环；`if (!ScanType(...)) goto end` 复现 ORIG 的
    // `call ScanType; xor $0x1,%eax; test %al,%al; jne exit`（P292：bool callee）。
slang_loop:
    if (!ScanType(token, true))
        goto slang_end;
    {
        if (token == "[SLANG]")
        {
            while (ScanStr(&text))
                out->m_slangList.push_back(text);
        }
        if (token == "[one byte skip char]")
        {
            // ORIG 形（asm +0xb7..+0xe8）：块级 `bool ok`（**无预清**）+
            // `for(;;){ value=ScanInt(&ok); if(ok) push_back((uchar)value);
            //  else break; }`——ScanInt 结果先落 int 槽 -0xc；push_back 实参是
            // **prvalue 临时**（`mov -0xc,%eax; mov %al,-0xf; lea -0xf`），
            // 具名 ch 左值恒选 RKh 且 4.4 对 `static_cast<T&&>(ch)` 会多物化一次。
            bool ok;
            for (;;)
            {
                value = ScanInt(&ok);
                if (ok)
                    out->m_charList.push_back(static_cast<unsigned char>(value));
                else
                    break;
            }
        }
        // ORIG 089081a3 +0x100：末段 if 的**不匹配**出口落 089081e7(nop) →
        // 089081e8(jmp 089081eb 循环头)——用 == 形（ORIG 符号 _ZSteqIc…）。
        if (token == "[two byte skip char]")
        {
            // 同 [one byte] 段：ORIG 第二段 ok 槽 -0x1a、临时槽 -0xe。
            bool ok;
            for (;;)
            {
                value = ScanInt(&ok);
                if (ok)
                    out->m_shortList.push_back(static_cast<unsigned short>(value));
                else
                    break;
            }
        }
    slang_token_end:;
    }
    goto slang_loop;

slang_end:
    return true;
}