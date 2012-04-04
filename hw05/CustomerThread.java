/**
 * Ian Dimayuga
 * EECS 338 Assignment 5
 * CustomerThread.java
 */
public class CustomerThread extends Thread implements Customer {

    private boolean m_Served;
    private Queue m_Queue;
    private long m_WaitTime;
    private long m_DoneTime;

    public CustomerThread(Queue queue) {
        m_Queue = queue;
        m_Served = false;
        m_WaitTime = m_DoneTime = 0;
    }

    @Override
    public void run() {
        try {
            m_Queue.enterQueue(this);
            this.waitForService();
        } catch (InterruptedException e) {
        }
    }

    public synchronized long waitForService() throws InterruptedException {
        m_WaitTime = System.currentTimeMillis();
        this.wait();
    }

    public synchronized void provideService() {
        m_DoneTime = System.currentTimeMillis();
        m_Served = true;
        this.notify();
    }

    public long getWaitTime() {
        return m_DoneTime - m_WaitTime;
    }
}
