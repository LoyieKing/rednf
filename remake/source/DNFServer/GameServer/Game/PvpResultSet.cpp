// ============================================================================
// df_game_r PvpResultSet（G2-4 PvP 结果集合，PvpResultType 派生薄封装）
// 对照 docs/class_func_reports/PvpResultSet.md：ctor/dtor/Clear 仅转发基类；
// 基类实现见 PvpResultType.cpp（同批交付）。
// ============================================================================

class PvpResultType
{
public:
    PvpResultType();
    ~PvpResultType();
    void Clear();
};

class PvpResultSet : public PvpResultType
{
public:
    PvpResultSet();
    ~PvpResultSet();
    void Clear();
};

PvpResultSet::PvpResultSet()
{
}

PvpResultSet::~PvpResultSet()
{
}

void PvpResultSet::Clear()
{
    PvpResultType::Clear();
}
