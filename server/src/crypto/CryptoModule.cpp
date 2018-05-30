#include "CryptoModule.h"
#include <cmath>

namespace crypto {

    void CryptoModule::setOuterRSAKey(const byte_vector &key) {
        rsaCrypto.setEncryptionKey(key);
        rsaCrypto.setDecryptionKey(key);
        outerRSAKey = true;
    }

    byte_vector CryptoModule::encryptRSA(const byte_vector &text) {
        byte_vector encryptBytes;
        if (outerRSAKey) {
            for (int i = 0; i < text.size(); i += rsaCrypto.getMaxPlainTextLength_()) {
                auto newBytes = rsaCrypto.encrypt(
                        byte_vector(text.begin() + i,
                                    text.begin() + std::min(text.size(), i + rsaCrypto.getMaxPlainTextLength_())));
                encryptBytes.insert(encryptBytes.end(), newBytes.begin(), newBytes.end());
            }
        } else {
            encryptBytes = text;
        }

        byte_vector encryptBytesByServer;
        for (int i = 0; i < encryptBytes.size(); i += rsaServerCrypto.getMaxPlainTextLength_()) {
            auto newBytes = rsaServerCrypto.encrypt(
                    byte_vector(encryptBytes.begin() + i,
                                encryptBytes.begin() +
                                std::min(encryptBytes.size(), i + rsaServerCrypto.getMaxPlainTextLength_())));
            encryptBytesByServer.insert(encryptBytesByServer.end(), newBytes.begin(), newBytes.end());
        }

        return encryptBytesByServer;
    }

    byte_vector CryptoModule::decryptRSA(const byte_vector &cipher) {
        byte_vector decryptBytesByServer;
        for (int i = 0; i < cipher.size(); i += rsaServerCrypto.getFixedCipherTextLength_()) {
            auto newBytes = rsaServerCrypto.decrypt(
                    byte_vector(cipher.begin() + i,
                                cipher.begin() + i + rsaServerCrypto.getFixedCipherTextLength_()));
            decryptBytesByServer.insert(decryptBytesByServer.end(), newBytes.begin(), newBytes.end());
        }

        byte_vector decryptBytes;
        if (outerRSAKey) {
            for (int i = 0; i < decryptBytesByServer.size(); i += rsaCrypto.getFixedCipherTextLength_()) {
                auto newBytes = rsaCrypto.decrypt(
                        byte_vector(decryptBytesByServer.begin() + i,
                                    decryptBytesByServer.begin() + i + rsaCrypto.getFixedCipherTextLength_()));
                decryptBytes.insert(decryptBytes.end(), newBytes.begin(), newBytes.end());
            }
        } else {
            return decryptBytesByServer;
        }

        return decryptBytes;
    }

    byte_vector CryptoModule::getSymmetricKey() const {
        return aesCrypto.getSymmetricKey();
    }

    byte_vector CryptoModule::encryptAES(const byte_vector &text) {
        return aesCrypto.encrypt(text);
    }

    byte_vector CryptoModule::decryptAES(const byte_vector &cipher) {
        return aesCrypto.decrypt(cipher);
    }
}