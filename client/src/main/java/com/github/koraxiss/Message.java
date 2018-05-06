package com.github.koraxiss;

public class Message {
    private MessageType type;
    private Packet packet;
    private String string;

    public Message(MessageType type, Packet packet, String string) {
        this.type = type;
        this.packet = packet;
        this.string = string;
    }

    public Packet getPacket() {
        return packet;
    }

    public MessageType getType() {
        return type;
    }

    public String getString() {
        return string;
    }

    public enum MessageType {
        PACKET,
        CLOSE,
        ERROR
    }
}