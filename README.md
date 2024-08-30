# OSPF Routing Protocol Implementation in C/C++

## Overview

This project is an implementation of the **OSPF (Open Shortest Path First)** routing protocol in **C/C++**. The primary goal is to build a routing table for a network of routers and simulate the routing of packets between them. This implementation involves the use of **Dijkstra's algorithm** (or a similar approach) to compute the shortest path to each router in the network.

## Features

- **Routing Table Construction:** Efficiently builds a routing table for a given network topology.
- **Shortest Path Calculation:** Implements the OSPF algorithm within the `update_routing_table()` method to compute the shortest path to each router using Dijkstra's algorithm or a similar approach.
- **Packet Routing Simulation:** Simulates the routing of packets between routers based on the computed shortest paths.
- **Network Topology Support:** Handles various network topologies, allowing flexible testing and simulation.
