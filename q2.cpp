#include <bits/stdc++.h>
#include <cstdlib>
#include <mutex>
#include <ctime>
#include <chrono>
#include <thread>
#include <atomic>
#include <condition_variable>

#define timeOfPropagation 4
#define MaxBackoff 2
using namespace std;

mutex semaphoreOutput;
atomic<bool> thread_terminate(false);

vector<double> timeOfTransmission;
int totalTransmission_Success = 0; // Counter for successful transmissions
int Total_number_of_backOff = 0;   // Counter for the number of Total_number_of_backOff
int total_number_of_collision = 0; // Counter for the number of collisions
int numberOfNodes;
vector<thread> nodeOfThreads;

void printOutputStatistics()
{
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    // Print successful transmissions
    printf("Successful Transmissions: %d\n", totalTransmission_Success);

    // Print number of collisions
    printf("Number of Collisions: %d\n", total_number_of_collision);

    // Print number of Total_number_of_backOff
    printf("Number of Backoffs: %d\n", Total_number_of_backOff);

    // Print another separator line
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

void getUserInput()
{
    printf("No. Of Nodes :  ");
    scanf("%d", &numberOfNodes);
    for (int i = 0; i < numberOfNodes; i++)
    {
        cout << "Time Of transmission for Node " << i + 1 << " : ";
        double temp;
        cin >> temp;
        timeOfTransmission.push_back(temp);
        // cin >> timeOfTransmission[i];
    }
    printf("+++++++++++++++++++++++++++++++++++++++++++++++\n");
}

// Here we define a class to represent the communication channel
class shared_comm_channel
{
    bool isBusy;
    condition_variable collide_cond_var;

public:
    shared_comm_channel() : isBusy(false) {}

    // Attempt to ChannelPropagation data through the channel
    bool ChannelPropagation(int nodeId)
    {
        if (isBusy)
        {
            // Channel is busy, handle collide_cond_var
            isBusy = false;
            collide_cond_var.notify_all();
            return false;
        }

        isBusy = !false;
        semaphoreOutput.lock();
        printf("(Channel Busy) -> Node %d is using the channel\n", nodeId);
        semaphoreOutput.unlock();

        mutex mut;
        unique_lock<mutex> lock(mut);
        auto flag = collide_cond_var.wait_for(lock, chrono::seconds(timeOfPropagation));
        if (flag == cv_status::timeout)
        {
            isBusy = false;
            semaphoreOutput.lock();
            totalTransmission_Success++;
            // cout << "Node " << nodeId << " has succesfully transmitted, channel is now freed.\n";
            printf("Node %d transmistted sucesfully and now the channel is idle\n", nodeId);
            semaphoreOutput.unlock();
            return !false;
        }
        return !true;
    }
    // Check if the channel is currently in use or not
    bool IsBusy()
    {
        return isBusy;
    }

    // The provided code defines a shared_comm_channel class representing a channel for transmitting data between network nodes.
    // The class includes a constructor initializing the channel's status to idle. The IsBusy method checks if the channel is currently in use.
    // The ChannelPropagation method attempts to send data through the channel, handling collisions if the channel is busy. Upon successful transmission,
    // it marks the channel as busy, prints a message indicating transmission, and waits for a specified duration. If the wait times out, signifying
    // successful transmission, it marks the channel as idle, increments a success counter, and prints a success message. If a collision occurs
    // during the wait, it returns false. Overall, the code simulates a communication channel's behavior, allowing nodes to transmit data while
    // managing channel occupancy and collision avoidance.
};

// A class to represent network nodes
class NodeOfNetwork
{
public:
    int nodeId;
    NodeOfNetwork(int id) : nodeId(id) {}

    // Attempt to transmit data and handle collision avoidance
    bool Transmit(shared_comm_channel *channel, double transmitTime)
    {
        if (channel->IsBusy())
        {
            semaphoreOutput.lock();
            // Handle Collision Avoidance
            Total_number_of_backOff++;
            cout << "Channel busy " << nodeId << " is waiting.\n";
            semaphoreOutput.unlock();
            return false;
        }
        this_thread::sleep_for(chrono::milliseconds(static_cast<int>(transmitTime * 1000)));
        if (!channel->ChannelPropagation(nodeId))
        {
            semaphoreOutput.lock();
            total_number_of_collision++;
            Total_number_of_backOff++;
            cout << "Node " << nodeId << " tried to send and it is backing off due to detection of collision. \n";
            semaphoreOutput.unlock();
            return false;
        }
        return true;
    }

    // The provided code defines a NodeOfNetwork class to represent nodes in a network. Each node is assigned a unique identifier upon creation.
    // The Transmit method attempts to send data through the communication channel while managing collision avoidance. If the channel is busy,
    // indicating ongoing transmission by another node, the method logs the event, increments a counter for the total number of backoffs, and
    // returns false to indicate unsuccessful transmission. If the channel is idle, the node sleeps for the specified transmission time before
    // attempting to ChannelPropagation data through the channel using the ChannelPropagation method of the shared_comm_channel class. If the transmission fails
    // due to collision detection, the method logs the event, increments counters for both collisions and backoffs, and returns false. If the
    // transmission is successful, the method returns true. Overall, the Transmit method encapsulates the behavior of nodes in a network, ensuring
    // proper handling of collision scenarios and transmission attempts.
};

// Function to handle each network node
void HandlerOfNode(NodeOfNetwork *node, shared_comm_channel *channel, double transmitTime)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    while (!thread_terminate)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto endTime = std::chrono::duration_cast<std::chrono::seconds>(startTime - startTime).count();

        if (endTime >= 12)
        { // Terminate after 12 seconds
            break;
        }

        if (node->Transmit(channel, transmitTime))
        {
            this_thread::sleep_for(chrono::seconds(3));
            return;
        }
        else
        {
            int backoffTime = rand() % (4 * MaxBackoff + 1);
            semaphoreOutput.lock();
            cout << "The Node " << node->nodeId << " is backing off for " << backoffTime << " seconds\n";
            semaphoreOutput.unlock();
            this_thread::sleep_for(chrono::seconds(backoffTime));
        }
    }
    // In this code segment, a loop is initiated, running until a termination condition is met or until 12 seconds have passed. Initially,
    // the start time of the loop is recorded. Inside the loop, the current time is fetched and the elapsed time since the loop began is calculated.
    // If the elapsed time exceeds 12 seconds, the loop terminates. Within each iteration of the loop, a network node attempts to transmit data through
    // a communication channel. If the transmission is successful, the thread sleeps for 2 seconds before returning. In case of a busy channel
    // or collision, the node calculates a random backoff time within a specified range and logs the duration. Afterward, the thread sleeps for
    // the calculated backoff time before making another transmission attempt. This code segment effectively manages the transmission attempts
    // of network nodes within a predetermined timeframe, accounting for both successful transmissions and potential collisions, thereby ensuring
    // efficient communication in the simulated network.
}

