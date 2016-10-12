#include "I2C_Shared.h"

namespace i2c
{
    TI2CStats::TI2CStats()
        : results{0}
    {
    }

    EI2CResult TI2CStats::AddResult( EI2CResult resultIn )
    {
        results[ resultIn ] += 1;
        return resultIn;
    }

    uint32_t TI2CStats::GetResultCount( EI2CResult resultIn )
    {
        return results[ resultIn ];
    }
}