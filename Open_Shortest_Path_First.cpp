#include <bits/stdc++.h>
using namespace std;

// Forward declaration
class Router;

// Represents a link between routers with a cost
struct RouterLink
{
    Router *neighbor;
    int cost;
    RouterLink(Router *neighbor, int cost)
    {
        this->neighbor = neighbor;
        this->cost = cost;
    }
};

// Router class
class Router
{
public:
    int router_id; // unique router id

    map<int, pair<int, int>> routing_table; // Stores the routing table for all routers

    // vector<pair<Router *, int>> neighbors; // vector of <Router, distance to that router>
    vector<RouterLink> neighbors; // vector of <Router, distance to that router>

    // Router class constructor
    Router(int id) : router_id(id) {}

    // Add a neighboring router with a cost
    void add_neighbor(Router *neighbor, int cost)
    {
        // storing the neighbour information for all routers
        RouterLink newLink(neighbor, cost);
        neighbors.push_back(newLink);
    }

    // function to print the routing table for all routers
    void print_routing_table()
    {
        cout << "----------------The Routing table for the router " << router_id + 1 << " is ----------------------" << endl;
        for (auto &it : routing_table)
        {
            if (it.first == router_id) // not printing for the same router
                continue;
            // cout << "Destination = " << it.first + 1 << " Next Hop = " << it.second.first + 1 << " Cost = ";
            //  if there is an edge from the router
            if (it.second.second != INT_MAX)
            {
                cout << "Destination = " << it.first + 1 << " Cost = ";
                cout << it.second.second << endl;
            }
            else
            {
                cout << "Destination " << it.first + 1;
                cout << " is not reachable" << endl;
            }
        }
    }
    void update_routing_table(vector<Router> &EveryRouter)
    {
        // destination , last hop
        map<int, int> previousRouters;
        // clearing the routing table
        routing_table.clear();

        // Inserting data in the routing table based on the user input
        for (const auto &neighbor : neighbors)
        {
            previousRouters[neighbor.neighbor->router_id] = router_id; // Storing the previous router
            routing_table[neighbor.neighbor->router_id] = make_pair(-1, INT_MAX);
        }
        // inserting the router info in the router table for which user has not given any input
        for (auto &router : EveryRouter)
        {
            if (routing_table.find(router.router_id) == routing_table.end())
            {
                previousRouters[router.router_id] = -1;
                routing_table[router.router_id] = make_pair(-1, INT_MAX);
            }
        }

        set<pair<int, Router *>> priorityQueue; // To store unvisited routers
        //{dist, router}
        priorityQueue.insert({0, this}); // distance from the source node is 0 only

        set<Router *> visited_Routers; // To store the visited routers

        // this is setting the source router distance and next hop
        routing_table[this->router_id] = make_pair(this->router_id, 0);

        // Diasktra's algorithm to find shortest distance
        while (!priorityQueue.empty())
        {
            pair<int, Router *> current = *priorityQueue.begin(); // Pick the unvisited node with the least link cost from the set
            priorityQueue.erase(priorityQueue.begin());
            visited_Routers.insert(current.second);

            for (auto neighbour : current.second->neighbors)
            {
                int new_distance = routing_table[current.second->router_id].second + neighbour.cost;
                // Edge relaxation
                if (new_distance < routing_table[neighbour.neighbor->router_id].second) // updating distance
                {
                    routing_table[neighbour.neighbor->router_id].second = new_distance;
                    previousRouters[neighbour.neighbor->router_id] = current.second->router_id;
                }
                // if the next router is not visited, put them in the priority queue
                if (visited_Routers.find(neighbour.neighbor) == visited_Routers.end())
                    priorityQueue.insert({neighbour.cost, neighbour.neighbor});
            }
        }

        // // checking the previousRouters
        // cout << "Checking previous routers" << endl;
        // for (auto it : previousRouters)
        //     cout << it.first + 1 << " " << it.second + 1 << endl;

        // cout << endl;
    }

    /*
1 2 2
1 3 5
2 3 3
3 4 1
2 4 7

1 2 1
2 3 2
1 5 2
1 4 3
4 3 1
3 5 2
*/
};

int routerInd = 0;
int main()
{
    int routerCount = 0;
    cout << "Enter the number of routers : ";
    cin >> routerCount;

    // vector of routers
    vector<Router> routers;

    for (int i = 0; i < routerCount; i++)
        routers.emplace_back(i);

    int edgeCount = 0;
    cout << "Enter the number of edges in the network : ";
    cin >> edgeCount;

    cout << "Enter the starting and ending router and their distance in between : " << endl;
    for (int i = 0; i < edgeCount; i++)
    {
        int r1, r2, cost;
        cin >> r1 >> r2 >> cost;
        routers[r1 - 1].add_neighbor(&routers[r2 - 1], cost);
        routers[r2 - 1].add_neighbor(&routers[r1 - 1], cost);
    }

    // Update routing tables for all routers
    for (Router &router : routers)
    {
        router.update_routing_table(routers);
    }

    // Printing  the routing tables for all routers
    for (Router &router : routers)
    {
        cout << endl;
        router.print_routing_table();
        cout << endl;
    }

    // To simulate the transfer of packets from one router to another
    while (true)
    {
        int choice;
        cout << "Enter -1 to break or 1 to continue : ";
        cin >> choice;
        if (choice == -1)
            break;
        else if (choice != 1)
        {
            cout << "Wrong input " << endl;
            continue;
        }
        int source, destination;
        cout << "Enter the source to send packet from : ";
        cin >> source;
        cout << "Enter the destination which will receive packet : ";
        cin >> destination;

        for (Router &router : routers)
        {
            // skipping all other routers except source
            if (router.router_id != source - 1)
                continue;

            // cout << "router id: " << router.router_id << endl;
            //  router.findingPaths();
            //  Find shortest path from r1 to r4
            //  vector<int> shortest_path = router.find_shortest_path(1, 4);

            // Output shortest path
            cout << "Packet reached from " << source << " to " << destination << " with cost = ";
            for (auto it : router.routing_table)
            {
                // skipping if destination is not same
                if (it.first != destination - 1)
                    continue;
                // incase destination is not reachable
                if (it.second.second == INT_MAX)
                    cout << "Not reachable";
                else
                    cout << it.second.second;
            }
            cout << endl;
        }
    }
}
