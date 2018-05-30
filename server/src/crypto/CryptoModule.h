#ifndef LOCUS_CRYPTOMODULE_H
#define LOCUS_CRYPTOMODULE_H

#include "Crypto.h"

namespace crypto {
    typedef std::vector<unsigned char> byte_vector;

    class CryptoModule {
    public:
        CryptoModule() = default;

        void setOuterRSAKey(const byte_vector &key);
        byte_vector encryptRSA(const byte_vector &text);
        byte_vector decryptRSA(const byte_vector &cipher);

        byte_vector getSymmetricKey() const;
        byte_vector encryptAES(const byte_vector &text);
        byte_vector decryptAES(const byte_vector &cipher);

    private:
        RSAServerCrypto rsaServerCrypto;
        RSACrypto rsaCrypto;
        AESCrypto aesCrypto;
        bool outerRSAKey = false;
    };
}


#endif //LOCUS_CRYPTOMODULE_H
