package com.github.koraxiss;

import java.io.*;

/**
 * Hello world!
 */
public class App {
    public static void main(String[] args) {
        System.out.println("Hello World!");
        Packet packet = new Packet(PacketType._SET_NAME);
        packet.setArg1((int) 3);
        packet.setArg2((String) "abc");
        byte[] bytes = packet.getBuffer();
        InputStream inputStream = new ByteArrayInputStream(bytes);
        Messenger messenger = new SimpleMessenger(inputStream);
        Messenger messenger1 = new ProtocolMessenger(messenger);
        try {
            Packet packet1 = messenger1.receive();
            System.out.println(packet1.getType() + " " + (int) packet1.getArg1() + " " + (String) packet1.getArg2());
        } catch (IOException e) {
            e.printStackTrace();
        }

//        String s = "Aaa";
//        byte[] bytes = Converter.stringToByte(s);
//        System.out.println(bytes);
//        String x = Converter.byteToString(bytes);
//        System.out.println(x);
//        try {
//            Client client = new Client();
//            Thread.sleep(1000);
//            client.stop();;
//        } catch (IOException e) {
//            e.printStackTrace();
//        } catch (InterruptedException e) {
//            e.printStackTrace();
//        }
    }
}
