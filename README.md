#  IOKit-Interactions

Interacting with kernel-level objects or performing privileged operations on macOS. (WIP)

#  Why

On macOS, system frameworks or particular APIs offered by Apple are usually used to communicate with kernel objects. The above example makes the assumption that device nodes (/dev/...) are utilized for direct kernel interaction in an environment akin to Linux. On macOS, though, this method needs to be modified. You would normally use Apple-provided APIs like IOKit or Kernel.framework to interface with kernel-level objects or to carry out privileged actions. This is an IOKit conceptual example.

#  Info

What is its link with the kernel extension MyKernelService? To identify the service, it first compares the supplied service name to IOServiceMatching. Following service discovery, the script calls IOServiceOpen to create a communication channel between the user-space program and the kernel extension. The script can alter kernel data objects over this connection by transmitting payloads to it via the IOConnectCallStructMethod. The script addresses issues that emerge during service discovery and connection, and it reports useful information for debugging. IOKit's smooth integration with the kernel extension allows users to take advantage of its features and perform complicated operations on kernel data objects.

Make sure your application has the necessary permissions to interact with the provided IOKit service before implementing the actual IOKit calls and methods. This needs knowledge of the particular service and methods supplied by your kernel object. You can successfully replicate kernel data manipulation and comprehend the required procedures for mitigation or debugging by modifying your technique to use IOKit or other macOS-specific APIs for kernel interaction. Depending on what has to be done, testing on macOS may require sudo access or other necessary permissions.

#  Features Implemented (WIP):

- Logging System
- Payload Management
- Secure Memory Handling (frees memory by zeroing out the data)
- Signal Handling (catch those seg faults)
- Kernel Data Manipulation
- Dynamic detection/connection to IOKit services 
- A user interface

#  Some documentation is available [here](https://github.com/rsprbp/iokit-interactions/blob/main/DOCS)
