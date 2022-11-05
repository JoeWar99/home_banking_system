# Home Banking System :house::moneybag:
Client / server application that simulates banking operations over the Internet.

The simulation is performed using processes in the same machine, with the communication between clients and server made through named pipes (FIFOs).

#### Final grade: 19.8


### Description
The application is composed by a 'user' program, that simulates a user of the home banking system, and a 'server' program, that processes the users' requests.

The service is **authenticated** and allows for the following operations:
* account creation
* balance check
* money transfer
* server shutdown

A high level of **concurrency** is assured with synchronization methods, according to the producer-consumer problem.

The server receives requests from a common FIFO, and each user receives their response from a dedicated FIFO.

The server is composed by a collection of "balconies", simulated using **threads**, that process, by order of arrival, the users' requests.

**Full project specifications and details can be seen in "project.pdf".**

### Technologies Used
* C

### Group Members
* [José Guerra](https://github.com/joewar99 "joewar99")
* [Manuel Coutinho](https://github.com/ManelCoutinho "ManelCoutinho")
* [Mário Gil](https://github.com/GambuzX "GambuzX")

### Curricular Unit Info
* **Date:** 2nd Year, 2nd Semester, 2018/2019
* **Course:** [Sistemas Operativos | Operating Systems](https://sigarra.up.pt/feup/en/UCURR_GERAL.FICHA_UC_VIEW?pv_ocorrencia_id=419998 "SOPE") (SOPE)

### Disclaimer 
This repository was used for educational purposes and we do not take any responsibility for anything related to its content. You are free to use any code or algorithm you find, but do so at your own risk.
