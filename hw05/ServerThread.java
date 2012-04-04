/**
 * Simulates a server as a thread.
 */
public class ServerThread extends Thread implements Server {

    Queue queue;
    double meanServiceTime;

    public ServerThread(Queue queue, double meanServiceTime) {
        this.queue = queue;
        this.meanServiceTime = meanServiceTime;
    }

    @Override
    public void run() {
        Customer customer;
        try {
            while (true) {
                // Get the next customer for us
                customer = queue.nextCustomer(this);

                // In case the simulation has ended:
                if (customer == null) {
                    break;
                }

                customer.provideService();

                // Simulate service time
                Simulation.exponentialSleep(meanServiceTime);
            }
        } catch (InterruptedException e) {
            // This happens at the end if we're
            // still waiting for customers in the
            // queue when the simulation is over.
        }
    }

}
