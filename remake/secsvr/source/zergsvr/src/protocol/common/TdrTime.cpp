// Reconstructed from gunnersvr disassembly
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32/src/protocol/common/TdrTime.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)

#include "src/protocol/common/TdrTime.h"
#include "src/protocol/common/TdrPal.h"
#include "src/protocol/common/TdrBuf.h"
#include "src/protocol/common/TdrError.h"

bool tsf4g_tdr::TdrDate::isValid() const {
    if (year > 9999) {
        return false;
    }
    if (month > 12) {
        return false;
    }
    if (day > 31) {
        return false;
    }
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return day != 31;
    }
    if (month == 2) {
        bool leap = (year % 4 == 0);
        if (year % 100 == 0) {
            leap = (year % 400 == 0);
        }
        if (day > (leap ? 29 : 28)) {
            return false;
        }
    }
    return true;
}
