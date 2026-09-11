// ORIG T 0x081730d6：param==""（operator== 成立）走默认公钥，否则指定路径；
// 返回对应 Init 的布尔结果。
// [语义轴·分支反转] ORIG 0x0817313b `je 0x0817314b`，落点体 0x0817313d
// `movl $0x9404754,(%esp); call _ZN11COpensslRSA4InitEv`（无参）为 **相等臂**；
// 不等臂 0x0817314b..0x08173159 压 path 后 call `_ZN11COpensslRSA4InitEPKc`。
// 旧形两臂写反（相等时调 Init(path)）⇒ path 为空串时行为不同（ORIG 用默认
// publickey.pem 路径，旧形传空路径给 openssl）。
// [b28d-solo-D02] verdict=DIFF（96 vs 91，N95/N104 族封顶）。本轮已修：
// ①arg 去判空三目（P171 照抄：ORIG 0x08173115 直接压 path 形参）；
// ②empty/arg 的 allocator 临时形与 ORIG 同构（0x8b83f20 空串字面量）。
// 残差 = 两 string 与 allocator 临时槽的**绝对槽位**（ORIG empty -0x18 /
// alloc -0x11 / arg -0x10 / alloc -0x9；我方 empty -0x10 / arg -0x14，方向相反）
// 与 unwind 清理链块序（ORIG -0x10→-0x9→-0x18→-0x11）——即 4.1「先声明深」
// vs 4.4「先声明浅」的槽序（P309/N95）+ EH 块排列（N104）。交换声明序会反转
// 构造指令序（ORIG 先构造 empty），两形互斥；探针三形不可兼得 ⇒ 封顶。
bool AuthGarenaToken::init(const char* path)
{
    // ORIG 81730d6：arg==empty（0x8b83f20）走带参 Init(path)，
    // 否则无参 Init()（= Init("publickey.pem"@0x8b81361)）。
    std::string empty("");
    // [b28d-solo-D02] ORIG 0x08173115 直接压 path 形参（无判空三目，P171）。
    std::string arg(path);
    if (arg == empty)
        return openSSLRSA_.Init();
    return openSSLRSA_.Init(path);
}