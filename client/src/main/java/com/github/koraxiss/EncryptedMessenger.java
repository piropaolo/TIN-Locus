package com.github.koraxiss;

import java.io.IOException;

public class EncryptedMessenger implements Messenger {
    Messenger simpleMessenger;

    public EncryptedMessenger(Messenger simpleMessenger) {
        this.simpleMessenger = simpleMessenger;
    }

    public void send(byte[] buffer, int offset, int n) throws IOException {
        CipherModule.encrypt(buffer, offset, n);
        simpleMessenger.send(buffer, offset, n);
    }

    public void receive(byte[] buffer, int offset, int n) throws IOException {
        simpleMessenger.receive(buffer, offset, n);
        CipherModule.decrypt(buffer, offset, n);
    }

    public void send(Packet packet) throws IOException {
        byte[] buffer = packet.getBuffer();
        send(buffer, 0, 1);
        send(buffer, 1, packet.getSize() - 1);
    }

    public Packet receive() throws IOException {
        byte[] buffer = Client.getBuffer();
        receive(buffer, 0, 1);
        int type = buffer[0];
        PacketType packetType = PacketType.packetTypeMap.get(type);
        switch (packetType) {
            case _OPEN_ENCR:
            case _PUBLIC_KEY:
            case _SYMMETRIC_KEY:
            case _TEST_KEY:
            case _ACK_ERR:
            case _ACK_OK:
            case _CLOSE:
            case _ALIVE:
            default:
                return null;
        }
    }

    public static void init() throws IOException {

    }
}
