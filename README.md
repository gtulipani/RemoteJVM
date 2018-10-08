# Remote JVM
## Description
CLI Server-Client Application that simulates a [Java Virtual Machine](https://en
.wikipedia.org/wiki/Java_virtual_machine) processing commands within
 a stack. The server opens a [Socket](https://en.wikipedia.org/wiki/Network_socket) for the communication 
 and the client sends the memory that will be necessary (the server will use 
 *variables array*) and the commands to be executed. 

## Communication Protocol
##### Message Handling
Both Client and Server communicate through a Socket using the [TCP](https://en.wikipedia.org/wiki/Transmission_Control_Protocol)
Protocol. The logic to handle the Sockets is written in [C](https://en.wikipedia.org/wiki/C_(programming_language)).
The protocol has been defined the following way, to facilitate the reception of 
messages:
1. The server opens a socket on the port received as parameter and waits for 
the client to connect
1. The client connects to the host and port received as parameters and 
continues with:start 
sending messages with commands:
   1. The client sends a 4 Bytes [Big Endian](https://en.wikipedia
   .org/wiki/Endianness#Big-endian) 
   (which is the same as [Network Endian](https://en.wikipedia.org/wiki/Endianness#Networking))
   int with the quantity of variables that will be used.
   1. The client continues sending all the [Byte Codes](https://en.wikipedia.org/wiki/Java_bytecode)
   1. The server process each Byte Code and print the operation in **stdout**
   1. The server prints the value of all the stored variables
   1. The server sends all the variables values through the socket
   1. The client receives all the variables and prints them in **stdout**

##### Connection End
When the Client finishes executing all its commands, it [shutdowns](http://man7.org/linux/man-pages/man2/shutdown.2.html)
the socket for Writing.
On the server side, it was waiting for a new message using the 
[`recv()`](http://man7.org/linux/man-pages/man2/recvmsg.2.html) function. 
When the socket is closed, it returns **zero** and the server finishes 
parsing commands.

## Byte Codes
The server application accepts a series of [Byte Codes](https://en.wikipedia.org/wiki/Java_bytecode)
which are represented with an hexadecimal value. All the operations that 
require two elements from the stack, are done in [RPN](https://en.wikipedia.org/wiki/Reverse_Polish_notation), 
which means that the last operand is the first element that was taken out 
from the stack:
#### istore
Reads the following byte code as a **unsigned int**. Uses that value as the 
index in the *variables array*, and pushes it in the *operands stack*. This 
command is represented with the value:
```
0x36
```
#### iload
Reads the following byte code as a **unsigned int**. Uses that value as the 
index in the variables array, and pops it from the *operands stack* into the 
*variables array*. This command is represented with the value:
```
0x15
```
#### bipush
Reads the following byte code, extends it into **signed int** and pushes it 
in the *operands stack*. This command is represented with the value:
```
0x10
```
#### dup
Duplicates the last element from the *operands stack* (without extracting it)
. This command is represented with the value:
```
0x59
```
#### iand
Pops the last two elements from the *operands stack* and perform a bitwise 
[AND](https://en.wikipedia.org/wiki/Bitwise_operation#AND) operation. Pushes 
the result into the stack. This command is represented with the value:
```
0x7E
```
#### ixor
Pops the last two elements from the *operands stack* and perform a bitwise 
[XOR](https://en.wikipedia.org/wiki/Bitwise_operation#XOR) operation. Pushes 
the result into the stack. This command is represented with the value:
```
0x82
```
#### ior
Pops the last two elements from the *operands stack* and perform a bitwise 
[OR](https://en.wikipedia.org/wiki/Bitwise_operation#OR) operation. Pushes 
the result into the stack. This command is represented with the value:
```
0x80
```
#### irem
Pops the last two elements from the *operands stack*, performs the division 
between them and gets the remainder value. Pushes the result into the stack. 
This command is represented with the value:
```
0x70
```
#### ineg
Pops the last element from the *operands stack* and perform a NOT operation 
but by using the [Two' Complement](https://en.wikipedia.org/wiki/Two%27s_complement)
operation. Pushes the result into the stack. This command is represented with
 the value:
```
0x74
```
#### idiv
Pops the last two elements from the *operands stack* and perform the division 
between them. Pushes the result (*casted to int*) into the stack. This 
command is represented with the value:
```
0x6C
```
#### iadd
Pops the last two elements from the *operands stack* and perform the 
**Addition** operation. Pushes the result into the stack. This command is 
represented with the value:
```
0x60
```
#### imul
Pops the last two elements from the *operands stack* and perform the 
**Multiplication** operation. Pushes the result into the stack. This command 
is represented with the value:
```
0x68
```
#### isub
Pops the last two elements from the *operands stack*, performs the 
**Subtraction** between them and gets the result. Pushes the result into the 
stack. This command is represented with the value:
```
0x64
```
### Commands Extension
There are more [Byte Codes](https://en.wikipedia.org/wiki/Java_bytecode_instruction_listings)
actually supported by the JVM. These can be easily added by including a new 
value in the `jvm_byte_code` enum from **jvm_utils.h**. Each enum is mapped 
to a `operation_result` (function pointer that must be implemented) in the 
`jvm_argument_detect()` function.

## Building and Running
### Build
1. Navigate to the `src` folder
1. Execute `make -f Makefile`

### Run
#### Server
The server must be executed with the following syntax:
```
./remoteJVM server ​<port>
```
##### Standard Out
The server will print the following in **stdout**:
- Each one of the executed byte codes:
```
Bytecode trace
[byte_code_1]
[byte_code_2]
...
```
- The final status of the **variables array**, each one of them as 
**hexadecimal** value with 8 digits:
```
Variables dump
[array_variable_1]
[array_variable_2]
...
```
#### Client
The client must be executed with the following syntax:
```
./remoteJVM client ​<host> <port>​ ​<N> ​[​<filename>​]
```
##### Standard In
The filename is **optional**. If no file is specified, **stdin** is taken as 
source of byte codes.
##### Standard Out
The client will print the following in **stdout** after the whole execution 
finishes:
```
Variables dump
[array_variable_1]
[array_variable_2]
...
```

### Examples
##### Arithmetic Operations
  - Start the server with the following command:
```
./remoteJVM server 8080
```
  - Start the client with the following command: 
```
./remoteJVM client localhost 8080 6 < 01_bytecodes.bin
```
  - The server displays the **Bytecode trace** and the variables output:
```
Bytecode trace
bipush
dup
dup
dup
dup
dup
bipush
iadd
istore
bipush
isub
istore
bipush
imul
istore
bipush
idiv
istore
bipush
irem
istore
ineg
istore

Variables dump
00000006
00000002
00000008
00000002
00000000
fffffffc
```
  - The client displays the final variables and finishes:
``` 
Variables dump
00000006
00000002
00000008
00000002
00000000
fffffffc
```
##### Bitwise Operations
  - Start the server with the following command:
```
./remoteJVM server 8080
```
  - Start the client with the following command: 
```
./remoteJVM client localhost 8080 6 < 02_bytecodes.bin
```
  - The server displays the **Bytecode trace** and the variables output:
```
Bytecode trace
iload
bipush
isub
dup
istore
bipush
imul
bipush
imul
istore
iload
iload
iand
istore
iload
iload
ixor
istore
iload
iload
ior
istore

Variables dump
ffffffff
ffffff00
ffffff00
000000ff
ffffffff
00000000
```
  - The client displays the final variables and finishes:
```
Variables dump
ffffffff
ffffff00
ffffff00
000000ff
ffffffff
00000000
```
##### Istore
  - Start the server with the following command:
```
./remoteJVM server 8080
```
  - Start the client with the following command: 
```
./remoteJVM client localhost 8080 4 < 03_bytecodes.bin
```
  - The server displays the **Bytecode trace** and the variables output:
```
Bytecode trace
bipush
bipush
bipush
bipush
istore
istore
istore
istore

Variables dump
00000041
00000042
00000043
00000044
```
  - The client displays the final variables and finishes:
```
Variables dump
00000041
00000042
00000043
00000044
```

### Analyze Code Style
1. Navigate to the `src` folder
1. Execute the following command that uses the cpplint script provided by 
Google and the filters, both already included in the repo
```
python ./cpplint.py --extensions=h,hpp,c,cpp --filter=`cat filter_options` `find -regextype posix-egrep -regex '.*\.(h|hpp|c|cpp)'`
```
1. The script will start analyzing each file. Example output:
```
Done processing ./jvm_server.c
Done processing ./jvm_utils.h
Done processing ./stack.c
Done processing ./int_vector.h
Done processing ./jvm_client.h
Done processing ./result.h
Done processing ./socket.c
Done processing ./main.c
Done processing ./int_vector.c
Done processing ./stack.h
Done processing ./jvm_utils.c
Done processing ./jvm_server.h
Done processing ./jvm_client.c
Done processing ./socket.h
Total errors found: 0
```

### Clean
1. Navigate to the `src` folder
1. Execute `make -f Makefile clean`. This will remove both executable and all 
the **.o** files.
