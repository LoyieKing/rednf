// df_game_r GameResultSet 还原（G2-4 批次，CParty +0x17f4 内嵌，尺寸 0x50）。
// 对照 docs/class_func_reports/GameResultSet.md：Ctor/Dtor/Clear 均为
// 对基类 GameResultType 的同名方法的转发（GameResultType 属后续批次，
// 本 TU 只声明不定义，调用符号按 ORIG 权威符号 _ZN14GameResultType*）。

class GameResultType
{
public:
    GameResultType();           // ORIG 非内联（0x84b73d4，另一批次）
    ~GameResultType() {}        // ORIG 内联空析构（0x84b7416，头部内联）
    void Clear();               // ORIG 非内联（0x84b7442，另一批次）

    char m_data[0x50];  // +0x00
};

class GameResultSet : public GameResultType
{
public:
    GameResultSet();
    ~GameResultSet();
    void Clear();
};

GameResultSet::GameResultSet()
{
}

GameResultSet::~GameResultSet()
{
}

void GameResultSet::Clear()
{
    GameResultType::Clear();
}
