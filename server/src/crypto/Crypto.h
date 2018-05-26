#ifndef CRYPTO_H
#define CRYPTO_H

#include <crypto++/rsa.h>
#include <crypto++/osrng.h>

#include <crypto++/aes.h>
#include <crypto++/modes.h>

using namespace CryptoPP;

namespace crypto {
    class RSACrypto {
        public:
            RSACrypto(bool loadKeysFromFiles = true, bool saveKeysToFiles = true,
                const std::string &publicKeyFilename = DEFAULT_PUBLIC_KEY_FILENAME, 
                const std::string &privateKeyFilename = DEFAULT_PRIVATE_KEY_FILENAME);
            RSACrypto(const std::string &encryptKey, const std::string &decryptKey);

            std::string encrypt(const std::string &text);
            std::string decrypt(const std::string &cipher);

            void setEncryptKey(const std::string &encryptKey);
            void setDecryptKey(const std::string &decryptKey);
            std::string getPublicKey() const;

        private:
            RSA::PrivateKey privateKey_;    // key used for decryption
            RSA::PublicKey publicKey_;      // key used for encryption

            RSAES_OAEP_SHA_Encryptor encryptor_;
            RSAES_OAEP_SHA_Decryptor decryptor_;

            std::size_t maxPlainTextLength_;
            std::size_t fixedCipherTextLength_;
            AutoSeededRandomPool randomGenerator_;

            //std::string encryptKeyFilename;
            //std::string decryptKeyFilename;
            static const char* DEFAULT_PUBLIC_KEY_FILENAME;
            static const char* DEFAULT_PRIVATE_KEY_FILENAME;

        private:
            void saveKeysToFileSystem(const std::string &encryptKeyFilename, const std::string &decryptKeyFilename);
            bool loadKeysFromFileSystem(const std::string &encryptKeyFilename, const std::string &decryptKeyFilename);

            void initCryptors();
    };

    class AESCrypto {
        public:
            AESCrypto();
            AESCrypto(const std::string &symmetricKey);

            std::string encrypt(const std::string &text);
            std::string decrypt(const std::string &cipher);

            void setSymmetricKey(const std::string &symmetricKey);
            std::string getSymmetricKey() const;

        private:
            SecByteBlock key_;

            ECB_Mode<AES>::Encryption encryptor_;
            ECB_Mode<AES>::Decryption decryptor_;

            AutoSeededRandomPool randomGenerator_;

            //SecByteBlock iv_;
            //CFB_Mode<AES>::Encryption encryptor_;
            //CFB_Mode<AES>::Decryption decryptor_;

        private:
            void initCryptors();
    };
}

#endif /*CRYPTO_H*/