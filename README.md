# **(System Tray Windows) Compilation with Custom Icon**

## **Overview**

This guide explains how to compile a C program into an executable (`.exe`) with a custom icon.

## **Prerequisites**

Ensure you have the following files in your project directory:

- **`systemTray.c`**: Your main C source file.
- **`bangla_keyboard.ico`**: Your custom icon file.
- **`resource.h`**: Resource header file.
- **`resources.rc`**: Resource script file.

## **Compilation Steps**

### 1. Compile the Resource File

Run the following command:

```bash
windres resources.rc -o resource.o
```
## Expected Result:

- **`resource.o`**: that contains machine code.

### 2. Generated Executable File (.exe)
```bash
gcc systemTray.c resource.o -o systemTray.exe -mwindows
```
OR
```bash
gcc systemTray.c resource.o -o systemTray.exe -lgdi32 -lcomctl32 -lshell32
```
OR
```bash
gcc systemTray.c resource.o -o systemTray.exe -mwindows -lgdi32 -lcomctl32 -lshell32
```
#### Behavior of Flags
> **`-mwindows:`** Tells GCC to create a Windows GUI application. This means that the application will not have a console window attached to it when run.

> **`-lcomctl32::`** Links the Common Controls library, which provides support for common Windows UI elements (like toolbars and status bars).

> **`-lshell32::`** Links the Shell library, which provides access to Windows shell functions.

> **`-lgdi32:`** Links the GDI (Graphics Device Interface) library, which is used for graphical output.


## Expected Result:

- **`systemTray.exe`**: Executable File (.exe)

### 3. Run Executable File (.exe)

```bash
systemTray.exe
```
OR
```bash
./systemTray.exe
```
