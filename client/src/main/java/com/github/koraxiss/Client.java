package com.github.koraxiss;

import java.io.IOException;

public class Client {
    private Thread readerThread;
    private Thread writerThread;
    private boolean running;

    public Client() throws IOException {
        readerThread = new Thread(new ReaderThread());
        writerThread = new Thread(new WriterThread());
        running = true;
        this.start();
    }

    public void start() {
        writerThread.start();
        readerThread.start();
    }

    public void stop() {
        running = false;
        readerThread.interrupt();
        writerThread.interrupt();
    }

    private class ReaderThread implements Runnable {

        @Override
        public void run() {
            try {
                while (running) {
                    Thread.sleep(3000);
                }
            } catch (InterruptedException e) {
                System.out.println("Reader thread interrupted");
                e.printStackTrace();
            } finally {

            }
        }
    }

    private class WriterThread implements Runnable {

        @Override
        public void run() {
            try {
                while (running) {
                    Thread.sleep(100);
                }
            } catch (InterruptedException e) {
                System.out.println("Writer thread interrupted");
                e.printStackTrace();
            } finally {

            }
        }
    }
}

