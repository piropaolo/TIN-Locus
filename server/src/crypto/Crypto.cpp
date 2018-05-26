#include "Crypto.h"

#include <crypto++/files.h>

#include <stdexcept>
#include <sstream>

#include <cstring>

using namespace CryptoPP;

namespace crypto {
    RSACrypto::RSACrypto(bool loadKeysFromFiles, bool saveKeysToFiles,
        const std::string &publicKeyFilename, 
        const std::string &privateKeyFilename) {
        if (!loadKeysFromFiles || !loadKeysFromFileSystem(publicKeyFilename, privateKeyFilename)) {
            // Create Pair of Keys
            privateKey_.GenerateRandomWithKeySize(randomGenerator_, 512);
            publicKey_.AssignFrom(privateKey_);

            if (saveKeysToFiles)
                saveKeysToFileSystem(publicKeyFilename, privateKeyFilename);
        }

        initCryptors();
    }

    RSACrypto::RSACrypto(const std::string &encryptKey, const std::string &decryptKey) {
        {
            StringSource encryptKeySource(encryptKey, true);
            ByteQueue queue;
            encryptKeySource.TransferTo(queue);
            queue.MessageEnd();
            publicKey_.Load(queue);

            if (!publicKey_.Validate(randomGenerator_, 2))
                throw std::runtime_error("RSA public key validation failed");
        }
        {
            StringSource decryptKeySource(decryptKey, true);
            ByteQueue queue;
            decryptKeySource.TransferTo(queue);
            queue.MessageEnd();
            privateKey_.Load(queue);

            if (!privateKey_.Validate(randomGenerator_, 2))
                throw std::runtime_error("RSA private key validation failed");
        }

        initCryptors();
    }

    std::string RSACrypto::encrypt(const std::string &text) {
        //NOTE: for RSA 512 (OAEP SHA) max text length is 22 (bytes)
        if (text.size() > maxPlainTextLength_) {
            std::ostringstream errorMsg;
            errorMsg << "Given plain text to encrypt with RSA exceeds max length, which is: " << maxPlainTextLength_;

            throw std::length_error(errorMsg.str());
        }

        std::string cipher; // encrypted text

        StringSource ess(
            text, true,
            new PK_EncryptorFilter(randomGenerator_, encryptor_, 
                new StringSink(cipher)
            )
        );

        return cipher;
    }

    std::string RSACrypto::decrypt(const std::string &cipher) {
        if (cipher.size() != fixedCipherTextLength_) {
            std::ostringstream errorMsg;
            errorMsg << "Given cipher text to decrypt with RSA doesn't max the required length, which is: " << fixedCipherTextLength_;

            throw std::length_error(errorMsg.str());
        }

        std::string recovered; // decrypted text

        StringSource dss(
            cipher, true,
            new PK_DecryptorFilter(randomGenerator_, decryptor_, 
                new StringSink(recovered)
            )
        );

        return recovered;
    }

    void RSACrypto::setEncryptKey(const std::string &encryptKey) {
        StringSource encryptKeySource(encryptKey, true);
        ByteQueue queue;
        encryptKeySource.TransferTo(queue);
        queue.MessageEnd();
        publicKey_.Load(queue);

        if (!publicKey_.Validate(randomGenerator_, 2))
            throw std::runtime_error("RSA public key validation failed");

        initCryptors();
    }
    void RSACrypto::setDecryptKey(const std::string &decryptKey) {
        StringSource decryptKeySource(decryptKey, true);
        ByteQueue queue;
        decryptKeySource.TransferTo(queue);
        queue.MessageEnd();
        privateKey_.Load(queue);

        if (!privateKey_.Validate(randomGenerator_, 2))
            throw std::runtime_error("RSA private key validation failed");

        initCryptors();
    }

    std::string RSACrypto::getPublicKey() const {
        std::string keyString;

        ByteQueue queue;
        publicKey_.Save(queue);
        StringSink keySS(keyString);
        queue.CopyTo(keySS);
        keySS.MessageEnd();

        return keyString;
    }

    void RSACrypto::saveKeysToFileSystem(const std::string &encryptKeyFilename, const std::string &decryptKeyFilename) {
        {
            ByteQueue queue;
            publicKey_.Save(queue);
            FileSink file(encryptKeyFilename.c_str());
            queue.CopyTo(file);
            file.MessageEnd();
        }
        {
            ByteQueue queue;
            privateKey_.Save(queue);
            FileSink file(decryptKeyFilename.c_str());
            queue.CopyTo(file);
            file.MessageEnd();
        }
    }

