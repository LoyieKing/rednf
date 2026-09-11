// ============================================================================
// importImageCommunicationInfo —— ORIG 0x08913dbc
// 报告 docs/class_func_reports/_lt_global_gt_/importImageCommunicationInfo.md
// 调用点（init.md +0x0966）：CDataManager::init → this+0xb4c4
//   （CDataManager::m_imageCommData，ImageCommunicationData，写 +0x00 int）。
// 段键："[charge]" 写首字；"[/image communication]" 结束返回 1。
// 验收：gap 12→1；差集 10/10 归零；残余 = ORIG 三态汇聚 ebx+终值 esi 双
// 寄存器物化（4.1 choose-without-store，N3/N76 族）。
// ===========================================================================
// [b28d-solo-RDARScriptEtcParameter] AE：ORIG 0x08913dbc；本波把两处 `return 1`
// 改为 `break` + 循环外单一 `return 1`（P283/P63）⇒ 4.4 复现 ORIG 三态共享尾
// （state 0=返回 result / 1=退出返回 1 / 2=继续，result/state 全程 esi/ebx 无槽），
// 112=112 归一化逐条全等（ndiff 45→0）。p167 CLEAR（10/10），语义闭合。
// [SplitC batch10 · 判据] 权威返回类型 = **bool**：ORIG 唯一外部调用点 `_ZN12CDataManager4initEv`
//   的 `call` 后为 `xor $0x1,%eax` + `test %al,%al`（运算符 ! 的 8 位语义）
//   ⇒ 与 DataManagerInitDepends.h 的 bool 声明一致；原 int 为历史错修。
bool importImageCommunicationInfo(const char* path, ImageCommunicationData* out)
{
    if (!loadRDARScriptFile("", path))
    {
        LogManager::logFormat(0, "../RDARScriptEtcParameter.cpp",
                              __FUNCTION__, 0x3233,
                              // Q7S4: ORIG 0x08e103d8 起 34 字节；我方此前 \xbe\xe8('얕') 错字，
                              // ORIG 为 \xbe\xf8('없')。
                              "ERROR : %s \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.", path);
        return 0;
    }
    // [b28d-solo-RDARScriptEtcParameter] ORIG 0x08913dbc：循环体内每轮重置
    // isType(-0xd)/ok(-0xe)+构造 token(-0x14)、尾析构；ScanTypeOrInt 返回值落
    // -0xc 死槽（照抄 P171）。ORIG 真形 = 三态共享尾（state 0=返回 result /
    // 1=退出循环返回 1 / 2=继续；result/state 全程 esi/ebx 无槽往返 ⇒ 原码用
    // break+循环外 return 1，而非循环内 return 1，P283/P63 族）。
    for (;;)
    {
        bool isType = false;
        bool ok = false;
        std::string token;
        int ret = ScanTypeOrInt(isType, token, ok, true);
        (void)ret;
        if (!ok)
            return 0;
        if (isType && token == "[charge]")
        {
            out->m_charge = ScanInt(&ok);
            if (!ok)
                return 0;
            break;
        }
        if (isType && token == "[/image communication]")
        {
            break;
        }
    }
    return 1;
}