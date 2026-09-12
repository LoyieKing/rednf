// ---------------------------------------------------------------------------
// loadFromFile —— ORIG T 0x08ad4ce4：fopen(path,"rb")（模式串 @0x8e32c38）；
// 失败且 errOut!=0 → *errOut="Cannot open file"@0x8e32c3b 并返回 0。
// 成功：fseek end/ftell/回 0 → fread 整体 → fclose →
// loadFromBinary(buf,size,err) → delete[]，透传结果（size<0 失败返 0）。
// ---------------------------------------------------------------------------
// [b28d-solo-StringResource N80/P75: loadFromBinary 三实参 ORIG 右→左全 %eax 串行
// （error→0xc, fileSize→0x8, buf→0x4, this→0x0）、4.4 左→右先取 buf 入 %eax、arg2/3 走 %edx；
// A(int fileSize)/B(具名 binData)/C((int)cast)/D(B+C) 四变体实测均劣（具名形多 2 条且槽漂）
// | ORIG 0x08ad4ce4]
// [M2S5StringRaw 2026-09-11 复核] 尾部 loadFromBinary 3 行装载序（ORIG 右→左全
// %eax 串联；4.4 arg0 预载 %eax、arg2/arg3 走 %edx）：本波追加新钥匙探针
// /tmp/probeF1（现行形）/F2（P226 语句表达式 `({ ... })`）实测两形逐条相同
// ⇒ N80 封顶维持（旧 5 形见 /tmp/p26_lf*.cpp）。
// [P8S5 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
bool StringResource::loadFromFile(const char* path, CharString* error)
{
    FILE* fp = fopen(path, "rb");
    // [SemB09 s2_09] 真差异修正：ORIG 0x08ad4ce4 的两条守卫 `jne +0x45`（fp != NULL
    //   跳过）与 `je +0x45`（error == NULL 跳过）**共落同一目标**（fseek 主路径）⇒
    //   原码是单一 `if (fp == NULL && error != NULL) {…return false;}`（&& 短路，
    //   4.1/4.4 同形），而非「外层 if(fp==NULL){ if(error)…; return false; }」。
    //   两者在 fp==NULL && error==NULL 时行为不同：前者落 fseek 主路径（ORIG 实际
    //   行为，fseek/ftell 对 NULL 返 -1 ⇒ jns 不成立 ⇒ fclose+return false），
    //   后者立即 return false（我方旧形）。按 ORIG 行为照抄原则（P171）还原。
    //   调用点核查：全仓唯一调用者 loadString(CharString*) 由 getStr_1/loadString.inc
    //   传栈上 &err（非 NULL），无 fp==NULL && error==NULL 的实参路径。
    if (fp == NULL && error != NULL)
    {
        *error = "Cannot open file";
        return false;
    }
    fseek(fp, 0, SEEK_END);
    const long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fileSize < 0)
    {
        fclose(fp);
        return false;
    }
    // ORIG 各使用点直接读 ftell 槽（-0x14）：无具名 int 中间量（P183）。
    // [N1F 2026-09-11] 缓冲区用 unsigned char*（ORIG 0x08ad4dbf 实参装载形：err→0xc/
    //   size→0x8/buf→0x4/this→0x0 逐条 `mov <src>,%eax; mov %eax,N(%esp)`，**无** cast
    //   节点的预取；带 reinterpret_cast 时 4.4 把 buf 预载入 %eax 并改走 %edx ⇒ 形态不符）。
    unsigned char* buf = new unsigned char[fileSize];
    fread(buf, 1, (size_t)fileSize, fp);
    fclose(fp);

    /* loadFromBinary 实参装载 N80 族（b26 NL26 旧结论「恒交错」已被上条重定类型推翻）：
     * 去 cast 后 4.4 逐条 `mov %eax` 直落，与 ORIG 同形。 */
    const bool result = loadFromBinary(buf, fileSize, error);
    delete[] buf;
    return result;
}