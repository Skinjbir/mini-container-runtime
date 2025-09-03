# 🚀 Container Runtime in C++

This project is a lightweight container runtime written in C++ that leverages Linux namespaces, clone(), chroot(), and mount() to provide process, UTS, and filesystem isolation. It allows running commands inside a minimal root filesystem, such as Alpine Linux, with a custom hostname and isolated mount points. Designed as a learning and experimental runtime, it demonstrates the fundamentals of containerization without relying on Docker or other heavy container platforms.
> Note: This is the first version. Currently, it only uses Linux namespaces for isolation.

## Features

Process, UTS, and mount namespaces  
Custom hostname inside container  
Rootfs isolation with `chroot`  
Runs `/bin/sh` inside an Alpine rootfs

## Root Filesystem (rootfs)

