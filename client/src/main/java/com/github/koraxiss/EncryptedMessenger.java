package com.github.koraxiss;

import javax.crypto.BadPaddingException;
import javax.crypto.IllegalBlockSizeException;
import java.io.IOException;
import java.util.Base64;

public class EncryptedMessenger implements Messenger {
    Messenger simpleMessenger;

    public EncryptedMessenger(Messenger simpleMessenger) {
        this.simpleMessenger = simpleMessenger;
    }

    public void send(byte[] buffer, int n) throws IOException {
//        try {
//            if (CipherModule.getState() != CipherModule.State.SESSION) {
//                int bytesLeft = n;
//                int counter = 0;
//                while (bytesLeft > 0) {
//                    int numberToEncrypt = bytesLeft > 53 ? 53 : bytesLeft;
//                    CipherModule.encrypt(buffer, counter * 53, numberToEncrypt);
//                    bytesLeft -= numberToEncrypt;
//                    ++counter;
//                }
//            } else {
//            if(CipherModule.getState() == CipherModule.State.SESSION)
//                System.arraycopy(CipherModule.encrypt(buffer, 0, n), 0, buffer, 0, n);
//            }
//        } catch (BadPaddingException | IllegalBlockSizeException e) {
//            e.printStackTrace();
//        }
        simpleMessenger.send(buffer, n);
    }

    public int receive(byte[] buffer, int n) throws IOException {
        int i = simpleMessenger.receive(buffer, n);
//        try {
//            if (CipherModule.getState() != CipherModule.State.SESSION) {
//                int bytesLeft = n;
//                int counter = 0;
//                while (bytesLeft > 0) {
//                    int numberToDecrypt = bytesLeft > 22 ? 22 : bytesLeft;
//                    CipherModule.decrypt(buffer, counter * 22, numberToDecrypt);
//                    bytesLeft -= numberToDecrypt;
//                    ++counter;
//                }
//            } else
//            if(CipherModule.getState() == CipherModule.State.SESSION)
//                System.arraycopy(CipherModule.decrypt(buffer, 0, n), 0, buffer, 0, n);
//        } catch (BadPaddingException | IllegalBlockSizeException e) {
//            e.printStackTrace();
//        }
        return i;
    }

    public void send(Packet packet) throws IOException {
        byte[] buffer = packet.getBuffer();
        send(buffer, buffer.length);
    }

    public Packet receive(byte[] buffer) throws IOException {
        int i = receive(buffer, 0);
        int type = buffer[0];
        System.out.println("Received packet " + PacketType.packetTypeMap.get(type));
        PacketType packetType = PacketType.packetTypeMap.get(type);
        System.out.println((int)buffer[0]);
        switch (packetType) {
            case _OPEN_ENCR:
                return new Packet(PacketType._OPEN_ENCR);
            case _PUBLIC_KEY:
                return null;
            case _SYMMETRIC_KEY:
                byte[] key = new byte[i - 1];
                System.arraycopy(buffer, 1, key, 0, i - 1);
                Packet packet = new Packet(PacketType._SYMMETRIC_KEY);
                packet.setArg1(key);
                return packet;
            case _TEST_KEY:
                byte[] test = new byte[i - 1];
                System.arraycopy(buffer, 1, test, 0, i - 1);
                Packet packet1 = new Packet(PacketType._TEST_KEY);
                packet1.setArg1(Converter.byteToString(test));
                return packet1;
            default:
                Packet otherPacket = new Packet(PacketType._OTHER);
                otherPacket.setSize(i);
                return otherPacket;
        }
    }
}
