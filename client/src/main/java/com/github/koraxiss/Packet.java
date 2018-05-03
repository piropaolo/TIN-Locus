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

    public Object getArg1() {
        return arg1;
    }

    public void setArg1(Object arg1) {
        this.arg1 = arg1;
    }

    public Object getArg2() {
        return arg2;
    }

    public void setArg2(Object arg2) {
        this.arg2 = arg2;
    }

    public Object getArg3() {
        return arg3;
    }

    public void setArg3(Object arg3) {
        this.arg3 = arg3;
    }

    public Object getArg4() {
        return arg4;
    }

    public void setArg4(Object arg4) {
        this.arg4 = arg4;
    }
}
