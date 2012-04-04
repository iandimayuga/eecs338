/**
 * Ian Dimayuga
 * EECS 338 Assignment 5
 * GroceryShortest.java
 * Grocery Queue where customers choose the shortest lane.
 */

import java.util.*;
public class GroceryShortest extends GroceryQueue {
    public LinkedList<Customer> laneChoice() {
        LinkedList<Customer> shortest = null;

        //find the first minimum length queue
        for( LinkedList<Customer> queue : m_Queues.values()) {
            if( shortest == null || queue.size() < shortest.size())
                shortest = queue;
        }

        return shortest;
    }
}
