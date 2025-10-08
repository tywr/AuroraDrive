#include "cmos.h"

extern "C"
{
    CMOS* CMOS_create(float fs, float c, float r, float i_s, float v_t)
    {
        return new CMOS();
    }

    float CMOS_process_sample(CMOS* cmos_instance, float inputVoltage)
    {
        return cmos_instance->processSample(inputVoltage);
    }

    void CMOS_destroy(CMOS* cmos_instance)
    {
        delete cmos_instance;
    }
}
