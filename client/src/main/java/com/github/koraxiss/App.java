package com.github.koraxiss;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;

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
        Messenger messenger = new SimpleMessenger("10.78.20.61", 9999);
        Messenger messenger1 = new ProtocolMessenger(messenger);
        try {
//            Packet packet1 = messenger1.receive();
//            System.out.println(packet1.getType() + " " + (int) packet1.getArg1() + " " + (String) packet1.getArg2());
            Thread.sleep(1000);
            bytes = new byte[82];
            messenger1.receive(bytes, 0, 82);
            System.out.println(Converter.byteToString(bytes));
            messenger1.send(bytes, 0, 82);
//            Thread.sleep(1000);
            messenger1.receive(bytes, 0, 82);
            System.out.println(Converter.byteToString(bytes));
            messenger1.send(bytes, 0, 82);
//            messenger1.send(bytes, 0, 82);
//            messenger1.receive(bytes, 0, 82);
        } catch (IOException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
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
