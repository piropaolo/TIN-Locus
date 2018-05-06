package com.github.koraxiss;

import java.io.IOException;

public class ProtocolMessenger implements Messenger {
    Messenger encryptedMessenger;

    public ProtocolMessenger(Messenger encryptedMessenger) {
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
        send(buffer, 0, buffer.length);
    }

    public Packet receive() throws IOException {
        byte[] buffer = new byte[100];
        receive(buffer, 0, 1);
        int type = Converter.byteToInt(buffer[0]);
        PacketType packetType = PacketType.packetTypeMap.get(type);
        switch (packetType) {
            case _OPEN_PROT:
                return new Packet(PacketType._OPEN_PROT);
            case _SET_NAME:
                Packet packet = new Packet(PacketType._SET_NAME);
                receive(buffer, 0, 1);
                int size = Converter.byteToInt(buffer[0]);
                byte[] name = new byte[size];
                receive(name, 0, size);
                String string = Converter.byteToString(name);
                packet.setArg1((Object) size);
                packet.setArg2((Object) string);
                return packet;
            case _ADD_FOLLOWER:
                return null;
            case _NEW_FOLLOWED:
                Packet packet1 = new Packet(PacketType._NEW_FOLLOWED);
                byte[] id = new byte[2];
                receive(id, 0, 2);
                short sId = Converter.byteToShort(id);
                packet1.setArg1((Object) sId);
                receive(buffer, 0, 1);
                int size1 = Converter.byteToInt(buffer[0]);
                packet1.setArg2((Object) size1);
                byte[] name1 = new byte[size1];
                receive(name1, 0, size1);
                String string1 = Converter.byteToString(name1);
                packet1.setArg3((Object) string1);
                return packet1;
            case _REMOVE_FOLLOWER:
                return null;
            case _REMOVE_FOLLOWED:
                Packet packet2 = new Packet(PacketType._REMOVE_FOLLOWED);
                receive(buffer, 0, 1);
                int size2 = Converter.byteToInt(buffer[0]);
                byte[] name2 = new byte[size2];
                receive(name2, 0, size2);
                String string2 = Converter.byteToString(name2);
                packet2.setArg1((Object) size2);
                packet2.setArg2((Object) string2);
                return packet2;
            case _MY_LOCATION:
                return null;
            case _LOCATION:
                Packet packet3 = new Packet(PacketType._LOCATION);
                byte[] id1 = new byte[2];
                receive(id1, 0, 2);
                short sId1 = Converter.byteToShort(id1);
                packet3.setArg1((Object) sId1);
                byte[] lat = new byte[8];
                receive(lat, 0, 8);
                float latitude = Converter.byteToFloat(lat);
                packet3.setArg2((Object) latitude);
                byte[] lon = new byte[8];
                receive(lon, 0, 8);
                float longitude = Converter.byteToFloat(lat);
                packet3.setArg3((Object) longitude);
                byte[] t = new byte[8];
                receive(t, 0, 8);
                long time = Converter.byteToLong(t);
                packet3.setArg4((Object) time);
                return packet3;
            case _ACK_ERR:
                return new Packet(PacketType._ACK_ERR);
            case _ACK_OK:
                return new Packet(PacketType._ACK_OK);
            case _CLOSE:
                return new Packet(PacketType._CLOSE);
            case _ALIVE:
                return new Packet(PacketType._ALIVE);
            default:
                return null;
        }
    }

    public static void init() throws IOException {

    }
}

