/**
* The copyrights, all other intellectual and industrial
* property rights are retained by XMOS and/or its licensors.
* Terms and conditions covering the use of this code can
* be found in the Xmos End User License Agreement.
*
* Copyright XMOS Ltd 2012
*
* In the case where this code is a modification of existing code
* under a separate license, the separate license terms are shown
* below. The modifications to the code are still covered by the
* copyright notice above.
*
**/


#include "object_dictionary.h"


// devuelve directamente contenido del arreglo donde se guardan los datos del diccionario
unsigned char od_read_byte (int address){
  return *(unsigned char *)address;
}

// escribe byte directamente al arreglo donde se guardan los datos del diccionario
void od_write_byte (int address, unsigned char c){
  *(unsigned char *)address = c;
}

unsigned short od_read_short (int address){
  return *(unsigned short *)address;
}
void od_write_short (int address, unsigned short c){
  *(unsigned short*)address = c;
}
int od_read_int (int address){
  int i=0;
  i |= *(unsigned char *)address++;
  i |= (*(unsigned char *)address++) << 8;
  i |= (*(unsigned char *)address++) << 16;
  i |= (*(unsigned char *)address) << 24;
  return i;
}
void od_write_int (int address, int i){
  *(unsigned char *)address++ = i & 0x000000FF;
  *(unsigned char *)address++ = (i & 0x0000FF00) >> 8;
  *(unsigned char *)address++ = (i & 0x00FF0000) >> 16;
  *(unsigned char *)address = (i & 0xFF000000) >> 24;
}


// Devuelve indice del objeto dentro del arreglo de chars data[]
// Permite acceso directo al diccionario sin llamar funciones
int od_find_data_address(int index, unsigned char od_sub_index)
{
  // index es índice del objeto buscado
  // obtenemos puntero al sub-objeto (subindice)
  int p = (int) *(object_dict[index].subindex_data + od_sub_index);
  return p;
}


/*---------------------------------------------------------------------------
 Find the index of object from the object dictionary based on object address
 ---------------------------------------------------------------------------*/
int od_find_index(int address)
{
  int index = 0;
  while(index != NUMBER_OF_OBJECTS)
  {
    if (object_dict[index].addr == address)
      break;
    index++;
  }// find index
  if (index == NUMBER_OF_OBJECTS)
    return -1;
  else
    return index;
}


/*---------------------------------------------------------------------------
 Find data length od the object based on index and subindex
 ---------------------------------------------------------------------------*/
int od_find_data_length(int index, unsigned char sub_index)
{
  int address, True = 0, length, count = 0;
  address = (int) *(object_dict[index].subindex_data + sub_index);
  while(count != OFFSETS_OF_SUBINDEX_LENGTH)
  {
    if (count == OFFSETS_OF_SUBINDEX_LENGTH - 1)
    {
      if (address >= (int) subindex_size_data[count].address)
        True = 1;
    }
    else
    {
      if ((address >= (int) subindex_size_data[count].address) && (address
          < (int) subindex_size_data[count + 1].address))
        True = 1;
    }
    if (True)
    {
      length = subindex_size_data[count].size;
      True = 0;
    }
    count++;
  } //find the data length
  return length;
}


/*---------------------------------------------------------------------------
 Read Data from the object dictionary and write it to the data buffer
 ---------------------------------------------------------------------------*/
void od_read_data(int index,
                       unsigned char od_sub_index,
                       char data_buffer[],
                       unsigned char data_length)
{
  char count = 0;
  while(data_length != count)
  {
    data_buffer[count]
        = *((unsigned char *) (*(object_dict[index].subindex_data
            + od_sub_index)) + count);
    count++;
  }
}


/*---------------------------------------------------------------------------
 Write data to the object dictionary based on index and sub index
 ---------------------------------------------------------------------------*/
void od_write_data(int index,
                      unsigned char od_sub_index,
                      char data_buffer[],
                      unsigned char data_length)
{
  char count = 0;
  while(data_length != count)
  {
    *((unsigned char *) (*(object_dict[index].subindex_data + od_sub_index))
        + count) = data_buffer[count];
    count++;
  }
}


/*---------------------------------------------------------------------------
 Find the access type of object from the object dictionary based on adderss
 ---------------------------------------------------------------------------*/
unsigned char od_find_access_of_index(int index, unsigned char od_sub_index)
{
  unsigned char access;
  access = *((unsigned char *) (object_dict[index].access_pointer)
      + od_sub_index);
  return access;
}

/*---------------------------------------------------------------------------
 Find No of SI entries in the object dictionary of an object based on index
 ---------------------------------------------------------------------------*/
unsigned od_find_no_of_si_entries(int index)
{
  return object_dict[index].subindex_count;
}
