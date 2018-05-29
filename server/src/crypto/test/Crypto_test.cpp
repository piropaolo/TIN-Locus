#include "gtest/gtest.h"
#include "crypto/keysGenerator.h"
#include "crypto/Crypto.h"
#include <iostream>

#define PRINT2

inline crypto::byte_vector toByteVector(const std::string &string) {
    return crypto::byte_vector(string.begin(), string.end());
}
inline std::string toString(const crypto::byte_vector &vector) {
    return std::string(vector.begin(), vector.end());
}

TEST(Crypto_RSA, Test) {
    crypto::keysGenerator(1024);
    crypto::RSAServerCrypto rsaServerCrypto;
    std::string res = "Ala ma kota";
    crypto::byte_vector vec = toByteVector(res);

    auto encryptVec = rsaServerCrypto.encrypt(vec);
    auto decryptVec = rsaServerCrypto.decrypt(encryptVec);

#ifdef PRINT
    auto encryptRes = toString(encryptVec);
    auto decryptRes = toString(decryptVec);

    //NOTE: for RSA 512 (OAEP SHA) max text length is 22 (bytes)
    std::cout << "RSA Before encryption: " << res << '\n';
    std::cout << "RSA Text length: " << res.size() << '\n';

    std::cout << "RSA Encrypted: " << encryptRes << '\n';
    std::cout << "RSA Encrypted length: " << encryptRes.size() << '\n';

    std::cout << "RSA Decrypted: " << decryptRes << '\n';

    EXPECT_NE(res, encryptRes);
    EXPECT_EQ(res, decryptRes);
#endif

    EXPECT_NE(vec, encryptVec);
    EXPECT_EQ(vec, decryptVec);
}

TEST(Crypto_AES, Test) {
//    EXPECT_EQ(buffer.size(), 0);
    crypto::AESCrypto aesCrypto;
    std::string res = "Ala ma kota";
    crypto::byte_vector vec = toByteVector(res);

    auto encryptVec = aesCrypto.encrypt(vec);
    auto decryptVec = aesCrypto.decrypt(encryptVec);

#ifdef PRINT
    auto encryptRes = toString(encryptVec);
    auto decryptRes = toString(decryptVec);

    std::cout << "AES Before encryption: " << res << '\n';
    std::cout << "AES Text length: " << res.size() << '\n';

    //NOTE: Cipher after AES encryption (ECB mode) is padded to the block size (16B)
    std::cout << "AES Encrypted: " << encryptRes << '\n';
    std::cout << "AES Encrypted length: " << encryptRes.size() << '\n';

    std::cout << "AES Decrypted: " << decryptRes << '\n';

    EXPECT_NE(res, encryptRes);
    EXPECT_EQ(res, decryptRes);
#endif

    EXPECT_NE(vec, encryptVec);
    EXPECT_EQ(vec, decryptVec);
}

TEST(Crypto_AES, TestWithTwoBlocksLength) {
//    EXPECT_EQ(buffer.size(), 0);
    crypto::AESCrypto aesCrypto;
    // cipher with 2 blocks length:
    std::string res = "Ala ma kota i duuuuzego psa";

    crypto::byte_vector vec = toByteVector(res);

    auto encryptVec = aesCrypto.encrypt(vec);
    auto decryptVec = aesCrypto.decrypt(encryptVec);

#ifdef PRINT
    auto encryptRes = toString(encryptVec);
    auto decryptRes = toString(decryptVec);

    std::cout << "AES long Before encryption: " << res << '\n';
    std::cout << "AES long Text length: " << res.size() << '\n';

    //NOTE: Cipher after AES encryption (ECB mode) is padded to the block size (16B)
    std::cout << "AES long Encrypted: " << encryptRes << '\n';
    std::cout << "AES long Encrypted length: " << encryptRes.size() << '\n';

    std::cout << "AES long Decrypted: " << decryptRes << '\n';

    EXPECT_NE(res, encryptRes);
    EXPECT_EQ(res, decryptRes);
#endif

    EXPECT_NE(vec, encryptVec);
    EXPECT_EQ(vec, decryptVec);
}