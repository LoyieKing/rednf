bool CSyncScript::LoadItemInfoMasterScript(const char* path)
{
    // [b28d-solo-sync_script] 判词：NEAR 封顶（N7/N18a 单条寄存器差 + 尾 nop）。
    // ORIG @0x08625064：446=446，P167 调用点差集 CLEAR（80=80），全帧槽集全等
    // （帧 0x90=0x90；type -0x1c / subType -0x18 / value -0x14 / extra -0x10 /
    // job -0xc + line/str/name 三 string 槽全等）。唯一残差 =
    // SWEAPONTYPE 键构造处 ORIG `mov -0x18(%ebp),%eax` 而 4.4 取 %edx（1 条
    // 寄存器名分配差）与 ORIG 尾对齐 nop（§6.1 豁免）。语义等价。
    // [S3/M3FreeNext15B 2026-09-11 复扫] ①§6.5 串审计 CLEAR（两侧 literal 多重集
    //   全等：8 kinds/8 refs，`[master type]`/`[type addition]`/`[reverse desc]`/
    //   `[material type]`/`[weapon type]`/`[armor type]`/`[item type]`/空串）；
    //   ②P248② 类型旋钮实测**反劣**：`unsigned int subType` ⇒ 086252dc 段 4.4
    //   多发一条 `mov -0x18(%ebp),%edx`（实参装载序被打乱）⇒ 已回滚；
    //   ③P285 无落点：ORIG 体尾 0x08625695 `nop` 是 4.1 对齐 nop（§6.1 豁免），
    //   非 `jmp+尾岛` 结构（我方尾 18c3 mov/../18ce ret 与 ORIG 逐条对齐，446=446）。
    //   ⇒ 维持 N7/N18a：唯一残差 = SWEAPONTYPE 键构造处 ORIG 0x086252dc
    //   `mov -0x18(%ebp),%eax` vs 4.4 `%edx`（1 条寄存器名分配差）。
    // [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR（N7/N18a/N98）→ 试=键构造形 A/B：
    //   具名 `SWEAPONTYPE key(job, subType); m_weaponMap[key] = …`（overlay）⇒ **反劣**
    //   （槽组整体浅移 8：-0x50→-0x48、-0x60/-0x64 新增，且多 1 枚 _ZNSsC1Ev ⇒ 已回滚）；
    //   改写后 ORIG 侧形 0x086252dc 段仍为 `mov -0x18(%ebp),%eax; mov %eax,0x8(%esp);
    //   mov -0xc(%ebp),%eax; mov %eax,0x4(%esp)`（4.1 纯右→左）vs 4.4 恒先 args[1] 到 %eax
    //   再 args[0] 到 %edx ⇒ 前端求值序不可造 ⇒ 维持 NEAR（446/447，gap -1；尾 nop 豁免）。
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    std::string line;
    std::string str;
    // ORIG 实证（0x08625064，size=1585）：本函数只有 3 个 _ZNSsC1Ev 调用点
    // （line/str/name）与 14 个 _ZNSsD1Ev。name 为**函数作用域单例**并在三个分支
    // 间复用（ScanStr 成功时整体覆盖），而非每分支各建一个具名局部；
    // 分支内声明会额外产生 2 个 _ZNSsC1Ev + 4 个 _ZNSsD1Ev（含 unwind 清理）。
    std::string name;
    // [b28d-solo-sync_script] P212：ORIG 槽使用计数 = -0x1c:14 / -0x18:8 /
    //   -0x14:6 / -0xc:2 ⇒ value/type/subType/extra/job 为**函数级共享**标量
    //   （分支内各声明一份会让 4.4 逐分支独立占槽，全帧槽号整体深移 0x28）。
    //   声明序 = ORIG 槽深→浅（type -0x1c / subType -0x18 / value -0x14 /
    //   extra -0x10 / job -0xc）。
    int type;
    int subType;
    int value;
    int extra;
    int job;
    // ORIG 0x08625093：循环条件在**循环体首部**以 `xor $1 / test / jne` 求反后
    // break（多出 1 个 xor + 2 个 nop），即 while(true){ if(!ScanType) break; }，
    // 而非 while(ScanType(...)) 的底部条件形态。
    while (true)
    {
        if (!ScanType(line, true))
        {
            break;
        }
        if (line == "[master type]")
        {
            // ORIG 0x086250b6 段：master 族分支与 weapon/armor/item 的 type 共用
            // 同一槽 -0x1c（4.1 互斥作用域复用）⇒ 源侧复用函数级 type 变量，
            // 4.4 按首次使用序即得同槽（P212/N98 联判）。
            type = ScanInt((bool*)0);
            ScanStr(&str);
            m_map64[(unsigned char)type] = str;
        }
        else if (line == "[type addition]")
        {
            type = ScanInt((bool*)0);
            m_vec1.push_back((unsigned char)type);
        }
        else if (line == "[reverse desc]")
        {
            type = ScanInt((bool*)0);
            m_vec2.push_back((unsigned char)type);
        }
        else if (line == "[material type]")
        {
            type = ScanInt((bool*)0);
            ScanStr(&str);
            m_map7c[(unsigned char)type] = str;
        }
        else if (line == "[weapon type]")
        {
            ScanStr(&str);
            type = ScanInt((bool*)0);
            subType = ScanInt((bool*)0);
            value = ScanInt((bool*)0);
            ScanStr(&name);
            job = getCharacterJob(str.c_str());
            // ORIG 0x086252d7..0x08625318：先构造右值 SITEMTYPE(local_64)，
            // 再构造下标键 SWEAPONTYPE(local_54)——键不是具名局部，
            // 而是 operator[] 实参处的临时（gcc 右→左求值）。
            m_weaponMap[SWEAPONTYPE((ENUM_CHARACTERJOB)job, subType)] =
                SITEMTYPE(false, (unsigned char)type,
                          (unsigned char)subType,
                          (unsigned short)value, name);
        }
        else if (line == "[armor type]")
        {
            ScanStr(&str);
            type = ScanInt((bool*)0);
            subType = ScanInt((bool*)0);
            value = ScanInt((bool*)0);
            ScanStr(&name);
            // ORIG 0x08625408..0x0862545e：同上，SITEMTYPE(local_4c) 先建，
            // SARMORTYPE(local_3c) 作为 operator[] 处的临时键后建。
            m_armorMap[SARMORTYPE((unsigned char)subType, str)] =
                SITEMTYPE(false, (unsigned char)type,
                          (unsigned char)subType,
                          (unsigned short)value, name);
        }
        else if (line == "[item type]")
        {
            ScanStr(&str);
            type = ScanInt((bool*)0);
            subType = ScanInt((bool*)0);
            value = ScanInt((bool*)0);
            extra = ScanInt((bool*)0);
            ScanStr(&name);
            m_itemTypeMap[str] = SITEMTYPE(type != 0, (unsigned char)subType,
                                           (unsigned char)value,
                                           (unsigned short)extra, name);

        }
    }
    return true;
}