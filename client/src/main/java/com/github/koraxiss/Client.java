package com.github.koraxiss;

import java.io.IOException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class Client {
    private SimpleMessenger simpleMessenger;
    private EncryptedMessenger encryptedMessenger;
    private ProtocolMessenger protocolMessenger;
    private Messenger activeMessenger;
    private BlockingQueue<Message> childMessages;
    private BlockingQueue<Message> sendInstructions;
    private Thread readerThread;
    private Thread writerThread;
    private boolean running;

    public Client() throws IOException {
        simpleMessenger = new SimpleMessenger("0.0.0.0", 9999);
        encryptedMessenger = new EncryptedMessenger(simpleMessenger);
        protocolMessenger = new ProtocolMessenger(encryptedMessenger);
        activeMessenger = simpleMessenger;
        childMessages = new LinkedBlockingQueue<>();
        sendInstructions = new LinkedBlockingQueue<>();
        readerThread = new Thread(new ReaderThread());
        writerThread = new Thread(new WriterThread());
        running = true;
        this.start();
    }

    public void start() {
        writerThread.start();
        readerThread.start();
        Message message;
        try {
            sendInstructions.put(new Message(Message.MessageType.PACKET, new Packet(PacketType._OPEN)));
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        while (running) {
            try {
//                message = childMessages.poll();
                message = childMessages.take();
                if (message != null) {
                    if (message.getType() == Message.MessageType.PACKET)
                        sendInstructions.put(message);
                    else
                        stop();
                }
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    public void stop() {
        running = false;
        readerThread.interrupt();
        writerThread.interrupt();
        System.out.println("Threads closed");
        try {
            simpleMessenger.closeSocket();
            System.out.println("Socket closed");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private class ReaderThread implements Runnable {

        @Override
        public void run() {
            Packet packet = null;
            while (running) {
                try {
                    packet = simpleMessenger.receive();
                    childMessages.put(new Message(Message.MessageType.PACKET, packet));
//                    childMessages.put(new Message(Message.MessageType.PACKET, packet, Converter.byteToString(bytes)));
                    Thread.sleep(500);
                } catch (IOException e1) {
//                    e1.printStackTrace();
                    try {
                        childMessages.put(new Message(Message.MessageType.ERROR, null));
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                } catch (InterruptedException e) {
//                    e.printStackTrace();
                }
            }
        }
    }

    private class WriterThread implements Runnable {

        @Override
        public void run() {
            Packet packet = null;
            Message message = null;
            while (running) {
                try {
//                    message = sendInstructions.poll();
                    message = sendInstructions.take();
                    if (message.getType() == Message.MessageType.PACKET) {
                        packet = message.getPacket();
                        activeMessenger.send(packet);
                    }
                    Thread.sleep(500);
                } catch (IOException e1) {
//                    e1.printStackTrace();
                    try {
                        childMessages.put(new Message(Message.MessageType.ERROR, null));
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                } catch (InterruptedException e) {
//                    e.printStackTrace();
                }
            }
        }
    }
}

