#ifndef __ENDIAN_UTILS_HPP__
#define __ENDIAN_UTILS_HPP__

#include <cstdint>
#include <vector>

class EndianUtils {
public:
    template<typename T>
    static void unpack(const std::vector<uint8_t>::const_iterator& message, T& out_value) {
        static_assert(std::is_fundamental<T>::value, "must use base type");
        uint8_t* p_out = (uint8_t*)&out_value;
        if (sizeof(T) == 8) {
            p_out[0] = *(message + 7);
            p_out[1] = *(message + 6);
            p_out[2] = *(message + 5);
            p_out[3] = *(message + 4);
            p_out[4] = *(message + 3);
            p_out[5] = *(message + 2);
            p_out[6] = *(message + 1);
            p_out[7] = *(message);
        } else if (sizeof(T) == 4) {
            p_out[0] = *(message + 3);
            p_out[1] = *(message + 2);
            p_out[2] = *(message + 1);
            p_out[3] = *(message);
        } else if (sizeof(T) == 2) {
            p_out[0] = *(message + 1);
            p_out[1] = *(message);
        } else if (sizeof(T) == 1) {
            *p_out = *message;
        }
    }

    template<typename T, int size>
    static void unpackArr(const std::vector<uint8_t>::const_iterator& iter, std::array<T, size>& out_value) {
        int offset = 0;
        for (size_t i = 0; i < size; i++) {
            unpack<T>(iter + offset, out_value[i]);
            offset += sizeof(T);
        }
    }

};








#endif
