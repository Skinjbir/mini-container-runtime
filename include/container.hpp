#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include <string>
#include <sys/types.h>

// Container class encapsulates a lightweight container process
class Container {
private:
    const char* rootfs;      // Path to root filesystem for container
    char* stack;             // Stack memory for child process
    pid_t pid;               // PID of the container process
    std::string hostname;    // Hostname inside the container
    char* const* command;    // Command to execute in the container

    static int childExecutes(void* arg); // Function executed in child process
    static void cleanupProc();           // Cleanup resources after process ends

public:
    // Constructor initializes container with rootfs, command, and hostname
    Container(const char* rootfsPath, char* const cmd[], std::string host = "MyContainer");
    ~Container();

    pid_t run(bool waitForExit = true); // Starts the container process
    void waitForChild();                // Waits for the child process to exit

    pid_t getPid() const;               // Returns container process PID
    std::string getHostname() const;    // Returns container hostname
};

#endif
