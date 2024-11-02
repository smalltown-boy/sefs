/*
 * sefs.cpp
 *
 * Created: 20.09.2024 16:12:07
 * Author: Aleksandr Selivanov
 */ 

#include "sefs.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

void storage::format_drive()										//A method for clearing all available EEPROM memory of a microcontroller
{                            
	for(uint16_t address = EEPROM_START_ADDR; address < TOTAL_MEMORY_SIZE; address++) {
		storage::eeprom_write_byte(address, 0xFF);
	}
}

uint8_t storage::eeprom_write_byte(uint16_t address, uint8_t byte)  //The method of writing a single byte to memory
{
	if(address < TOTAL_MEMORY_SIZE) {
		while (EECR & (1 << EEPE));
		EEAR = address;                    
		EEDR = byte;                       
		EECR |= (1 << EEMPE);              
		EECR |= (1 << EEPE);
		while (EECR & (1 << EEPE));        
		return 0;                          
	}
	else {
		return 1;													//Return error code 1
	}
}

uint8_t storage::eeprom_read_byte(uint16_t address)					
{
	uint8_t data;
	
	if(address < TOTAL_MEMORY_SIZE) {
		EEAR = address;                 
		EECR |= (1 << EERE);             
		data = EEDR;                    
	}
	
	return data;
}

uint8_t storage::add_file(char *filename, char *data, uint8_t file_length) 
{
	size_t name_length = strlen(filename);                  
	size_t data_length = file_length;                      
	
	uint16_t free_filename_space;                           
	uint16_t free_data_space;
	
	uint16_t current_filename_pointer;                      
	uint16_t current_data_pointer;
	
	uint8_t packet[FILE_NAME_SIZE + 3] = {0x00};
	uint8_t packet_pointer = 0x00; 
	
	if(name_length > FILE_NAME_SIZE)                        
	{	
		return 3;
	}	
	else
	{	
		free_filename_space = storage::eeprom_check_free_filename_memory();
		
		if(free_filename_space < name_length)
		{
			return 1;
		}
		else
		{
			free_data_space = storage::eeprom_check_free_data_memory();
			
			if(free_data_space < data_length)
			{
				return 2;
			}
			else
			{
				current_filename_pointer = storage::eeprom_read_word(TOTAL_MEMORY_SIZE - 3);
				current_data_pointer = storage::eeprom_read_word(TOTAL_MEMORY_SIZE - 1);
				
				for(uint8_t i = 0x00; i < name_length; i++)
				{
					packet[packet_pointer] = filename[i];
					packet_pointer++;
				}
				
				packet[packet_pointer++] = current_data_pointer;
				packet[packet_pointer++] = current_data_pointer >> 8;
				packet[packet_pointer++] = data_length;
				
				for(uint8_t i = 0; i < sizeof(packet); i++)
				{
					storage::eeprom_write_byte(current_filename_pointer++, packet[i]);
				}
				
				for(uint8_t i = 0; i < data_length; i++)
				{
					storage::eeprom_write_byte(current_data_pointer++, data[i]);
				}
				
				storage::eeprom_write_word(TOTAL_MEMORY_SIZE - 3, current_filename_pointer);
				storage::eeprom_write_word(TOTAL_MEMORY_SIZE - 1, current_data_pointer);
			}
		}
	}	
	
}

uint16_t storage::eeprom_check_free_filename_memory()
{
	uint16_t namespace_end = EEPROM_START_ADDR + FILE_NAME_TABLE_SIZE; 
	uint16_t free_memory = 0;
	uint16_t current_pointer;
	
	current_pointer = storage::eeprom_read_word(TOTAL_MEMORY_SIZE - 3);
	free_memory = namespace_end - current_pointer;
	
	return free_memory;
}

uint16_t storage::eeprom_check_free_data_memory()
{
	uint16_t dataspace_end = EEPROM_START_ADDR + FILE_NAME_TABLE_SIZE + DATA_SIZE_SPACE; 
	uint16_t free_memory = 0;
	uint16_t current_pointer;
	
	current_pointer = storage::eeprom_read_word(TOTAL_MEMORY_SIZE - 1);
	free_memory = dataspace_end - current_pointer;
	
	return free_memory;
}

uint8_t storage::init_drive()
{
	if((FILE_NAME_TABLE_SIZE + DATA_SIZE_SPACE) > 4091) 
	{
		return 1;                                      
	}
	else                                              
	{
		storage::eeprom_write_word(TOTAL_MEMORY_SIZE - 3, EEPROM_START_ADDR);   
		storage::eeprom_write_word(TOTAL_MEMORY_SIZE - 1, EEPROM_START_ADDR + FILE_NAME_TABLE_SIZE); 
		return 0;                                       
	}
}

uint8_t storage::eeprom_write_word(uint16_t address, uint16_t word)
{
	while (EECR & (1 << EEPE)); 
	EEAR = address;

	EEDR = word & 0xFF; 
	EECR |= (1 << EEMPE); 
	EECR |= (1 << EEPE); 

	while (EECR & (1 << EEPE)); 
	EEDR = (word >> 8) & 0xFF; 
	EEAR++; 
	EECR |= (1 << EEMPE); 
	EECR |= (1 << EEPE); 

	while (EECR & (1 << EEPE)); 
}

uint16_t storage::eeprom_read_word(uint16_t address)
{
	uint16_t word = 0;

	EEAR = address;
	EECR |= (1 << EERE);
	word |= EEDR; 
	EEAR++;
	EECR |= (1 << EERE); 
	word |= (EEDR << 8); 

	return word; 
}

uint8_t storage::read_file(file *file)
{
	uint8_t operation_result;
	
	char eeprom_dump[FILE_NAME_TABLE_SIZE];
	char *search_name = file->filename;
	uint16_t current_pointer = storage::eeprom_read_word(TOTAL_MEMORY_SIZE - 3);
	
	for(uint16_t address =  EEPROM_START_ADDR, i = 0; address <= current_pointer; address++, i++)
	{
		eeprom_dump[i] = storage::eeprom_read_byte(address);
	}
	
	for(uint16_t address = 0; address <= FILE_NAME_TABLE_SIZE; address += (FILE_NAME_SIZE + 3))
	{
		if(strncmp((char*)&eeprom_dump[address], search_name, FILE_NAME_SIZE) == 0)
		{
			char *additional = &eeprom_dump[address + FILE_NAME_SIZE];
			file->start_addr = additional[1] << 8 | additional[0];
			file->file_size = additional[2];
			
			for(uint16_t i = 0, read_addr = file->start_addr; i <= file->file_size; i++)
			{
				file->data[i] = storage::eeprom_read_byte(read_addr++);
			}
		}
		else
		{
			asm("nop");
		}
	}
	return operation_result;
}

uint8_t storage::delete_file(file *file)
{
	uint8_t operation_result;
	uint16_t address;
	
	storage::read_file(file);
	
	for(uint8_t i = 0, address = file->start_addr; i < file->file_size; i++, address++)
	{
		storage::eeprom_write_byte(address, 0xFF);
	}
}