# MoOS

A hobby operating system for x86 architecture, built from scratch in C++.

```
   ╔═══╗  ╔═══╗
   ║ ♦ ║  ║ ♦ ║   MoOS v1.0
   ╚═╦═╝  ╚═╦═╝   Modern Operating System
   ╔═╩════╩═╗
   ║ ░░░░░░ ║
   ╚════════╝
```

## Features

- **Shell** - Command-line interface with history, inline editing, and tab completion
- **Text Editor** - Nano-like editor with file save/create support
- **FAT32 Filesystem** - Read and write files to disk
- **Keyboard & Mouse** - Full input device support
- **VGA Text Mode** - 80x25 color console with Unicode (CP437) support

### Shell Commands

| Command | Description |
|---------|-------------|
| `help` | Show available commands |
| `clear` | Clear the screen |
| `ls` | List files on disk |
| `cat <file>` | Display file contents |
| `nano <file>` | Edit or create a file |
| `echo <text>` | Print text to console |
| `pwd` | Print working directory |
| `uname` | Show kernel version |
| `uptime` | Show system uptime |
| `free` | Show memory usage |
| `lspci` | List PCI devices |
| `reboot` | Reboot the system |
| `halt` | Shutdown (exits QEMU) |

### Editor (nano)

- **Arrow keys** - Navigate
- **Type** - Insert text
- **Backspace/Delete** - Delete characters
- **Shift+O** - Save file
- **Esc** - Exit editor

## Building

### Prerequisites

Docker is required for building. One-time setup:

```bash
make build-docker-image
```

### Build & Run

```bash
# Build the kernel ISO
make docker

# Run in QEMU (terminal mode)
make qemu-run-terminal

# Run in QEMU (GUI mode)
make qemu
```

### Quick Start

```bash
make build-docker-image  # First time only
make qemu-run-terminal   # Build and run
```

## Project Structure

```
moos/
├── src/
│   ├── kernel.cpp          # Kernel entry point
│   ├── core/               # GDT, memory management, syscalls
│   ├── common/             # Console, strings, utilities
│   ├── drivers/            # Keyboard, mouse, ATA disk
│   ├── file_system/        # FAT32 implementation
│   ├── gui/                # Terminal, editor
│   └── hardware/           # Interrupts, PCI, ports
├── headers/                # Header files
├── barebones/              # Minimal "hello world" OS example
├── Makefile
└── Dockerfile
```

## Barebones Example

For the most basic "hello world" OS implementation, see the [barebones folder](barebones/).

## Useful Commands

### Creating a FAT32 disk image with mtools

```bash
# Create 50MB disk image
dd if=/dev/zero of=disk.img bs=1M count=50

# Format with FAT32
mformat -i disk.img -F ::

# Copy a file to the disk
mcopy -i disk.img example.txt ::

# List files
mdir -i disk.img ::
```

### Installing QEMU

```bash
# Ubuntu/Debian
sudo apt install qemu-system-x86 -y

# With full virtualization support
sudo apt install qemu qemu-kvm libvirt-daemon-system libvirt-clients bridge-utils virt-manager -y
```

## License

This project is just for fun and learning. See [LICENSE](LICENSE) for details.
