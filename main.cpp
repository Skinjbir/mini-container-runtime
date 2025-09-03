#include "container.hpp"
#include <iostream>

int main() {
    const char* rootfs = "/home/mohammedreda/Desktop/containerRunTIme/alpine";

    char* const cmd[] = { (char*)"/bin/sh", nullptr };

    Container c(rootfs, cmd, "MyAlpine");
    c.run();

    std::cout << "[Parent] Container PID: " << c.getPid() << std::endl;
    std::cout << "[Parent] Container Hostname: " << c.getHostname() << std::endl;

    return 0;
}
