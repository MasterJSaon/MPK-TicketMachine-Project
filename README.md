# MPK Ticket Machine

## Project Building on Debian (C++ with Qt and SQLite)

#### **Quick review**:
- [Requirements: setting up environment step by step](#requirements)
- [Setting up project](#project-setup)
- [Building and Running the Project if ready](#building-and-running-the-project)
- [Future Plans](#future-plans)

## Requirements

Before building the project, make sure you have the following dependencies installed:

- **g++** (GNU Compiler Collection) for compiling C++ code
- **Qt** (for GUI development)
- **SQLite3** (for database management)

### Install Dependencies

Follow the instructions below to install the necessary packages on Debian.

### 1. Install g++ (C++ Compiler)

To install the C++ compiler (`g++`), run the following command:

`sudo apt update`  
`sudo apt install g++`

### 2. Install Qt Development Libraries

For building Qt-based GUI applications, you need to install the Qt development tools. To install Qt5 (you can also use Qt6, but this guide assumes Qt5), run:

`sudo apt install qt5-qmake qtbase5-dev qtchooser qtbase5-dev-tools`

This will install the Qt tools required for compiling Qt-based applications, including `qmake`, which is used to configure Qt projects.

### 3. Install SQLite3 Development Libraries

To work with SQLite in your project, you will need to install the SQLite3 development libraries. Run the following command to install them:

`sudo apt install libsqlite3-dev`

This will install SQLite3 along with the necessary headers and libraries for compiling SQLite-based programs.

## Project Setup

### Clone the Repository

First, clone the project repository to your local machine. Replace `<repository-url>` with the actual URL of your project repository.

`git clone <repository-url>`  
`cd <project-directory>`

### Configure the Build System

Depending on the build system used in the project, you will either use `qmake` (for Qt projects) or `CMake` (for general C++ projects).

#### 1. For Qt Projects (Using qmake)

If the project uses Qt, configure it with `qmake`. In the root directory of the project, run:

`qmake`

This will generate a `Makefile` for your project.

## Building and running the Project

Once the project has been configured, DON'T use `make` to build it.

Just run `./run.sh` to build and run program.

If your `run.sh` file does not have execution permissions, you can add them with the following command:

`chmod +x run.sh`

This will grant the file execution permissions, allowing you to run it directly:

`./run.sh`

## Future plans
- admin ticket management, users etc.
- Website with WASM for users to buy tickets
- logic for user buying ticket
