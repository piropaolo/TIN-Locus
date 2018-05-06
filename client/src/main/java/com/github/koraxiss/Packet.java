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
        switch (type) {
            case _ALIVE:
            case _CLOSE:
            case _OPEN:
            case _ACK_ERR:
            case _ACK_OK:
            case _OPEN_ENCR:
            case _OPEN_PROT:
                byte[] bytes = new byte[1];
                byte b = Converter.intToByte(PacketType.reversedMap.get(type));
                bytes[0] = b;
                return bytes;
            case _PUBLIC_KEY:
                return bufferPublicKey();
            case _SYMMETRIC_KEY:
                return bufferSymmetricKey();
            case _TEST_KEY:
                return bufferTestKey();
            case _SET_NAME:
                return bufferSizeName();
            case _ADD_FOLLOWER:
                return bufferSizeName();
            case _NEW_FOLLOWED:
                return bufferNewFollowed();
            case _REMOVE_FOLLOWER:
                return bufferSizeName();
            case _REMOVE_FOLLOWED:
                return bufferSizeName();
            case _MY_LOCATION:
                return bufferMyLocation();
            case _LOCATION:
                return bufferLocation();
            default:
                return null;
        }
    }

    private byte[] bufferLocation() {
        byte[] bytes = new byte[27];
        bytes[0] = Converter.intToByte(PacketType.reversedMap.get(type));
        byte[] shor = Converter.shortToByte((short) this.getArg1());
        for (int i = 0; i < shor.length; ++i)
            bytes[1 + i] = shor[i];
        byte[] lat = Converter.floatToByte((long) this.getArg2());
        for (int i = 0; i < lat.length; ++i)
            bytes[3 + i] = lat[i];
        byte[] lon = Converter.floatToByte((long) this.getArg3());
        for (int i = 0; i < lon.length; ++i)
            bytes[11 + i] = lon[i];
        byte[] t = Converter.longToByte((long) this.getArg4());
        for (int i = 0; i < t.length; ++i)
            bytes[19 + i] = t[i];
        return bytes;
    }

    private byte[] bufferMyLocation() {
        byte[] bytes = new byte[25];
        bytes[0] = Converter.intToByte(PacketType.reversedMap.get(type));
        byte[] lat = Converter.floatToByte((long) this.getArg1());
        for (int i = 0; i < lat.length; ++i)
            bytes[1 + i] = lat[i];
        byte[] lon = Converter.floatToByte((long) this.getArg2());
        for (int i = 0; i < lon.length; ++i)
            bytes[9 + i] = lon[i];
        byte[] dt = Converter.longToByte((long) this.getArg3());
        for (int i = 0; i < dt.length; ++i)
            bytes[17 + i] = dt[i];
        return bytes;
    }

    private byte[] bufferNewFollowed() {
        byte[] bytes = new byte[1 + 2 + 1 + (int) this.getArg2()];
        bytes[0] = Converter.intToByte(PacketType.reversedMap.get(type));
        byte[] shor = new byte[2];
        shor = Converter.shortToByte((short)this.getArg1());
        bytes[1] = shor[0];
        bytes[2] = shor[1];
        bytes[3] = Converter.intToByte((int) this.getArg2());
        byte[] string = Converter.stringToByte((String) this.getArg2());
        for (int i = 0; i < string.length; ++i)
            bytes[4 + i] = string[i];
        return bytes;
    }

    private byte[] bufferSizeName() {
        byte[] bytes = new byte[1 + 1 + (int) this.getArg1()];
        bytes[0] = Converter.intToByte(PacketType.reversedMap.get(type));
        bytes[1] = Converter.intToByte((int) this.getArg1());
        byte[] string = Converter.stringToByte((String) this.getArg2());
        for (int i = 0; i < string.length; ++i)
            bytes[2 + i] = string[i];
        return bytes;
    }

    private byte[] bufferTestKey() {
        return new byte[0];
    }

    private byte[] bufferSymmetricKey() {
        return new byte[0];
    }

    private byte[] bufferPublicKey() {
        return new byte[0];
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
