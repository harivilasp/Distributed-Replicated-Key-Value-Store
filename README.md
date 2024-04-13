# Distributed-Multithreaded-Order-Management

## TLDR
- Developed a client-server system using TCP sockets to facilitate seamless communication between customers and the factory, enabling efficient order placement and delivery tracking.
- Implemented client stubs to abstract away low-level network details, allowing customers to place orders effortlessly while ensuring secure and reliable data transmission.
- Designed server stubs to handle incoming orders, manage laptop assembly processes, and ship customized laptops, optimizing factory operations and enhancing productivity.
- Integrated multi-threading and synchronization primitives to support concurrent order processing, enabling the factory to efficiently handle a large volume of customer requests.
- Introduced customization options for customers, allowing them to request custom modules for their laptops and seamlessly coordinating the assembly process between regular and expert engineers.
- Implemented performance metrics tracking to monitor order latency and throughput, enabling continuous improvement of system efficiency and customer satisfaction.

## Overview

This project implements a client-server system that simulates a laptop factory's order-and-delivery process. Customers (clients) can place orders for laptops, which the factory (server) assembles and ships back to the customers. The system uses TCP sockets for communication, abstracted through simple RPC-like stubs. This project was developed as part of the CS 7610 Programming Assignment 1 at Northeastern University.

## Features

- **TCP Socket Communication:** Establishes client-server connections using TCP sockets.
- **RPC-like Abstractions:** Utilizes client and server stubs to abstract the details of network communication.
- **Multi-threaded:** Supports multi-threaded client and server programs for handling multiple orders concurrently.
- **Performance Metrics:** Measures and reports latency and throughput for the factory's order processing.

## Getting Started

### Prerequisites

- Linux environment (Tested on Khoury College Linux cluster)
- C++ compiler supporting C++11 standards
- Make (for building the project with the provided Makefile)

### Building the Project

1. Clone the repository to your local machine.
2. Navigate to the project directory.
3. Run the `make` command to build the `client` and `server` binaries.

``` make all ```
Running the Server
To start the server, use the following command, replacing [port #] with your desired port number:


 ```./server [port #] ```
For handling custom laptop orders, add the number of expert engineers available as a second argument:

```./server [port #] [# experts]```
Running the Client
To run the client, use the following command with the appropriate arguments:

``` ./client [ip addr] [port #] [# customers] [# orders] [laptop type] ```

ip addr: IP address of the server

port : Port number of the server

- customers: Number of customer threads to simulate
- orders: Number of orders each customer will place
- laptop type: Type of laptop (0 for regular, 1 for custom)

## System Design

### Client-Server Communication
The system uses ClientStub and ServerStub classes for communication. Orders and laptop information are serialized into byte streams for transmission over the network.

### Orders and Laptop Information
Orders include customer ID, order number, and laptop type.
Laptop information includes customer ID, order number, laptop type, engineer ID, and expert ID.
### Multi-threading and Synchronization
The server assigns an engineer (thread) to each customer connection.
Expert engineers are pooled and handle custom laptop requests concurrently.
### Performance Measurement
The client measures latency (time taken for an order to be processed) and throughput (orders processed per second). These metrics are reported at the end of the client program's execution.

### Contributing
Feel free to fork the repository and submit pull requests. Please open issues for any bugs or feature requests.

### License
This project is licensed under the MIT License - see the LICENSE file for details.
