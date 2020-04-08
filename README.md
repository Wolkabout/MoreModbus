#MoreModbus

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
- Libmodbus (this is downloaded by CMake automatically)

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

## Usage

Create some mappings, you can create some normal mappings

#### Default Mapping

All the HOLDING_REGISTER/INPUT_REGISTER mappings by default are UInt16Mappings,
and INPUT_CONTACT/COIL are BoolMapping.

```c++
const auto& normalRegisterMapping =
  std::make_shared<wolkabout::UInt16Mapping>("U16M", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 0);

const auto& normalContactMapping =
  std::make_shared<wolkabout::BoolMapping>("BM", wolkabout::RegisterMapping::RegisterType::INPUT_CONTACT, 0);
```

#### Multi Register Mapping

You can create a multi-register mapping, by listing the addresses in a vector.
You of course, need to list the operation for the mapping,
and make it as a Int32Mapping, UInt32Mapping or StringMapping based on the return value you want.

```c++
const auto& stringMapping = std::make_shared<wolkabout::StringMapping>(
  "STR1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, std::vector<int16_t>{0, 1, 2},
  wolkabout::RegisterMapping::OperationType::STRINGIFY_ASCII);
```

#### Bit Mapping

You can create a bit mapping,
one that will take a single bit from a register.You need to enlist as type HOLDING_REGISTER / INPUT_REGISTER,
the TAKE_BIT operation and the bit index.

```c++ 
const auto& getFirstBitMapping =
      std::make_shared<wolkabout::BoolMapping>("B4-1", wolkabout::RegisterMapping::RegisterType::HOLDING_REGISTER, 4,
                                               wolkabout::RegisterMapping::OperationType::TAKE_BIT, 0);
```

### Device

Next you need to merge all the mappings into a device. While creating the device, you also hand it a slaveAddress,
which will be used to access it over SERIAL/RTU if you use RS485 with multiple devices.

```c++
const auto& mappings = std::vector<std::shared_ptr<wolkabout::RegisterMapping>>{normalRegisterMapping, 
                                    normalContactMapping, stringMapping, getFirstBitMapping, getSecondBitMapping};

const auto& device = std::make_shared<wolkabout::ModbusDevice>(
      "Test Device 1", 1, mappings);
```

The device is also automatically creating `Groups`, so that part is completely out of the users hands.

If you want to be able to see mappings change in value, you have to set up the callback method.
Of course, since you're getting the `std::shared_ptr<RegisterMapping>` you need to cast it by the output type,
if you want access to the parsed value.

```c++
device->setOnMappingValueChange([](const std::shared_ptr<wolkabout::RegisterMapping>& mapping) {
    // You can do this for all output types.
    if (mapping->getOutputType() == wolkabout::RegisterMapping::OutputType::BOOL)
    {
        const auto& boolMapping = std::dynamic_pointer_cast<wolkabout::BoolMapping>(mapping);
        LOG(DEBUG) << "Application: Mapping is bool, value : " << boolMapping->getBoolValue();

        if (!boolMapping->getBoolValue())
            boolMapping->writeValue(true);
    }
    else if (mapping->getOutputType() == wolkabout::RegisterMapping::OutputType::STRING)
    {
        const auto& stringMapping = std::dynamic_pointer_cast<wolkabout::StringMapping>(mapping);
        LOG(DEBUG) << "Application: Mapping is string, value : " << stringMapping->getStringValue();

        if (stringMapping->getStringValue().empty())
            stringMapping->writeValue("Test");
    }
    else
    {
        LOG(DEBUG) << "Application: Mapping " << mapping->getReference() << " value changed.";
    }
});
```

### Client

You can create a client, TCP/IP or SERIAL/RTU depending on your needs. This will be necessary for the reader.

```c++
const auto& modbusClient = std::make_shared<wolkabout::LibModbusSerialRtuClient>(
  "/dev/tty0", 115200, 8, 1, wolkabout::LibModbusSerialRtuClient::BitParity::NONE, std::chrono::milliseconds(500));

const auto& modbusClient =
  std::make_shared<wolkabout::LibModbusTcpIpClient>("<IP ADDRESS>", 502, std::chrono::milliseconds(500));
```

### Reader

And the final part, is the reader logic, which needs the client, and list of all devices that the reader should read.

```c++
const auto& reader = std::make_shared<wolkabout::ModbusReader>(
      *modbusClient, std::vector<std::shared_ptr<wolkabout::ModbusDevice>>{device}, std::chrono::milliseconds(1000));
``` 

And the control to start/stop, you can invoke methods.
While the reader is running, make sure your main thread doesn't stop running, because the whole program will stop, 
and the reader, with the program, will crash. You can do that with a while loop with a sleep method inside.

```c++
reader->start();

while (reader->isRunning())
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

reader->stop();
```



