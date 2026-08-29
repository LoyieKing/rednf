// ORIG 布局证据: objdump（报告缺失, nm 定址转写）
//   ctor 0x0891b12c / dtor 0x0891b174 / clear 0x0891b18a /
//   copy ctor 0x08929428 / operator= 0x089294d8
// sizeof=0xc: int@+0x00, std::string@+0x04, 3×byte@+0x08..+0x0a
#ifndef DNF_GAME_STDEADTOWERDIALOG_H_
#define DNF_GAME_STDEADTOWERDIALOG_H_

#include <string>
#include <vector>

class STDeadTowerDialog
{
public:
    STDeadTowerDialog();                                // W 0x0891b12c
    STDeadTowerDialog(STDeadTowerDialog const&);        // W 0x08929428
    ~STDeadTowerDialog();                               // W 0x0891b174
    STDeadTowerDialog& operator=(STDeadTowerDialog const&); // W 0x089294d8
    void clear();                                       // W 0x0891b18a
    int m_index;            // +0x00  [推断: importDeadTowerDialogScript 中 ScanInt 结果]
    std::string m_str;      // +0x04  [推断: ScanStr 结果]
    unsigned char m_flag1;  // +0x08  [推断: 脚本中置 1 的标记]
    unsigned char m_flag2;  // +0x09
    unsigned char m_flag3;  // +0x0a
};

static_assert(sizeof(STDeadTowerDialog) == 0x0c, "STDeadTowerDialog ABI drift");
// STDeadTowerFullDialog —— importDeadTowerDialogScript（ORIG T 0x08908d46）目标。
// 布局（汇编实证）：+0x00/+0x0c/+0x18 三个 vector<STDeadTowerDialog>，
// sizeof=0x24；分别承载 "[dialog on map start]/[dialog on map complete]/[dialog on die]"。
struct STDeadTowerFullDialog
{
    std::vector<STDeadTowerDialog> m_mapStartDialog;    // +0x00
    std::vector<STDeadTowerDialog> m_mapCompleteDialog; // +0x0c
    std::vector<STDeadTowerDialog> m_dieDialog;         // +0x18
};

static_assert(sizeof(STDeadTowerFullDialog) == 0x24, "STDeadTowerFullDialog ABI drift");

#endif  // DNF_GAME_STDEADTOWERDIALOG_H_
