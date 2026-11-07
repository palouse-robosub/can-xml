# can-xml
Source of truth for CAN IDs and their uses for Palouse Robosub.

## Usage
Add to your `CMakeLists.txt`.\
Depend on the generated `guppy_can.hpp`.\
Look at `include/shared.hpp` for the stubs of what is provided in `guppy_can.hpp`.

## Interpreting IDs
Messages are sent as little endian bytes. Without CAN FD the amount of data sent
in each message is 1-8 bytes. As of time of writing we almost exclusively send
4 byte messages.
### Unspecified
Could be any data. You need to read the relevant code (if the ID is currently in use)
and figure out what it is sending. If you want to add a new data type it would be
greatly appreciated if you make a PR to the repo or open an issue.
### Version
### State
A four byte message where the three least significant bytes are version
numbers (least significant byte is the least significant version number, etc.)

The most significant byte is either 0x00 normally or 0xff in case
of an error with the version. The least significant byte then represents an error
code:

| LSB | Error                                             |
|-----|---------------------------------------------------|
| 1   | Tag too short / doesn't exist                     |
| 2   | Non-numeric character                             |
| 3   | Single version component greater than 255         |
| 4   | Empty version component                           |
| 5   | Greater or fewer than three components of version |
### Bool
Least-significant-bit will be 0 for false and 1 for true
### UInt32
Little endian unsigned 32 bit integer
### Int32
Little endian signed 32 bit integer
### Float32
Little endian 32 bit floating point number. Most of the time this needs to be a
number [-1, 1] or [0, 1] because a float is being used to represent a percentage.