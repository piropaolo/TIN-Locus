package com.github.koraxiss;

import java.io.IOException;

public class ProtocolMessenger {
    EncryptedMessenger encryptedMessenger;

    public ProtocolMessenger(EncryptedMessenger encryptedMessenger) {
        this.encryptedMessenger = encryptedMessenger;
    }

    public void send(byte[] buffer, int offset, int n) throws IOException {
        encryptedMessenger.send(buffer, offset, n);
    }

    public void receive(byte[] buffer, int offset, int n) throws IOException {
        encryptedMessenger.receive(buffer, offset, n);
    }

    public void send(Packet packet) throws IOException {
        byte[] buffer = packet.getBuffer();
        send(buffer, 0, packet.getSize());
    }

    public Packet receive() throws IOException {
        byte[] buffer = Client.getBuffer();
        receive(buffer, 0, 1);
        int type = buffer[0];
        PacketType packetType = PacketType.packetTypeMap.get(type);
        switch (packetType) {
            case _OPEN_PROT:
            case _SET_NAME:
            case _ADD_CLIENT:
            case _NEW_CLIENT:
            case _REMOVE_CLIENT:
            case _REMOVE_MY_CLIENT:
            case _MY_LOCATION:
            case _LOCATION:
            case _ACK_ERR:
            case _ACK_OK:
            case _ALIVE:
            case _CLOSE:
            default:
                return null;
        }
    }

    public static void init() throws IOException {

    }
}
