package com.github.koraxiss;

import javax.crypto.NoSuchPaddingException;
import java.io.IOException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.spec.InvalidKeySpecException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class Client {
    private LayerManager layerManager;
    private BlockingQueue<Message> childMessages;
    private BlockingQueue<Message> sendInstructions;
    private Thread readerThread;
    private Thread writerThread;
    private boolean running;

    public Client() throws IOException {
        layerManager = new LayerManager();
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
                message = childMessages.take();
                if (message.getType() == Message.MessageType.PACKET) {
                    switch(message.getPacket().getType()) {
                        case _OPEN:
                        case _OPEN_PROT:
                        case _ALIVE:
                            sendInstructions.put(new Message(Message.MessageType.PACKET, new Packet(PacketType._ACK_OK)));
                            break;
                        case _OPEN_ENCR:
                            CipherModule.initializeClientCiphers();
                            CipherModule.initializeServerCiphers();
                            layerManager.setEncryptionOn(true);
                            CipherModule.setState(CipherModule.State.SERVER_PUBLIC);
                            Packet packet6 = new Packet(PacketType._PUBLIC_KEY);
                            packet6.setArg1(CipherModule.getKeyPair().getPublic());
                            sendInstructions.put(new Message(Message.MessageType.PACKET, packet6));
                            CipherModule.setState(CipherModule.State.CLIENT_PRIVATE_SERVER_PUBLIC);
                            break;
                        case _CLOSE:
                            stop();
                            break;
                        case _SYMMETRIC_KEY:
                            CipherModule.setSessionKey((String) message.getPacket().getArg1());
                            sendInstructions.put(new Message(Message.MessageType.PACKET, message.getPacket()));
                            message = childMessages.take();
                            if(message.getType() != Message.MessageType.PACKET && message.getPacket().getType() != PacketType._ACK_OK)
                                stop();
                            CipherModule.setState(CipherModule.State.SESSION);
                            CipherModule.initializeSessionCiphers();
                            break;
                        case _TEST_KEY:
//                            do something with challenge;
                            Packet packet = new Packet(PacketType._TEST_KEY);
//                            packet.setArg1(challenge after doing soething with it);
                            sendInstructions.put(new Message(Message.MessageType.PACKET, packet));
                            break;
                        case _SET_NAME:
//                            get name from gui
                            Packet packet1 = new Packet(PacketType._SET_NAME);
//                            packet.setArg1(name that we got from gui);
                            sendInstructions.put(new Message(Message.MessageType.PACKET, packet1));
                            break;
                        case _ADD_FOLLOWER:
//                          get who we want to follow from gui
                            Packet packet4 = new Packet(PacketType._ADD_FOLLOWER);
//                            action initialized by giu so we have name from there
//                            packet2.setArg1(name from gui);
                            sendInstructions.put(new Message(Message.MessageType.PACKET, packet4));
                            break;
                        case _NEW_FOLLOWED:
//                            tell gui to display information about new followed user
                        case _REMOVE_FOLLOWED:
                            Packet packet3 = new Packet(PacketType._REMOVE_FOLLOWED);
//                            action initialized by giu so we have name from there
//                            packet2.setArg1(name from gui);
                            sendInstructions.put(new Message(Message.MessageType.PACKET, packet3));
                            break;
                        case _REMOVE_FOLLOWER:
                            Packet packet2 = new Packet(PacketType._REMOVE_FOLLOWER);
//                            action initialized by giu so we have name from there
//                            packet2.setArg1(name from gui);
                            sendInstructions.put(new Message(Message.MessageType.PACKET, packet2));
                            break;
                        case _LOCATION:
//                            tell gui to update position of client of given id
                            break;
                        case _ACK_ERR:
                            break;
                        case _ACK_OK:
                            break;
                        case _MY_LOCATION:
//                            gui gives us location
                            Packet packet5 = new Packet(PacketType._REMOVE_FOLLOWED);
//                            action initialized by giu so we have name from there
//                            packet2.setArg1(latitude from gui);
//                            packet2.setArg2(longitude from gui);
//                            packet2.setArg3(delta time from gui);
                            sendInstructions.put(new Message(Message.MessageType.PACKET, packet5));
                            break;
                    }
                } else
                    stop();
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            } catch (NoSuchPaddingException e) {
                e.printStackTrace();
            } catch (NoSuchAlgorithmException e) {
                e.printStackTrace();
            } catch (InvalidKeyException e) {
                e.printStackTrace();
            } catch (InvalidKeySpecException e) {
                e.printStackTrace();
            } catch (IOException e) {
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
            layerManager.closeSocket();
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
                    byte[] bytes = new byte[5];
                    layerManager.receive();
//                    packet = layerManager.receive();
                    packet = new Packet(PacketType._OPEN);
                    packet.setArg1(Converter.byteToString(bytes));
                    childMessages.put(new Message(Message.MessageType.PACKET, packet));
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
                    message = sendInstructions.take();
                    if (message.getType() == Message.MessageType.PACKET) {
                        packet = message.getPacket();
                        layerManager.send(packet);
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

