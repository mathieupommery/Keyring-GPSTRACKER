
#include "ee.h"
#include "eeConfig.h"
#include <string.h>

#define   PAGE                  0
#define   SECTOR                1
#define   PAGE_NUM              100



#define   _EE_SIZE              2048
#define   _EE_ADDR_INUSE        (((uint32_t)0x08000000) | (_EE_SIZE * _EE_USE_FLASH_PAGE_OR_SECTOR))
#define   _EE_FLASH_BANK        FLASH_BANK_1
#define   _EE_PAGE_OR_SECTOR    PAGE_NUM
#if (_EE_USE_FLASH_PAGE_OR_SECTOR > 127)
#error  "Please Enter correct address, maximum is (127)"
#endif

#if (_EE_USE_RAM_BYTE > 0)
uint8_t ee_ram[_EE_USE_RAM_BYTE];
#endif

//##########################################################################################################
bool ee_init(void)
{
#if (_EE_USE_RAM_BYTE > 0)
  return ee_read(0, _EE_USE_RAM_BYTE, NULL);
#else
  return true;
#endif
}
//##########################################################################################################
bool ee_format(bool keepRamData)
{
  uint32_t error;
  HAL_FLASH_Unlock();
  FLASH_EraseInitTypeDef flashErase;
#if _EE_PAGE_OR_SECTOR == PAGE
	flashErase.NbPages = 1;
  flashErase.Page = _EE_USE_FLASH_PAGE_OR_SECTOR;
  flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
#elif _EE_PAGE_OR_SECTOR == SECTOR
  flashErase.NbSectors = 1;
  flashErase.Sector =  _EE_ADDR_INUSE;
  flashErase.TypeErase = FLASH_TYPEERASE_SECTORS;
#elif _EE_PAGE_OR_SECTOR == PAGE_NUM
  flashErase.NbPages = 1;
  flashErase.Page = _EE_USE_FLASH_PAGE_OR_SECTOR;
  flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
#endif
#ifdef _EE_FLASH_BANK
  flashErase.Banks = _EE_FLASH_BANK;
#endif
#ifdef _EE_VOLTAGE_RANGE
  flashErase.VoltageRange = _EE_VOLTAGE_RANGE;
#endif
  if (HAL_FLASHEx_Erase(&flashErase, &error) == HAL_OK)
  {
    HAL_FLASH_Lock();
    if (error != 0xFFFFFFFF)
      return false;
    else
    {
#if (_EE_USE_RAM_BYTE > 0)
      if (keepRamData == false)
        memset(ee_ram, 0xFF, _EE_USE_RAM_BYTE);
#endif
      return true;
    }
  }
  HAL_FLASH_Lock();
  return false;
}
//##########################################################################################################
bool ee_read(uint32_t startVirtualAddress, uint32_t len, uint8_t* data)
{
  if ((startVirtualAddress + len) > _EE_SIZE)
    return false;
  for (uint32_t i = startVirtualAddress; i < len + startVirtualAddress; i++)
  {
    if (data != NULL)
    {
      *data = (*(__IO uint8_t*) (i + _EE_ADDR_INUSE));
      data++;
    }
#if (_EE_USE_RAM_BYTE > 0)
    if (i < _EE_USE_RAM_BYTE)
      ee_ram[i] = (*(__IO uint8_t*) (i + _EE_ADDR_INUSE));
#endif
  }
  return true;
}
//##########################################################################################################
bool ee_write(uint32_t startVirtualAddress, uint32_t len, uint8_t *data)
{
  if ((startVirtualAddress + len) > _EE_SIZE)
    return false;
  if (data == NULL)
    return false;
  HAL_FLASH_Unlock();
#ifdef FLASH_TYPEPROGRAM_BYTE
  for (uint32_t i = 0; i < len ; i++)
  {		
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, ((i + startVirtualAddress)) + _EE_ADDR_INUSE, (uint64_t)(data[i])) != HAL_OK)
    {
      HAL_FLASH_Lock();
      return false;
    }
  }	
#endif
#ifdef FLASH_TYPEPROGRAM_HALFWORD
  for (uint32_t i = 0; i < len ; i+=2)
  {		
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ((i + startVirtualAddress)) + _EE_ADDR_INUSE, (uint64_t)(data[i] | (data[i+1] << 8))) != HAL_OK)
    {
      HAL_FLASH_Lock();
      return false;
    }
  }	
#endif
#ifdef FLASH_TYPEPROGRAM_DOUBLEWORD
  for (uint32_t i = 0; i < len; i += 8)
  {
    uint64_t data64 = data[i];
    data64 += data[i + 1] * 0x100;
    data64 += data[i + 2] * 0x10000;
    data64 += data[i + 3] * 0x1000000;
    data64 += data[i + 4] * 0x100000000;
    data64 += data[i + 5] * 0x10000000000;
    data64 += data[i + 6] * 0x1000000000000;
    data64 += data[i + 7] * 0x100000000000000;
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, ((i + startVirtualAddress)) + _EE_ADDR_INUSE, data64) != HAL_OK)
    {
      HAL_FLASH_Lock();
      return false;
    }
  }
#endif
  HAL_FLASH_Lock();
  return true;
}
//##########################################################################################################
bool ee_writeToRam(uint32_t startVirtualAddress, uint32_t len, uint8_t* data)
{
#if (_EE_USE_RAM_BYTE > 0)
  if ((startVirtualAddress + len) > _EE_USE_RAM_BYTE)
    return false;
  if (data == NULL)
    return false;
  memcpy(&ee_ram[startVirtualAddress], data, len);
  return true;
#else
  return false;
#endif
}
//##########################################################################################################
bool  ee_commit(void)
{
#if (_EE_USE_RAM_BYTE > 0)
  if (ee_format(true) == false)
    return false;
  return ee_write(0, _EE_USE_RAM_BYTE, ee_ram);
#else
  return false;
#endif
}
//##########################################################################################################
uint32_t  ee_maxVirtualAddress(void)
{
  return (_EE_SIZE);  
}
//##########################################################################################################
