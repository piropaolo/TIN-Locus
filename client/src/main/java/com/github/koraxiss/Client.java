package com.github.koraxiss;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

public class Client {
    Socket socket;
    DataInputStream input;
    DataOutputStream output;
    private Thread readerThread;
    private Thread writerThread;
    private boolean running;

    public Client() throws IOException {
        socket = new Socket("google.com", 80);
        input = new DataInputStream(socket.getInputStream());
        output = new DataOutputStream(socket.getOutputStream());

        readerThread = new Thread(new ReaderThread());
        writerThread = new Thread(new WriterThread());
        running = true;
        this.start();
    }

    public void start() {
        readerThread.start();
        writerThread.start();
    }

    public void stop() {
        running = false;
        readerThread.interrupt();
        writerThread.interrupt();
    }

    private class ReaderThread implements Runnable {

        @Override
        public void run() {
            while (running) {
                try {
                    Thread.sleep(3000);
                } catch (InterruptedException e) {
                    System.out.println("Reader thread interrupted");
                    e.printStackTrace();
                }
            }
        }
    }

    private class WriterThread implements Runnable {

        @Override
        public void run() {
            while (running) {
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    System.out.println("Writer thread interrupted");
                    e.printStackTrace();
                }
            }
        }
    }
}
