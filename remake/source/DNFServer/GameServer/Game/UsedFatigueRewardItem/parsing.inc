// ORIG T 0x081a25d2：Load(path,"on_off") 失败返 0xa1；GetRowNum()==0 返 0xa4；
// GetString(0,"flag")=="true" → m_enable=1；Load(path,"item_reward") 失败返
// 0xac、GetRowNum()==0 返 0xaf；逐行 GetInt("fatigue"/"item_count"/"item_id")
// + GetString("message"/"title") 填 Data 后 push_back；成功返 0。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF → n1_v 新口径 AE（gap=+0，blocks=0）⇒ 旧判词作废。
int UsedFatigueRewardItem::parsing(char* path)
{
    Arad_Script script;
    if (!script.Load(path, "on_off"))
        return 0xa1;
    if (script.GetRowNum() == 0)
        return 0xa4;

    m_enable = false;
    std::string flag(script.GetString(0, "flag"),
                     std::allocator<char>());   // ORIG SaIcC1+SsC1(ptr,alloc)
    if (flag == std::string("true"))            // ORIG string==string（构造临时）
        m_enable = true;

    if (!script.Load(path, "item_reward"))
        return 0xac;
    if (script.GetRowNum() == 0)
        return 0xaf;

    // [P3·UsedFatigueRewardItem] ORIG 0x081a2774：Data::Data() 只在循环外构造一次
    // （在 movl $0x0,-0xc(%ebp) 之前），循环体 [0x081a2785,0x081a2883] 内无构造调用，
    // 且全 ELF 无任何 Data::~Data 外部调用点；故对象声明在循环外、每轮复用。
    Data d;
    for (int i = 0; i < script.GetRowNum(); ++i)
    {
        d.m_begin = (short)script.GetInt(i, "fatigue");
        d.m_end = (short)script.GetInt(i, "item_count");
        d.m_id = script.GetInt(i, "item_id");
        strncpy(d.m_str, script.GetString(i, "message"), 0xff);
        strncpy(d.m_name, script.GetString(i, "title"), 0x10);
        m_data.push_back(d);
    }
    return 0;
}