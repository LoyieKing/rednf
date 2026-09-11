// ============================================================================
// importItmeGroupTable —— ORIG 0x0890bb25
// 报告 _lt_global_gt_/importItmeGroupTable.md；调用点 init.md +0x3e40：
//   importItmeGroupTable("Etc/", "ItemGroupName.tbl", getItemGroupNameTable())。
// 循环读 (id, 名称) 对插入 table；返回 0=成功、0x1ebd=ScanStr 失败、
// 0x1eb2=加载失败（错误码与 ORIG 一致）。
// ============================================================================
int importItmeGroupTable(const char* dir, const char* file,
                         std::map<std::string, int>& table)
{
    if (!loadRDARScriptFile(dir, file))
        return 0x1eb2;

    std::string name;
    bool ok = false;

    while (true)
    {
        int id = ScanInt(&ok);
        if (!ok)
            break;
        // b28c-l23：ORIG @0x0890bb5x 循环体内**无** `movb $0x0,-0x25(%ebp)` 预清
        // （ok 仅由 ScanInt 出参写、判后不复位）⇒ 该复位是原码没有的死写入，删。
        if (!ScanStr(&name))
            return 0x1ebd;
        table.insert(std::make_pair(name, id));
    }
    return 0;
}