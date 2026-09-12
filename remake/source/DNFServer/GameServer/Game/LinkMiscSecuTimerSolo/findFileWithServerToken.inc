// ===========================================================================
// findFileWithServerToken  ORIG T 0x88bb7db
// pathStr(path) 副本 + 默认构造 full；find_last_of("/",npos)，无则
// find_last_of("\\",npos)；命中则 full = substr(0,pos+1); full += "(R)";
// full += substr(pos+1,size())；ss_strcpy 回写 path，pack 模式查
// g_script_package.is_exist，否则 access(path,0)==0；命中即 true（path 保留
// full），未命中 ss_strcpy 还原 pathStr 后 false；pos==-1 直接 false。
// [b28e 定稿 · 181=181 gap 0] **P248 档② 源字段同型化（两全形）**：ORIG
// 0x088bb982..0x088bb98e 为 `movzbl 0x93f4d24,%eax; xor $1,%eax; test %al,%al; je`
// （s_bUseScriptPack 与 findFileWithServerToken 在 ORIG 同属 RDARScript.cpp）。
// 重建把 RDAR 域拆两个 TU（定义在 ScriptExtras.cpp），`_ZL16s_bUseScriptPack` 为
// 内部链接 ⇒ 只能经真实访问器 `call _Z14UseScriptPackv`（P189 禁 extern 桥）⇒
// 调用点恒差 1 条 call vs TU-local movzbl（**不可消除**）。
// 三形净账实测（净 identical 红线）：
//   ①char 字段 + char 返回（原状）：182 条 gap +1（`call; test; sete; test; je`）；
//   ②char 字段 + bool 返回：消费点 181/gap 0，但定义侧本体被插 `test; setne`
//     落 DIFF（ScriptExtras 合计 **293→292，identical 账 −1 ⇒ 净负，已回退**）；
//   ③**bool 字段 + bool 返回（现形，P248 档②）**：返回表达式本身已是 bool ⇒
//      本体仍是 `movzbl mem; ret` 直返（identical 保持），消费点
//      `call; xor $1; test %al,%al; je` = 181 条 = ORIG（gap 0）
//      ⇒ ScriptExtras **103/190/0/9 → 103/191/0/8**（ae +1、diff −1、identical 不减）。
// 其余语义（含 pack/PackSet 双分支、full 默认构造+substr 临时 operator= 链）与
// ORIG 切片逐条对位。
// [P1T3LinkMiscSecuTimerSolo 2026-09-11 复核] n1_v 181/181 gap 0（唯一残差 =
// `call _Z14UseScriptPackv` vs ORIG `movzbl _ZL16s_bUseScriptPack,%eax`；p167 记
// EXTRA 1 `_Z13UseScriptPackv`）。三形净账与前人结论一致 ⇒ 维持 N（P189 封顶）：
// 本 TU 另立 static 副本会与 ScriptExtras.cpp 的写点（InitRDARScript）分叉
// （语义轴禁止），extern 桥因内部链接符号不可见 + P189 禁用，objcopy 越权 ⇒ 无源改路径。
// ===========================================================================
bool findFileWithServerToken(char* path)
{
    std::string pathStr(path);
    std::string full;
    int pos = pathStr.find_last_of("/", std::string::npos);
    if (pos == -1)
        pos = pathStr.find_last_of("\\", std::string::npos);
    if (pos != -1)
    {
        full = pathStr.substr(0, pos + 1);
        full += "(R)";
        full += pathStr.substr(pos + 1, pathStr.size());
        ss_strcpy(path, 0x104, full.c_str());
        // ORIG 982 `movzbl s_b;xor$1;test;je pack`：pack==0 fall=access，
        // pack==1 je=PackSet；P22 `if(!f())`⇒xor+test 同 line 数。
        // [SemA02 2026-09-12] 本 .inc 宿主改为 ScriptExtras.cpp（RDAR 域，与
        // s_bUseScriptPack 同 TU）⇒ 直接读同 TU static，复现 ORIG `movzbl <A>,%eax`。
        if (!s_bUseScriptPack)
        {
            if (access(path, 0) == 0)
                return true;
        }
        else
        {
            if (g_script_package.is_exist(path))
                return true;
        }
        ss_strcpy(path, 0x104, pathStr.c_str());
    }
    return false;
}