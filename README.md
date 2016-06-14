# xeon_phi_kernel_integration (KNIGHTS CORNER)

All of this code was written during my computer science mather thesis.
It benchmarks the Intel Xeon Phi encryption (AES) performance, latency, memory throughput and pcie throughput.
These benchmarks are also implemented to use memory allocated by the linux kernel.

Usually it works like the following: The host programm allocates memory and makes it accessible by the Xeon Phi.
The Xeon Phi reads this memory into its own performs some operations (encryption, decryption, etc) and writes it back into the hosts memory.

