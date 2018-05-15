package com.github.koraxiss;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;
import java.io.*;
import java.security.*;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;

public class CipherModule {
    private static KeyPairGenerator keyPairGenerator;
    private static KeyPair keyPair;
    private static Key serverPublicKey;
    private static Key sessionKey;

    private static Cipher encryptClientPrivate;
    private static Cipher decryptClientPrivate;

    private static Cipher encryptServerPublic;
    private static Cipher decryptServerPublic;

    private static Cipher encryptSession;
    private static Cipher decryptSession;

    private static State state;

    enum State{
        ESTABLISHING_CONNECTION,
        CIPHERING_DATA
    }

    static KeyPair generateKeyPair() throws NoSuchAlgorithmException, IOException {
        keyPairGenerator = KeyPairGenerator.getInstance("RSA");
        keyPairGenerator.initialize(512);
        keyPair = keyPairGenerator.generateKeyPair();

        byte[] publicKey = keyPair.getPublic().getEncoded();
        byte[] privateKey = keyPair.getPrivate().getEncoded();

        File dir = new File("key");
        if(!dir.exists())   dir.mkdir();

        File publicKeyFile = new File("key/public_key");
        File privateKeyFile = new File("key/private_key");

        if(!publicKeyFile.exists())     publicKeyFile.createNewFile();
        if(!privateKeyFile.exists())    privateKeyFile.createNewFile();

        FileOutputStream publicKeyFOS = new FileOutputStream(publicKeyFile);
        FileOutputStream privateKeyFOS = new FileOutputStream(privateKeyFile);

        publicKeyFOS.write(publicKey);
        publicKeyFOS.close();

        privateKeyFOS.write(privateKey);
        privateKeyFOS.close();

        return keyPair;
    }

    static KeyPair loadKeyPair() throws IOException, NoSuchAlgorithmException, InvalidKeySpecException {
        FileInputStream publicKeyFIS = new FileInputStream("key/public_key");
        FileInputStream privateKeyFIS = new FileInputStream("key/private_key");

        byte[] publicKeyBytes = new byte[publicKeyFIS.available()];
        byte[] privateKeyBytes = new byte[privateKeyFIS.available()];

        publicKeyFIS.read(publicKeyBytes);
        publicKeyFIS.close();

        privateKeyFIS.read(privateKeyBytes);
        privateKeyFIS.close();

        X509EncodedKeySpec pubKeySpec = new X509EncodedKeySpec(publicKeyBytes);
        KeyFactory pubKeyFactory = KeyFactory.getInstance("RSA");
        PublicKey pubKey = pubKeyFactory.generatePublic(pubKeySpec);

        PKCS8EncodedKeySpec privKeySpec = new PKCS8EncodedKeySpec(privateKeyBytes);
        KeyFactory privKeyFactory = KeyFactory.getInstance("RSA");
        PrivateKey privKey = privKeyFactory.generatePrivate(privKeySpec);

        keyPair = new KeyPair(pubKey, privKey);
        return keyPair;
    }

    public static void printKeyPair(){
        System.out.println(keyPair.getPublic().toString());
        System.out.println(keyPair.getPrivate().toString());
    }

    public static void loadServerPublic(){

    }

    public static void loadSessionKey(){

    }

    public static void initializeClientCiphers() throws NoSuchPaddingException, NoSuchAlgorithmException, InvalidKeyException {
        encryptClientPrivate = Cipher.getInstance("RSA");
        decryptClientPrivate = Cipher.getInstance("RSA");

        encryptClientPrivate.init(Cipher.ENCRYPT_MODE, keyPair.getPrivate());
        decryptClientPrivate.init(Cipher.DECRYPT_MODE, keyPair.getPrivate());
    }

    public static void initializeServerCiphers() throws NoSuchPaddingException, NoSuchAlgorithmException, InvalidKeyException {
        encryptServerPublic = Cipher.getInstance("RSA");
        decryptServerPublic = Cipher.getInstance("RSA");

        encryptServerPublic.init(Cipher.ENCRYPT_MODE, serverPublicKey);
        decryptServerPublic.init(Cipher.DECRYPT_MODE, serverPublicKey);
    }

    public static void initializeSessionCiphers() throws NoSuchPaddingException, NoSuchAlgorithmException, InvalidKeyException {
        encryptSession = Cipher.getInstance("AES");
        decryptSession = Cipher.getInstance("AES");

        encryptSession.init(Cipher.ENCRYPT_MODE, sessionKey);
        encryptSession.init(Cipher.ENCRYPT_MODE, sessionKey);
    }


    public static byte[] encrypt(byte[] buffer, int offset, int n) throws BadPaddingException, IllegalBlockSizeException {
        return encryptClientPrivate.doFinal(buffer, offset, n);
    }

    public static byte[] decrypt(byte[] buffer, int offset, int n) throws BadPaddingException, IllegalBlockSizeException {
        return decryptServerPublic.doFinal(buffer, offset, n);
    }

    public static State getState() {
        return state;
    }

    public static void setState(State state) {
        CipherModule.state = state;
    }
}
