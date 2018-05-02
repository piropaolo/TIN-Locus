package com.github.koraxiss;

public class Packet {
    private PacketType type;
    private Object arg1;
    private Object arg2;
    private Object arg3;
    private Object arg4;

    public Packet(PacketType type) {
        this.type = type;
    }

    public PacketType getType() {
        return this.type;
    }

    public int getSize() {
        return 0;
    }

    public byte[] getBuffer() {
        return null;
    }
}
