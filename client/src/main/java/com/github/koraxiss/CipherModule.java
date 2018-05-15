package com.github.koraxiss;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import java.security.InvalidKeyException;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;

public class CipherModule {
    private static KeyPairGenerator keyPairGenerator;
    private static KeyPair keyPair;
    private static Cipher cipherEncrypt;
    private static Cipher cipherDecrypt;

    static {
        try {
            keyPairGenerator = KeyPairGenerator.getInstance("RSA");
            cipherEncrypt = Cipher.getInstance("RSA");
            cipherDecrypt = Cipher.getInstance("RSA");
        } catch (NoSuchAlgorithmException | NoSuchPaddingException e) {
            e.printStackTrace();
        }

        keyPairGenerator.initialize(512);
        keyPair = keyPairGenerator.generateKeyPair();

        try {
            cipherEncrypt.init(Cipher.ENCRYPT_MODE, keyPair.getPrivate());
            cipherDecrypt.init(Cipher.DECRYPT_MODE, keyPair.getPublic());
        } catch (InvalidKeyException e) {
            e.printStackTrace();
        }
    }

    public static byte[] encrypt(byte[] buffer, int offset, int n) throws BadPaddingException, IllegalBlockSizeException {
        return cipherEncrypt.doFinal(buffer, offset, n);
    }

    public static byte[] decrypt(byte[] buffer, int offset, int n) throws BadPaddingException, IllegalBlockSizeException {
        return cipherDecrypt.doFinal(buffer, offset, n);
    }
}
