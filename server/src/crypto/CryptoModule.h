#ifndef LOCUS_CRYPTOMODULE_H
#define LOCUS_CRYPTOMODULE_H

#include "Crypto.h"

namespace crypto {
    typedef std::vector<unsigned char> byte_vector;

    class CryptoModule {
    public:
        enum class Algorithm : unsigned int {
            None,
            RSA,
            AES
        };

        CryptoModule() = default;

        void setOuterRSAKey(const byte_vector &key);
        byte_vector encryptRSA(const byte_vector &text);
        byte_vector decryptRSA(const byte_vector &cipher);

        byte_vector getSymmetricKey() const;
        byte_vector encryptAES(const byte_vector &text);
        byte_vector decryptAES(const byte_vector &cipher);

        void use(const Algorithm &algo);
        byte_vector encrypt(const byte_vector &text);
        byte_vector decrypt(const byte_vector &cipher);

    private:
        RSAServerCrypto rsaServerCrypto;
        RSACrypto rsaCrypto;
        AESCrypto aesCrypto;
        bool outerRSAKey = false;
        Algorithm algorithm = Algorithm::None;
    };
}


#endif //LOCUS_CRYPTOMODULE_H
