#ifndef GAME_BLUEMARBLE_MAPSCRIPT_H_
#define GAME_BLUEMARBLE_MAPSCRIPT_H_

// ============================================================================
// df_game_r BlueMarbleMapScript（G2-4 大富翁地图脚本，尺寸 0xc）
// 布局依据 ORIG 构造（0x088d53d6）：仅一个 vector<BlueMarbleTileScript*>。
// ============================================================================

#include <vector>

class BlueMarbleTileScript;

class BlueMarbleMapScript
{
public:
    BlueMarbleMapScript();
    ~BlueMarbleMapScript();

    void clear();
    BlueMarbleTileScript* getTile(unsigned int zone);
    int importScript(const char* path);

private:
    std::vector<BlueMarbleTileScript*> m_tiles;  // +0x00（0xc）
};

#endif  // GAME_BLUEMARBLE_MAPSCRIPT_H_
