# "Five Philosophers" Project

## Project Description

The "Five Philosophers" project is an implementation of the classical dining philosophers problem, where five philosophers try to use shared forks in a conflict-free manner. This is a naive solution, it may lead to deadlock when each
of philosophers will effectively lift his left fork. Then each of them will wait for his right one fork infinitely long. The project utilizes various concurrency mechanisms, such as semaphores, message queues, and shared memory, in the C programming language.

## Repository Structure

- `filozof.c`: This file contains the implementation of the life cycle of each philosopher, including thinking, waiting for fork access, eating, and releasing forks, using concurrency mechanisms.
- `mainp.c`: The main project file manages the initialization and coordination of concurrency mechanisms, launches philosopher processes, and handles signals for controlled program termination and resource release.

## Requirements

The project is written in C and has been developed and tested primarily on the Ubuntu Linux operating system. While it may work on other Unix-like operating systems, I recommend using a Linux-based environment, such as Ubuntu, for compiling and running the project. 

## Running the Project

1. Clone the repository to your local computer.
2. Compile the project, e.g., using the command `gcc filozof.c -o filozof` and `gcc mainp.c -o mainp`.
3. Run the project using the command `./mainp`.

## Author

This project was created by Dominika Hojniak as part of learning concurrent programming in C and Operating Systems.
