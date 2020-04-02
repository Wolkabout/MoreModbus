# MoreModbus

This is an abstraction layer over the `libmodbus` library. The functionality added allow creation
of mappings that create more meaningful data from modbus registers, such as 32 bit types created from
two registers, strings, and single bits from registers.

This library contains `Mappings`, the base building block. `Mapping` implies a single, logical bit of data.
They are of different types different from the native modbus `uint16_t`, like:
- INT16, UINT16 (where UINT16 is native for registers)
- INT32, UINT32, FLOAT
- STRING
- BOOL (where BOOL is native for discrete types)

These are built using operations 

|      TYPE     |               OPERATIONS              |
|:-------------:|:-------------------------------------:|
| INT32, UINT32 | MERGE_BIG_ENDIAN, MERGE_LITTLE_ENDIAN |
|     FLOAT     |              MERGE_FLOAT              |
|     STRING    |   STRINGIFY_ASCII, STRINGIFY_UNICODE  |
|      BOOL     |                TAKE_BIT               |

- STRINGIFY_ASCII (0-127) and STRINGIFY_UNICODE (0-255) both store two characters as a 
single `uint16_t` (understandable to modbus)
- TAKE_BIT in combination with an index takes an exact bit (0-15) of a 16-bit register

You can create a `Device` that groups `Mappings`. A `Group` contains `Mappings` that can be read with a single
modbus message, and then the data is aggregated to each `Mapping`, and parsed to requested type.

Also, there exists a `Reader` that takes in all devices. This part is multi-threaded, all devices get their own thread
for reading the groups and parsing the data.

## Compiling

This library requires:
- GNU GCC and G++
- CMake
- Libmodbus (this is downloaded by the script)

So run
```shell script
sudo apt install gcc g++ cmake
```

And after that, run the configuration script
```shell script
./configure.sh
```

The out directory should be generated, so move in there, and run build
```shell script
cd out
make
```

After that, you can run the example, and run the tests
```shell script
./moreModbusExample
./moreModbusTests (tests still missing, coming soon)
```
