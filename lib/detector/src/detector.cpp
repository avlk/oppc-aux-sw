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
void ObjectDetector::write(const int16_t *data, size_t length) {

    while (length--) {
        int32_t sample = *data++;

        // object detector
        bool v = sample > m_threshold;
        if (v) {
            if (!m_in_obj) {
                m_in_obj = true;
                m_obj_start = m_timestamp;
                m_obj_ampl = sample;
                m_obj_power = sample;
            } else {
                m_obj_ampl = std::max(m_obj_ampl, sample);
                m_obj_power += sample;
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
}

