#include "diode.h"

extern "C"
{
    Diode* Diode_create(float fs, float c, float r, float i_s, float v_t)
    {
        return new Diode(fs, c, r, i_s, v_t);
    }

    float Diode_process_sample(Diode* diode_instance, float inputVoltage)
    {
        return diode_instance->processSample(inputVoltage);
    }

    void Diode_destroy(Diode* diode_instance)
    {
        delete diode_instance;
    }
}
