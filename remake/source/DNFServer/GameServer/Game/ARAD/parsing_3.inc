int Arad_Mileage_Data::parsing(char* data)
{
    Arad_Script script;
    if (!script.Load(data, "default"))
        return 0x18c;
    if (script.GetRowNum() == 0)
        return 0x18f;

    m_defaultPercent = 0;
    m_defaultPercent = script.GetInt(0, "percent");
    if (m_defaultPercent < 0 || m_defaultPercent > 0x3e8)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Script_Loader.cpp",
            "virtual int Arad_Mileage_Data::parsing(char*)",
            0x197, "[ERROR] invalid mileage default value. (%f)", m_defaultPercent);
        return 0x198;
    }

    if (!script.Load(data, "item"))
        return 0x19e;
    if (script.GetRowNum() == 0)
        return 0;

    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        int ipg = script.GetInt(r, "ipg_no");
        int percent = script.GetInt(r, "percent");
        // b28d-solo-ARAD：P0——ORIG 0x81a3598 `call find; mov %eax,-0xc(%ebp)` 有具名
        // 结果槽（-0xc），判断为 `cmpl $0x0,-0xc; jns <错误块>`；内联 find() 被 4.4
        // 折成 `shr $0x1f,%eax` 且整帧浅 4（script -0x70 vs ORIG -0x74）。
        int found = find(ipg);
        if (found < 0)
        {
            // b28d-solo-ARAD：ORIG 0x081a35ca 调 push_back(Data&&)（`OS1_` 右值重载）⇒
            // 原码是匿名临时形（Data 临时占 -0x20 sret 槽，P0 限定①类临时不可由槽区分
            // 具名性）；具名 `Data d; push_back(d)` 选 RKS1_（p167 差集 1/1）。
            m_mileageList.push_back(Data(ipg, percent));
        }
        else
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_Mileage_Data::parsing(char*)",
                0x1b0, "[ERROR] already exist item index (IPG:%d, percent:%d)",
                ipg, percent);
            return 0x1b1;
        }
    }
    return 0;
