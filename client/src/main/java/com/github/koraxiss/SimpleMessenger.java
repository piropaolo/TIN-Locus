package com.github.koraxiss;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class SimpleMessenger implements Messenger {
    private Socket socket;
    private InputStream input;
    private OutputStream output;

    public SimpleMessenger(String host, int port) {
        try {
            socket = new Socket(host, port);
            input = socket.getInputStream();
            output = socket.getOutputStream();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void send(byte[] buffer, int offset, int n) throws IOException {
        output.write(buffer, offset, n);
    }

    public void receive(byte[] buffer, int offset, int n) throws IOException {
        input.read(buffer, offset, n);
    }

    public void send(Packet packet) throws IOException {
        send(packet.getBuffer(), 0, packet.getSize());
    }

    public Packet receive() throws IOException {
        byte[] buffer = Client.getBuffer();
        receive(buffer, 0, 1);
        int type = buffer[0];
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

    public void init() {

    }
}
