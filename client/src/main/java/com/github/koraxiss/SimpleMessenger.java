package com.github.koraxiss;

import java.io.*;
import java.net.Socket;

public class SimpleMessenger implements Messenger {
    private Socket socket;
    private InputStream input;
    private OutputStream output;

    public SimpleMessenger(String host, int port) throws IOException {
            socket = new Socket(host, port);
            input = socket.getInputStream();
            output = socket.getOutputStream();
    }

    public void closeSocket() throws IOException {
        this.socket.close();
    }

    public void send(byte[] buffer, int n) throws IOException {
        byte[] message = new byte[buffer.length + 1];
        short size = (short) (buffer.length + 1);
        byte sizeByte = (byte)size;
        message[0] = sizeByte;
        for(int i = 0; i < buffer.length; ++i)
            message[i + 1] = buffer[i];
        output.write(message, 0, message.length);
    }

    public int receive(byte[] buffer, int n) throws IOException {
        input.read(buffer, 0, 1);
        byte size = buffer[0];
        short s = size;
        int i = input.read(buffer, 0, s - 1);
        return i;
    }

    public void send(Packet packet) throws IOException {
        send(packet.getBuffer(), packet.getBuffer().length);
        System.out.println("Sent packet " + packet.getType());
    }

    public Packet receive(byte[] buffer) throws IOException {
        int i = receive(buffer, 0);
        int type = buffer[0];
        System.out.println("Received packet " + PacketType.packetTypeMap.get(type));
        PacketType packetType = PacketType.packetTypeMap.get(type);
        switch (packetType) {
            case _OPEN:
                return new Packet(PacketType._OPEN);
            case _CLOSE:
                return new Packet(PacketType._CLOSE);
            case _ACK_ERR:
                return new Packet(PacketType._ACK_ERR);
            case _ACK_OK:
                return new Packet(PacketType._ACK_OK);
            case _ALIVE:
                return new Packet(PacketType._ALIVE);
            default:
                return null;
        }
    }
}
