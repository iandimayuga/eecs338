/**
 * Ian Dimayuga
 * EECS 338 Assignment 5
 * Simulation.java
 *
 * Runs the simulation.
 * TODO: Implement `main` and finish implementation of `simulate`
 */
public class Simulation {

    public static final int CUSTOMERS = 1000;
    public static final int SERVERS = 5;
    public static final double MEAN_INTERARRIVAL_TIME = 8;
    public static final double MEAN_SERVICE_TIME = 32;

    public static int s_Customers = CUSTOMERS;

    public static void main(String[] args) {
        // TODO: Run the simulation with each of the three
        // queues, and print out the results.
        BankQueue bank = new BankQueue();
        try {
            System.out.printf("Bank Queue: %f seconds\n", simulate(bank));
        } catch( InterruptedException e) {
            System.out.println("ERROR: Bank Queue interrupted.");
        }
    }

    /**
     * Simulates the given queue, and returns the average wait time of a customer.
     */
    public static double simulate(Queue queue) throws InterruptedException {

        // For recording customer wait time
        double totalWaitTime = 0;

        // Create server threads, add to queue, and start
        ServerThread[] servers = new ServerThread[SERVERS];
        for (int i = 0; i < SERVERS; i++) {
            ServerThread server = new ServerThread(queue, MEAN_SERVICE_TIME);
            servers[i] = server;
            queue.addServer(server);
        }
        for( ServerThread server : servers) {
            server.start();
        }

        /******************** TODO ********************/
        // Create and start customer threads, using
        // exponentialSleep(MEAN_INTERARRIVAL_TIME);
        // between starting each customer.
        /**********************************************/
        //Create customer threads
        CustomerThread[] customers = new CustomerThread[s_Customers];
        for( int i = 0; i < s_Customers; i++) {
            CustomerThread customer = new CustomerThread( queue);
            customers[i] = customer;
            queue.addCustomer( customer);
        }
        for( CustomerThread customer : customers) {
            //wait for the customer to arrive
            exponentialSleep( MEAN_INTERARRIVAL_TIME);
            customer.start();
        }


        /******************** TODO ********************/
        // Join in each customer thread, and add its
        // wait time (using getWaitTime()) to the
        // totalWaitTime.
        /**********************************************/
        for( CustomerThread customer : customers) {
            try {
                customer.join();
                totalWaitTime += customer.getWaitTime();
            } catch( InterruptedException e) {}
        }

        // Close the queue
        queue.close();

        // Interrupt and join in server threads
        for (ServerThread server : servers) {
            server.interrupt();
            server.join();
        }

        return totalWaitTime / s_Customers;
    }

    /**
     * Sleeps for an exponentially distributed random amount of time,
     * with given average.
     */
    public static void exponentialSleep(double meanSleepTime) throws InterruptedException {
        long k = Math.round(-Math.log(Math.random()) * meanSleepTime);
        Thread.sleep(k);
    }

}