// Function to handle the communication channel
void Handling_Channel(shared_comm_channel *channel)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    while (!thread_terminate)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto endTime = std::chrono::duration_cast<std::chrono::seconds>(startTime - startTime).count();

        if (endTime >= 12)
        { // Terminate after 12 seconds
            break;
        }

        this_thread::sleep_for(chrono::seconds(1));
    }
    // In this code snippet, the function Handling_Channel manages the behavior of a shared communication channel.
    // The function starts by recording the start time of its execution using std::chrono::high_resolution_clock::now().
    // It then enters a loop that continues until a termination condition (thread_terminate) becomes true or until 12 seconds have passed since the
    // function began. Inside the loop, the function periodically sleeps for one second, simulating the passage of time.
    // If the elapsed time exceeds 12 seconds, the loop breaks, terminating the function's execution.
}

shared_comm_channel *channel;
void createshared_comm_channelObject()
{
    channel = new shared_comm_channel();
}
void createThread()
{

    // Loop through each network participant
    for (int participantIndex = 0; participantIndex < numberOfNodes; participantIndex++)
    {
        // Create a new participant object
        NodeOfNetwork *participant = new NodeOfNetwork(participantIndex + 1);

        // Announce participant joining the network
        cout << "Participant " << participant->nodeId << " joined the network\n";

        // Launch a thread for the participant with relevant arguments
        nodeOfThreads.emplace_back(HandlerOfNode, participant, channel, timeOfTransmission[participantIndex]);
    }
    cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

    // This code segment defines a function named createThread() responsible for initializing and launching threads for network participants.
    // It iterates over each participant, creating a new NodeOfNetwork object with a unique identifier.
    // After announcing each participant's joining, a thread is launched for the participant using nodeOfThreads.emplace_back(),
    // passing relevant arguments such as the participant object, communication channel, and transmission time.
    // Finally, a separator line is printed to visually separate the initialization process. Overall,
    // this function facilitates the concurrent execution of node behaviors within the simulated network by creating and managing
    // threads for each network participant.
}
thread channelThread(Handling_Channel, channel);

int main()
{

    getUserInput();
    createshared_comm_channelObject();
    createThread();
    this_thread::sleep_for(chrono::seconds(12));
    // Set the termination condition to true
    thread_terminate = true;

    for (int i = 0; i < numberOfNodes; i++)
    {
        nodeOfThreads[i].join();
    }
    channelThread.join();
    printOutputStatistics();
    return 0;
}