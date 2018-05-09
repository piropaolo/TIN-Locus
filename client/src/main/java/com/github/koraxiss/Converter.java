package com.github.koraxiss;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

public class Converter {
    public static byte intToByte(int i) {
        return (byte)i;
    }

    public static int byteToInt(byte b) {
        return b;
    }

    public static byte[] floatToByte(float f) {
        byte[] bytes = new byte[4];
        ByteBuffer.wrap(bytes).putFloat(f);
        return bytes;
    }

    public static float byteToFloat(byte[] b) {
        ByteBuffer byteBuffer = ByteBuffer.wrap(b);
        return byteBuffer.getFloat();
    }

    public static byte[] shortToByte(short s) {
        byte[] bytes = new byte[2];
        ByteBuffer buffer = ByteBuffer.allocate(bytes.length);
        buffer.putShort(s);
        return buffer.array();
    }

    public static short byteToShort(byte[] bytes) {
        ByteBuffer byteBuffer = ByteBuffer.wrap(bytes);
        return byteBuffer.getShort();
    }

    public static byte[] stringToByte(String string) {
        return string.getBytes(StandardCharsets.UTF_8);
    }

    public static String byteToString(byte[] bytes) {
        return new String(bytes, StandardCharsets.UTF_8);
    }

    public static long byteToLong(byte[] longBytes){
        ByteBuffer byteBuffer = ByteBuffer.wrap(longBytes);
        return byteBuffer.getLong();
    }

    public static byte[] longToByte(long l) {
        byte[] bytes = new byte[8];
        ByteBuffer buffer = ByteBuffer.allocate(bytes.length);
        buffer.putLong(l);
        return buffer.array();
    }
}
