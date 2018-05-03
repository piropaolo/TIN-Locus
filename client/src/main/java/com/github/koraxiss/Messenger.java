package com.github.koraxiss;

import java.io.IOException;

public interface Messenger {
    void send(byte[] buffer, int offset, int n) throws IOException;
    void receive(byte[] buffer, int offset, int n) throws IOException;
    void send(Packet packet) throws IOException;
    Packet receive() throws IOException;
}
