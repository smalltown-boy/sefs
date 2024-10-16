# SEFS
## _Simple EEPROM File System for ATmega microcontrollers._

## Description of the project
Simple EEPROM File System (SEFS) is designed for ATmega series microcontrollers with built-in EEPROM memory.
The file system is designed to store small configuration files used in systems with microcontrollers.

## Adding a library to a project

- Add the files ```sefs.h``` and ```sefs.cpp``` to the folder with the source files of your project.
- The library is connected using the ```#include "sefs.h"``` directive.

## Setting the parameters of the microcontroller's EEPROM memory (all values in bytes).

Open the file ``sefs.h`` and edit the following parameters so that they fit your microcontroller and goals:

- ```#define TOTAL_MEMORY_SIZE``` - Specify the total number of EEPROM memory of the microcontroller.
- ```#define FILE_NAME_TABLE_SIZE``` - Specify the size of the _name table_ used to store information about files.
- ```#define DATA_SIZE_SPACE``` - Specify the amount of memory that will be used to store files.
- ```#define FILE_NAME_SIZE``` - Specify the size of the file name.
- ```#define EEPROM_START_ADDR``` - Specify the starting address of the allocated memory for the file system.

Example of setting parameters:
```
#define TOTAL_MEMORY_SIZE 4095
#define FILE_NAME_TABLE_SIZE 64
#define DATA_SIZE_SPACE 512
#define FILE_NAME_SIZE 5
#define EEPROM_START_ADDR 0
```

**Attention!** The sum of the ```FILE_NAME_TABLE_SIZE``` and ```DATA_SIZE_SPACE``` parameters must not exceed the value of the ```TOTAL_MEMORY_SIZE``` parameter.

## Using the library
### Clearing the memory

As a first step, you need to clear the entire EEPROM memory of the microcontroller. This can be done using the `format_drive()` method of the `storage` class.

Example code:
```
storage memory;
memory.format_drive();
```

### Initializing memory

After clearing all the memory, it needs to be "initialized". This procedure records the values of the start addresses of the _name table_ and the data space in specially designated places in the EEPROM memory.

Example code:
```
memory.init_drive();
```

### Writing a file

Let's say you have an array of data that you want to record. To do this, you need to use the method ```add_file(char *filename, char *data, uint8_t file_length)```.

Example code:
```
char ip_addr[] = {0x0A,0x00,0x1E,0x1C};
memory.add_file("ipaddr",ip_addr,sizeof(ip_addr));
```

**Attention!** The number of characters passed to the first argument of the class method used as a name must not exceed the value of the parameter ``FILE_NAME_SIZE`` from the file ``sehs.h``.

### Reading a file

To search for the required file by name, use the ```read_file(file *file)``` method. As a parameter, it is passed a structure containing a field with the name of the file being searched, which is set by the user himself. The remaining fields of the structure are filled in by the ```read_file``` method if the file search was performed successfully.

The contents of the ```file``` structure:
```
typedef struct {
	char *filename;               //File name
	uint16_t start_addr;          //The address from which the data starts
	uint8_t file_size;            //File size
	uint8_t *data;                //File data
	} file;
```

Example code:
```
file datafile;
datafile.filename = "file1";
memory.read_file(&datafile)
```

If the file was successfully found and read, all fields of the `file` structure will be filled with data. Otherwise, the method will return an error code (it is omitted in the example).

### Deleting a file

To delete a file, use the method ```delete_file(file *file)```. The transfer of the file name for deletion is carried out in the same way as for the method ```read_file(file *file)```.

Example code:
```
file datafile;
datafile.filename = "file2";
memory.delete_file(&datafile)
```

## Version history

| По левому краю | По центру | По правому краю |
| текст | текст | текст |

|---------------------------------------------------------------------------------------------------------------------------------|
|version 0.0.1| Added functions for formatting EEPROM memory, initializing the file system, as well as writing and reading files. |
|version 0.0.2| Added a function to delete a file, and fixed a bug in the file reading method.                                    |
|---------------------------------------------------------------------------------------------------------------------------------|