    bool RSACrypto::loadKeysFromFileSystem(const std::string &encryptKeyFilename, const std::string &decryptKeyFilename) {
        try {
            {
                FileSource file(encryptKeyFilename.c_str(), true);
                ByteQueue queue;
                file.TransferTo(queue);
                queue.MessageEnd();
                publicKey_.Load(queue);

                if (!publicKey_.Validate(randomGenerator_, 2))
                    throw std::runtime_error("RSA public key validation failed");
            }
            {
                FileSource file(decryptKeyFilename.c_str(), true);
                ByteQueue queue;
                file.TransferTo(queue);
                queue.MessageEnd();
                privateKey_.Load(queue);

                if (!publicKey_.Validate(randomGenerator_, 2))
                    throw std::runtime_error("RSA private key validation failed");
            }
            return true;
            // load successful
        }
        catch (const FileSource::Err &err) {
            return false;
            // load unsuccessful
        }
    }

    void RSACrypto::initCryptors() {
        RSAES_OAEP_SHA_Encryptor encryptor(publicKey_);
        encryptor_ = encryptor;
        maxPlainTextLength_ = encryptor_.FixedMaxPlaintextLength();

        RSAES_OAEP_SHA_Decryptor decryptor(privateKey_);
        decryptor_ = decryptor;
        fixedCipherTextLength_ = decryptor_.FixedCiphertextLength();
    }

    const char* RSACrypto::DEFAULT_PUBLIC_KEY_FILENAME = "rsa-public.key";
    const char* RSACrypto::DEFAULT_PRIVATE_KEY_FILENAME = "rsa-private.key";

    /*struct SymmetricKeyAndIV {
        std::string key;
        std::string iv;
        // byte arrays?

        SymmetricKeyAndIV(const char *keyPtr, std::size_t keySize, const char *ivPtr, std::size_t ivSize) : key(keyPtr, keySize), iv(ivPtr, ivSize) {
        }
    };*/

    AESCrypto::AESCrypto() {
        // Generate a random key
        // Default key length is 16B (128b)
        key_.Assign((byte*)NULL, AES::DEFAULT_KEYLENGTH);
        randomGenerator_.GenerateBlock( key_, key_.size() );

        // Generate a random IV (Initialization vector)
        // Block size for AES is always 16B
        //iv_.Assign(AES::BLOCKSIZE, (byte)0);
        //randomGenerator_.GenerateBlock(iv_, iv_.size());   

        initCryptors();
    }

    AESCrypto::AESCrypto(const std::string &symmetricKey) {
        if (symmetricKey.size() != AES::DEFAULT_KEYLENGTH) {
            std::ostringstream errorMsg;
            errorMsg << "Given symmetric key for AES doesn't max the required length, which is: " << AES::DEFAULT_KEYLENGTH;

            throw std::length_error(errorMsg.str());
        }

        key_.Assign((const byte*) symmetricKey.data(), symmetricKey.size());

        initCryptors();
    }

    std::string AESCrypto::encrypt(const std::string &text) {
        std::string cipher;

        //NOTE: The StreamTransformationFilter adds padding
        //  as required. ECB and CBC Mode must be padded
        //  to the block size of the cipher.
        StringSource ess( 
            text, true,
            new StreamTransformationFilter(encryptor_,
                new StringSink(cipher)
            )
        );

        return cipher;
    }

    std::string AESCrypto::decrypt(const std::string &cipher) {
        std::string recovered;

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource dss( 
            cipher, true,
            new StreamTransformationFilter(decryptor_,
                new StringSink(recovered)
            )
        );

        return recovered;
    }

    void AESCrypto::setSymmetricKey(const std::string &symmetricKey) {
        if (symmetricKey.size() != AES::DEFAULT_KEYLENGTH) {
            std::ostringstream errorMsg;
            errorMsg << "Given symmetric key for AES doesn't max the required length, which is: " << AES::DEFAULT_KEYLENGTH;

            throw std::length_error(errorMsg.str());
        }

        key_.Assign((const byte*) symmetricKey.data(), symmetricKey.size());

        initCryptors();
    }

    /*SymmetricKeyAndIV getSymmetricKey() const {
        return SymmetricKeyAndIV(
            key_.data(), key_.SizeInBytes(),
            iv_.data(), iv_.SizeInBytes());
    }*/

    std::string AESCrypto::getSymmetricKey() const {
        return std::string((char*)key_.data(), key_.SizeInBytes());
    }

    void AESCrypto::initCryptors() {
        encryptor_.SetKey(key_, key_.size());
        decryptor_.SetKey(key_, key_.size());
    }
}