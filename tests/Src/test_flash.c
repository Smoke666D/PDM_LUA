/*
 * test_flash.c
 *
 *  Created on: Jul 20, 2022
 *      Author: 79110
 */
#include "tests.h"
#include "unity.h"
#include "flash.h"
#include "string.h"

#define TEST_FLASH_SIZE  100U

static uint8_t data[TEST_FLASH_SIZE] = { 0U };

void test_eFLASHwriteScript ( void )
{
  FLASH_STATE res = FLASH_OK;
  for ( uint32_t i=0U; i<TEST_FLASH_SIZE; i++ )
  {
    data[i] = ( uint8_t )i;
  }
  res = eFLASHstartWriting();
  TEST_ASSERT_EQUAL( FLASH_OK, res );
  res = eFLASHwriteScript( 0U, data, TEST_FLASH_SIZE );
  TEST_ASSERT_EQUAL( FLASH_OK, res );
  res = eFLASHendWriting();
  TEST_ASSERT_EQUAL( FLASH_OK, res );
  res = eFLASHwriteScript( 0U, data, FLASH_STORAGE_LENGTH );
  TEST_ASSERT_EQUAL( FLASH_ERROR_LENGTH, res );
  return;
}
void test_eFLASHreadScript ( void )
{
  FLASH_STATE res = FLASH_OK;
  memset( data, 0U, TEST_FLASH_SIZE );
  res = eFLASHreadScript( 0U, data, TEST_FLASH_SIZE );
  TEST_ASSERT_EQUAL( FLASH_OK, res );
  for ( uint32_t i=0U; i<TEST_FLASH_SIZE; i++ )
  {
    TEST_ASSERT_EQUAL_UINT8( ( uint8_t )i, data[i] );
  }
  res = eFLASHreadScript( 0U, data, FLASH_STORAGE_LENGTH );
  TEST_ASSERT_EQUAL( FLASH_ERROR_LENGTH, res );
  return;
}

void test_uFLASHgetScript ( void )
{
  TEST_ASSERT_EQUAL_UINT32( FLASH_STORAGE_ADR, uFLASHgetScript() );
  const uint8_t* script = uFLASHgetScript();
  TEST_ASSERT_EQUAL_UINT32( FLASH_STORAGE_ADR, script );
  for ( uint32_t i=0U; i<TEST_FLASH_SIZE; i++ )
  {
    TEST_ASSERT_EQUAL_UINT8( ( uint8_t )i, script[i] );
  }
  uint8_t dd0 = script[0U];
  uint8_t dd1 = script[1U];
  uint8_t dd2 = script[2U];
  return;
}

void runTest_flash ( void )
{
  UnitySetTestFile( "test_flash.c" );
  UnityDefaultTestRun( test_eFLASHwriteScript, "Writing lua script to the FLASH", 0U );
  UnityDefaultTestRun( test_eFLASHreadScript, "Read lua script from the FLASH", 0U );
  UnityDefaultTestRun( test_uFLASHgetScript, "Read lua script from pointer to the Flash", 0U );
  return;
}

