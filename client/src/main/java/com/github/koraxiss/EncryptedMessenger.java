package com.github.koraxiss;

import java.io.IOException;

public class EncryptedMessenger implements Messenger {
    Messenger simpleMessenger;

    public EncryptedMessenger(Messenger simpleMessenger) {
        this.simpleMessenger = simpleMessenger;
    }

    public void send(byte[] buffer, int n) throws IOException {
        CipherModule.encrypt(buffer, 0, n);
        simpleMessenger.send(buffer, n);
    }

    public int receive(byte[] buffer, int n) throws IOException {
        int i = simpleMessenger.receive(buffer, n);
        CipherModule.decrypt(buffer, 0, i);
        return i;
    }

    public void send(Packet packet) throws IOException {
        byte[] buffer = packet.getBuffer();
        send(buffer, buffer.length);
    }

    public Packet receive(byte[] buffer) throws IOException {
        int i = receive(buffer, 0);
        int type = buffer[0];
        PacketType packetType = PacketType.packetTypeMap.get(type);
        switch (packetType) {
            case _OPEN_ENCR:
                return new Packet(PacketType._OPEN_ENCR);
            case _PUBLIC_KEY:
                return null;
            case _SYMMETRIC_KEY:
                byte[] key = new byte[i - 1];
                for (int j = 0; j < i - 1; ++j)
                    key[j] = buffer[j + 1];
                Packet packet = new Packet(PacketType._SYMMETRIC_KEY);
                packet.setArg1(Converter.byteToString(key));
                return packet;
            case _TEST_KEY:
                byte[] test = new byte[i - 1];
                for (int j = 0; j < i - 1; ++j)
                    test[j] = buffer[j + 1];
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
