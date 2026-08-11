#ifndef DNF_SEGMENTATION_FAULT_SIG_H_
#define DNF_SEGMENTATION_FAULT_SIG_H_

#include "DNFSignal.h"

class CSegmentationFaultSig : public CSignal
{
public:
    CSegmentationFaultSig() throw();
    virtual ~CSegmentationFaultSig() throw();
    virtual void handle(int sig);
};

#endif // DNF_SEGMENTATION_FAULT_SIG_H_
