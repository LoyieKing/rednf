// ============================================================================
// importEmotionTypeTagsScript —— ORIG T 0x0890de85
//   bool (const char*, stEmotionTypeTagsInfo*)
// [EMOTION INFO] 内：tag 入 m_tags(+0x00) 且记入 stOneEmotionData.m_0；
// 随后 4 值/3 布尔/字符串族填充元素后 push 入 m_data(+0x0c)；$END 结束
// m_18 字符串列表。任一 Scan 失败 → false。
// ============================================================================
// b28d-solo-InitScriptProducers_6: DIFF/N 封顶 — ORIG 0x0890de85。已修：load 检查
// 最前；函数级 token(-0x10)/unused(-0x14)/ok(-0x15)，out 判空在声明之后；`str`
// 改为 [EMOTION INFO] 块内局部（ORIG -0x1c）。p167 由 3 MISSING 转 CLEAR（29=29）。
// 残差 N86/N89（string 构造/析构族）+ N82 槽 ⇒ N 封顶。
// [SweepInitProd9 本轮] fresh 读数：origN 234 / ourN 245（gap −11），jcc 38/43
// （多 5 个）。逐位核对：3 处 `data.m_10/11/12 = (v==1)?1:0` 的 ORIG 形是
// `mov slot,%eax; cmp $1; sete %al`（无分支），4.4 恒折 `cmpl $1,slot; jne;
// mov $1; jmp; …`（每处 +1 jne +1 jmp）⇒ 属 P197④/setcc-fold 负例（4.1 槽比
// sete 不可复现，探针族已证），非语义差；其余 2 段为块首落点差。语义等价。
// [C03CryptoInit68 修·2026-09-10（推翻 N197④ 判词）] 原 `data.m_1x = (v==1) ? 1 : 0`
// 产**分支形**（`cmpl $1,slot; jne; mov $1; jmp; mov $0`）；ORIG 是
// `mov -0x20(%ebp),%eax; cmp $0x1,%eax; sete %al; mov %al,-0x34(%ebp)`。
// **钥匙 = 直接 bool→字节字段赋值**：`data.m_10 = (v == 1);`（不加三目、不加
// static_cast）⇒ 4.4 逐条复现 ORIG 的 `cmp/sete/直存`，三处同改后
// **ourN 248 → 239 = ORIG（gap −11 → 0）**。
// （反例留痕：`static_cast<char>(v == 1)` 与三目同产分支形；`unsigned int v` 局部
//  不改变内存直比；补 `int v4` 具名中转 ⇒ 239→241 且全槽整体深 4，已回退。）
// 残余（跨版本族，语义等价）：① 比较形 —— ORIG `mov -0x20,%eax; cmp $1,%eax`
// 载寄存器 vs 我方 `cmpl $1,-0xc` 内存直比（P209/P286 族，局部实参 int 不可择）；
// ② 失败路径返回寄存器 %ebx(ORIG) vs %esi(我方)（N7/N18a 寄存器角色）；
// ③ ORIG 0x0890dedf 一处 `jmp+nop` 岛（P223/N112）。
// p167 复核：本件 MISSING/EXTRA 全为 C1↔C2 同址别名伪差（`_ZNSsC2Ev` vs `_ZNSsC1Ev`、
// stOneEmotionData C1/C2），无真语义缺口（P93）。
bool importEmotionTypeTagsScript(const char* path, stEmotionTypeTagsInfo* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                  // ORIG -0x10
    std::string unused;                 // ORIG -0x14（仅构造/析构，零引用）
    bool ok = false;                    // ORIG -0x15

    if (out == 0)
    {
        return false;                               // ORIG 0x890decb 判空
    }

    for (;;)
    {
        if (!ScanType(token, true))
        {
            return true;
        }

        if (token == "[EMOTION INFO]")
        {
            stOneEmotionData data;                  // ctor 0x891b560 + clear
            std::string str;                        // ORIG -0x1c（块内构造）
            int tag = ScanInt(&ok);
            if (!ok) { return false; }
            out->m_tags.push_back(tag);
            data.m_0 = tag;

            // [C03 实测] ORIG 0x0eb/0x103 的 `mov %eax,-0xc; mov -0xc,%eax; mov %eax,-0x40`
            // 是 4.1 的具名中转；我方补 `int v4 = ScanInt(&ok); data.m_4 = v4;` 后
            // ourN 239→241 且全槽整体深 4（新槽未复用 ORIG 的 -0xc）⇒ 反劣，已回退。
            data.m_4 = ScanInt(&ok);
            if (!ok) { return false; }
            if (!ScanStr(&data.m_8))
            {
                return false;
            }
            if (!ScanStr(&data.m_c))
            {
                return false;
            }
            int v = ScanInt(&ok);
            if (!ok) { return false; }
            // ORIG `mov -0x20(%ebp),%eax; cmp $0x1,%eax; sete %al; mov %al,-0x34(=data.m_10)`
            // ⇒ 直接 bool→字节字段赋值即复现 sete 形（勿写 `(v==1)?1:0`：那会产分支形）。
            data.m_10 = (v == 1);
            v = ScanInt(&ok);
            if (!ok) { return false; }
            data.m_11 = (v == 1);
            v = ScanInt(&ok);
            if (!ok) { return false; }
            data.m_12 = (v == 1);

            if (!ScanStr(&data.m_14))
            {
                return false;
            }
            for (;;)
            {
                if (!ScanStr(&str))
                {
                    break;                          // 列表终止（含 $END 缺失情形）
                }
                if (str == "$END")
                {
                    break;
                }
                data.m_18.push_back(str);
            }
            out->m_data.push_back(data);
        }
    }
}