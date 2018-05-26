#include <iostream>
#include "Crypto.h"

int main() {
    crypto::RSACrypto rsaCrypto;
    crypto::AESCrypto aesCrypto;
    std::string res = "Ala ma kota";

    //NOTE: for RSA 512 (OAEP SHA) max text length is 22 (bytes)
    std::cout << "RSA Before encryption: " << res << '\n';
    std::cout << "RSA Text length: " << res.size() << '\n';

    res = rsaCrypto.encrypt(res);

    std::cout << "RSA Encrypted: " << res << '\n';
    std::cout << "RSA Encrypted length: " << res.size() << '\n';

    res = rsaCrypto.decrypt(res);

    std::cout << "RSA Decrypted: " << res << '\n';

    res = "Ala ma kota";

    std::cout << "AES Before encryption: " << res << '\n';
    std::cout << "AES Text length: " << res.size() << '\n';

    res = aesCrypto.encrypt(res);

    //NOTE: Cipher after AES encryption (ECB mode) is padded to the block size (16B)
    std::cout << "AES Encrypted: " << res << '\n';
    std::cout << "AES Encrypted length: " << res.size() << '\n';

    res = aesCrypto.decrypt(res);

    std::cout << "AES Decrypted: " << res << '\n';

    // cipher with 2 blocks length:
    res = "Ala ma kota i duuuuzego psa";

    std::cout << "AES long Before encryption: " << res << '\n';
    std::cout << "AES long Text length: " << res.size() << '\n';

    res = aesCrypto.encrypt(res);

    //NOTE: Cipher after AES encryption (ECB mode) is padded to the block size (16B)
    std::cout << "AES long Encrypted: " << res << '\n';
    std::cout << "AES long Encrypted length: " << res.size() << '\n';

    res = aesCrypto.decrypt(res);

    std::cout << "AES long Decrypted: " << res << '\n';
    return 0;
}