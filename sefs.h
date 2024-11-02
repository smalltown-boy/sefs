/*
 * IncFile1.h
 *
 * Created: 20.09.2024 16:11:44
 * Author: Aleksandr Selivanov
 */ 
#ifndef INCFILE1_H_
#define INCFILE1_H_

#include <stdint.h>

#define TOTAL_MEMORY_SIZE 4095
#define FILE_NAME_TABLE_SIZE 64
#define DATA_SIZE_SPACE 512
#define FILE_NAME_SIZE 5
#define EEPROM_START_ADDR 0

typedef struct {
	char *filename;
	uint16_t start_addr;
	uint8_t file_size;
	uint8_t *data;
	} file;

class storage {
	public:
		void format_drive();
		uint8_t init_drive();
		uint8_t add_file(char *filename, char *data, uint8_t file_length);
		uint8_t read_file(file *file);
		uint8_t delete_file(file *file);
		
	private:
		uint8_t eeprom_write_byte(uint16_t address, uint8_t byte);
		uint8_t eeprom_read_byte(uint16_t address);
		uint16_t eeprom_check_free_filename_memory();
		uint16_t eeprom_check_free_data_memory();
		uint8_t eeprom_write_word(uint16_t address, uint16_t word);
		uint16_t eeprom_read_word(uint16_t address);	
	};



#endif /* INCFILE1_H_ */