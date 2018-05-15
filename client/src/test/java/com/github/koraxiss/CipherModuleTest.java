package com.github.koraxiss;

import org.junit.Test;
import static org.junit.Assert.*;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;

public class CipherModuleTest {
    @Test
    public void textUponEncryptionAndDecryptionIsTheSame() throws BadPaddingException, IllegalBlockSizeException {
        String plainText = "testinho";
        byte[] cipherText = CipherModule.encrypt(Converter.stringToByte(plainText), 0, plainText.length());
        byte[] decryptedText = CipherModule.decrypt(cipherText, 0, cipherText.length);

        assertEquals(plainText, Converter.byteToString(decryptedText));
    }
}
