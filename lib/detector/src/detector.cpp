#include <math.h>
#include <string.h>
#include "detector.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#ifndef PLATFORM_NATIVE
#define EXECUTE_FROM_RAM(subsection) __attribute__ ((long_call, section (".time_critical." subsection))) 
#else
#define EXECUTE_FROM_RAM(subsection)
#endif

using namespace detector;

EXECUTE_FROM_RAM("det")
void ObjectDetector::write(const uint16_t *data, size_t length) {
    auto dc_acc = m_dc_acc;
    auto dc_prev_x = m_dc_prev_x;
    auto dc_prev_y = m_dc_prev_y;

    while (length--) {
        int32_t sample = *data++;

        // DC removal stage
        // https://dspguru.com/dsp/tricks/fixed-point-dc-blocking-filter-with-noise-shaping/
        // https://www.iro.umontreal.ca/~mignotte/IFT3205/Documents/TipsAndTricks/DCBlockerAlgorithms.pdf
        {
            dc_acc -= dc_prev_x;
            dc_prev_x = sample << DC_BASE_SHIFT;
            dc_acc += dc_prev_x;
            dc_acc -= DC_POLE_NUM*dc_prev_y; 
            dc_prev_y = dc_acc / (1 << DC_BASE_SHIFT); // hopefully translates to ASR
        }
        auto int_y = dc_prev_y;

        // object detector
        bool v = int_y > m_threshold;
        if (v) {
            if (!m_in_obj) {
                m_in_obj = true;
                m_obj_start = m_timestamp;
                m_obj_ampl = int_y;
                m_obj_power = int_y;
            } else {
                m_obj_ampl = std::max(m_obj_ampl, int_y);
                m_obj_power += int_y;
            }
        } else {
            if (m_in_obj) {
                m_in_obj = false;
                // append object
                detected_object_t obj;
                obj.start = m_obj_start;
                obj.len = static_cast<uint32_t>(m_timestamp - obj.start);
                obj.power = m_obj_power;
                obj.ampl = m_obj_ampl;
                obj.source = 0;
                if (obj.len >= m_min_length) {
                    results.push_back(obj);
                }
            }
        }

        m_timestamp++;
    }

    m_dc_acc = dc_acc;
    m_dc_prev_x = dc_prev_x;
    m_dc_prev_y = dc_prev_y;
}

