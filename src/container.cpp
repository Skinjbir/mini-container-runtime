#include "container.hpp"
#include <iostream>
#include <unistd.h>
#include <sched.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>

#define STACK_SIZE (1024 * 1024)

int Container::childExecutes(void* arg) {
    Container* self = static_cast<Container*>(arg);

    if (mount(nullptr, "/", nullptr, MS_REC | MS_PRIVATE, nullptr) != 0) {
        perror("[Child] Failed to make mounts private");
        exit(1);
    }

    if (mount("proc", "/proc", "proc", MS_NOEXEC | MS_NOSUID | MS_NODEV, "") != 0) {
        perror("[Child] Mount /proc failed");
        exit(1);
    }

    if (chroot(self->rootfs) != 0 || chdir("/") != 0) {
        perror("[Child] chroot/chdir failed");
        umount2("/proc", MNT_DETACH);
        exit(1);
    }

    if (sethostname(self->hostname.c_str(), self->hostname.size()) != 0) {
        perror("[Child] sethostname failed");
        umount2("/proc", MNT_DETACH);
        exit(1);
    }

    pid_t wrapper = fork();
    if (wrapper == 0) {
        execv(self->command[0], self->command);
        perror("[Child] execv failed");
        exit(1);
    } else if (wrapper > 0) {
        int status;
        waitpid(wrapper, &status, 0);
        umount2("/proc", MNT_DETACH);

        if (WIFEXITED(status)) return WEXITSTATUS(status);
        if (WIFSIGNALED(status)) return 128 + WTERMSIG(status);
        return 1;
    } else {
        perror("[Child] fork failed");
        umount2("/proc", MNT_DETACH);
        exit(1);
    }
}

Container::Container(const char* rootfsPath, char* const cmd[], std::string host)
    : rootfs(rootfsPath), stack(nullptr), pid(-1), hostname(std::move(host)), command(cmd) {}

Container::~Container() {
    if (stack) delete[] stack;
}

pid_t Container::run(bool waitForExit) {
    stack = new char[STACK_SIZE];
    int flags = CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | SIGCHLD;
    pid = clone(childExecutes, stack + STACK_SIZE, flags, this);
    if (pid == -1) { perror("clone failed"); exit(1); }

    if (waitForExit) waitForChild();
    return pid;
}

void Container::waitForChild() {
    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
    } else if (WIFEXITED(status)) {
        std::cout << "[Parent] " << hostname << " exited with code "
                  << WEXITSTATUS(status) << std::endl;
    } else if (WIFSIGNALED(status)) {
        std::cout << "[Parent] " << hostname << " killed by signal "
                  << WTERMSIG(status) << std::endl;
    }
}

pid_t Container::getPid() const { return pid; }
std::string Container::getHostname() const { return hostname; }
