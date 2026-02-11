# Security Policy

## Overview

The **Mythology Quiz - Modern Edition** is a locally running C++ application that uses **OpenGL**, **GLFW**, and **Dear ImGui**. This document outlines security considerations and guidelines for responsibly reporting issues.

---

## Supported Platforms

* **Linux** (Debian, Ubuntu, Fedora, Arch)
* **macOS**
* **Windows** (manual setup required)

> On Linux/macOS, `setup.sh` automatically installs dependencies. Users should review scripts before execution.

---

## Security Considerations

### 1. Local Script Execution

* `setup.sh` and `launch.sh` are **shell scripts** executed locally.
* Users should **inspect scripts** before running to ensure no malicious commands are present.
* Scripts require elevated permissions (`sudo`) on Linux/macOS to install dependencies.

### 2. Dependency Management

* The project uses external libraries (GLFW, Dear ImGui).
* Libraries are **cloned from official repositories**.
* Users should **verify dependencies** when possible.

### 3. Input Validation

* The application **does not accept network input**; all quiz interaction is local.
* User input is limited to:

  * Selecting number of questions
  * Choosing answers from multiple-choice options

### 4. File System Access

* The quiz may create temporary files in the **Build/** folder during compilation.
* Users should ensure the **Build/** folder is secure and not shared with untrusted sources.

### 5. Cross-Platform Considerations

* Windows setup requires **manual installation** of dependencies; users should follow official library sources.
* Linux/macOS setup scripts should be run from trusted directories to avoid executing malicious scripts.

---

## Reporting Security Issues

If you discover a security vulnerability:

* Open an **issue or pull request** on the GitHub repository:
* Include:
  * A description of the issue
  * System information (OS, compiler version)

Security issues will be addressed in updates as quickly as possible.

---

## Recommended Security Practices for Users

* Review scripts (`setup.sh`, `launch.sh`) before running.
* Keep your system up-to-date with security patches.
* Run the quiz on a non-root account whenever possible.
* Windows users should verify official sources of dependencies.
* Avoid downloading pre-compiled binaries from untrusted sources.

---
