package com.volterra.tin.petarda;


public class TransferThread implements Runnable{
    private static boolean isRunning = true;

    @Override
    public void run() {
        android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_BACKGROUND);
//        Thread thread = new Thread(() -> {
//            try {
//                ThreadConnector.sendLocation();
//                Thread.sleep(5000);
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
//        });
//        thread.start();
        while (isRunning) {
           ListActivity.runOnUIThread(ThreadConnector::poll);
            try {
                ThreadConnector.sendLocation();
                Thread.sleep(2000);
            } catch (InterruptedException e) {
                setNotRunning();
                e.printStackTrace();
            }
        }
    }

    public static void setNotRunning() {
        TransferThread.isRunning = false;
    }
}
