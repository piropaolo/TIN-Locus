#ifndef LOCUS_CONVERTER_H
#define LOCUS_CONVERTER_H

#include <vector>
#include <memory>
#include <type_traits>
#include <iomanip>
#include <cstddef>

namespace packet {

//    using byte = unsigned char;
    using byte = unsigned char;

    template<typename T>
    std::vector<byte> to_bytes(const T &object) {
        std::vector<byte> bytes(sizeof(T));

        const byte *begin = reinterpret_cast< const byte * >( std::addressof(object));
        const byte *end = begin + sizeof(T);
        std::copy(begin, end, std::begin(bytes));

        return bytes;
    }

    template<typename T>
    T &from_bytes(const std::vector<byte> &bytes, T &object) {
        static_assert(std::is_trivially_copyable<T>::value, "not a TriviallyCopyable type");

        byte *begin_object = reinterpret_cast< byte * >( std::addressof(object));
        std::copy(std::begin(bytes), std::end(bytes), begin_object);

        return object;
    }
    /*
     * using:
     * int n = 8;
     * const auto bytes = to_bytes(n);
     *
     * int m;
     * from_bytes( bytes, m );
     * 
     * website address:
     * http://www.cplusplus.com/forum/beginner/155821/
     */
}

#endif //LOCUS_CONVERTER_H
