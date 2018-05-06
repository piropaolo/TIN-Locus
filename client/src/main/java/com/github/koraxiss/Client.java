package com.github.koraxiss;

import java.io.IOException;
import java.net.SocketException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class Client {
    private SimpleMessenger simpleMessenger;
    private EncryptedMessenger encryptedMessenger;
    private ProtocolMessenger protocolMessenger;
    private BlockingQueue<Message> childMessages;
    private BlockingQueue<Message> sendInstructions;
    private Thread readerThread;
    private Thread writerThread;
    private boolean running;

    public Client() throws IOException {
        simpleMessenger = new SimpleMessenger("0.0.0.0", 9999);
        encryptedMessenger = new EncryptedMessenger(simpleMessenger);
        protocolMessenger = new ProtocolMessenger(encryptedMessenger);
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
        while (running) {
            try {
                message = childMessages.poll();
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
            byte[] bytes = new byte[81];
            while (running) {
                try {
//                    packet = simpleMessenger.receive();
                    simpleMessenger.receive(bytes, 0, 81);
                    childMessages.put(new Message(Message.MessageType.PACKET, packet, Converter.byteToString(bytes)));
                    Thread.sleep(500);
                } catch (IOException e1) {
//                    e1.printStackTrace();
                    try {
                        childMessages.put(new Message(Message.MessageType.ERROR, null, null));
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
                    message = sendInstructions.poll();
                    if (message != null && message.getType() == Message.MessageType.PACKET) {
                        packet = message.getPacket();
                        simpleMessenger.send(Converter.stringToByte(message.getString()), 0, 81);
                    }
                    Thread.sleep(500);
                } catch (IOException e1) {
//                    e1.printStackTrace();
                    try {
                        childMessages.put(new Message(Message.MessageType.ERROR, null, null));
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

