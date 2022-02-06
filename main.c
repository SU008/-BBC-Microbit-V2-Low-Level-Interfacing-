
/* main.c - Application main entry point
   Based on an example from Zephyr toolkit,modified by frank duignan of TU Dublin
   further modified by Sajjad Ullah.  
   Copyright (c) 2015-2016 Intel Corporation
 
   SPDX-License-Identifier: Apache-2.0


 
   This example advertises three services:
   0x1800 Generic ACCESS (GAP)
   0x1801 Generic Attribute (GATT)
   And a custom service 1-2-3-4-0 
   This custom service contains a custom characteristic called char_value
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <device.h>
#include <drivers/sensor.h>
#include <stdio.h>
#include "sensirion_i2c.h"

#include "scd30.h"

#include "matrix.h"
#include "buttons.h"



/*Information source:https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.3.2/zephyr/guides/bluetooth/bluetooth-arch.html
 *Generic Access Profile (GAP)
 *GAP simplifies Bluetooth Low Energy (BLE) access by defining four distinct roles of BLE usage:
 *
 *Two types of Connection-oriented roles
 *=>Peripheral (e.g. a smart sensor, often with a limited user interface)
 *=>Central (typically a mobile phone or a PC)
 *
 *Two types of Connection-less roles
 *=>Broadcaster (sending out BLE advertisements, e.g. a smart beacon)
 *=>Observer (scanning for BLE advertisements)
 *
 *Most Zephyr-based BLE devices will most likely be peripheral-role devices.
*/



/*Information source: https://developer.nordicsemi.com/nRF_Connect_SDK/doc/1.3.2/zephyr/reference/bluetooth/gatt.html?highlight=gatt
 *Generic Attribute Profile (GATT)
 *The GATT layer manages the service database by using Application Programming Interfaces (APIs) for service registration and attribute declaration.
 *A bt_gatt_service 'struct' is declared and provides the list of attributes the service will contain. 
 *This bt_gatt_service service will then be registered using a bt_gatt_service_register() API.  
 *The helper 'macro' BT_GATT_SERVICE() can be used to declare a service.
 *Attributes can be declared using the bt_gatt_attr struct or using one of other helper macros
 *
 *In our project the smartphone application will act as the central device and the sensor as the peripheral device. 
*/









//Declare the rows and cols for led matrix and guarantee 8 bits.
int ret;
uint8_t rows = 1;
uint8_t cols = 1;

// ********************[ Start of CO2 characteristic ]**************************************
#define BT_UUID_CO2_VAL BT_UUID_128_ENCODE(1, 2, 3, 4, (uint64_t)1)                         //This Encodes a C macro 128 bit Universal Unique Identifier (UUID) into array values in little-endian format,& stores this into the attribute "BT_UUID_CO2_VAL" for the co2 sensor.
static struct bt_uuid_128 co2_id = BT_UUID_INIT_128(BT_UUID_CO2_VAL);                      //This Encodes a C struct 128 bit UUID,& stores into "co2_id" for the co2 sensor. used for GATT
uint32_t co2_value;                                                                       //This is the GATT characateristic value that is being shared over BLE and has been declared a numeric type that guarantees 32 bits.

static ssize_t read_co2(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);//declare a function called "read_co2" with parameters that is activated when the characteristic is read by central device

static ssize_t read_co2(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)//create the function called "read_co2"
{
	printf("Got a read in CO2 attribute: %d\n", co2_value);                              //print out the co2 attribute to terminal
	const char *value = (const char *)&co2_value;			                            //before we can return the attribute values, we must use a pointer to point at the value in memory
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(co2_value));  //this can now be passed into 'bt_gatt_attr_read()', which will pass the read in attribute values for co2 back up through the BLE stack
}

#define BT_GATT_CHAR1 BT_GATT_CHARACTERISTIC(&co2_id.uuid, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_co2, NULL, &co2_value)//define a GATT characteristic for co2 eg."BT_GATT_CHAR1", using the characteristic and value declaration macro 'BT_GATT_CHARACTERISTIC()' function to set the values.   
/*
 *Arguments to BT_GATT_CHARACTERISTIC =( _uuid, _props, _perm, _read, _write, _value)
 *Parameters:
 *=>_uuid – Characteristic attribute uuid.
 *
 *=>_props – Characteristic attribute properties.
 *
 *=>_perm – Characteristic Attribute access permissions.
 *
 *=>_read – Characteristic Attribute read callback.
 *
 *=>_write – Characteristic Attribute write callback.
 *
 *=>_user_data – Characteristic Attribute user data.
*/
// ********************[ End of CO2 characteristic ]****************************************















// ********************[ Start of Temp characteristic ]**************************************
#define BT_UUID_TEMP_VAL BT_UUID_128_ENCODE(1, 2, 3, 4, (uint64_t)2)                        //This Encodes a C macro 128 bit UUID into array values in little-endian format,& stores this into the attribute "BT_UUID_TEMP_VAL" for the temp sensor.
static struct bt_uuid_128 temp_id = BT_UUID_INIT_128(BT_UUID_TEMP_VAL);                    //This Encodes a C struct 128 bit UUID,& stores into "temp_id" for the co2 sensor. used for GATT
uint32_t temp_value;                                                                      //This is the GATT characateristic value that is being shared over BLE and has been declared a numeric type that guarantees 32 bits. 

static ssize_t read_temp(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);//declare a function called "read_temp" with parameters that is activated when the characteristic is read by central device

static ssize_t read_temp(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)//create the function called "read_temp"
{
	printf("Got a read in Temp attribute:  %p\n", attr);                                      //print out the temp attribute to terminal

	const char *value = (const char *)&temp_value;                                           //before we can return the attribute values, we must use a pointer to point at the value in memory
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(temp_value));      //this can now be passed into 'bt_gatt_attr_read()', which will pass the read in attribute values for temp back up through the BLE stack
}

#define BT_GATT_CHAR2 BT_GATT_CHARACTERISTIC(&temp_id.uuid, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_temp, NULL, &temp_value)//define a GATT characteristic for temp eg."BT_GATT_CHAR2", using the characteristic and value declaration macro 'BT_GATT_CHARACTERISTIC()' function to set the values.     
// ********************[ End of Temp characteristic ]****************************************












// ********************[ Start of Humidity characteristic ]**************************************
#define BT_UUID_HUM_VAL BT_UUID_128_ENCODE(1, 2, 3, 4, (uint64_t)3)                             //This Encodes a C macro 128 bit UUID into array values in little-endian format,& stores this into the attribute "BT_UUID_HUM_VAL" for the Humidity sensor.
static struct bt_uuid_128 hum_id = BT_UUID_INIT_128(BT_UUID_HUM_VAL);                          //This Encodes a C struct 128 bit UUID,& stores into "hum_id" for the Humidity sensor. used for GATT
uint32_t hum_value;                                                                           //This is the GATT characateristic value that is being shared over BLE and has been declared a numeric type that guarantees 32 bits. 

static ssize_t read_hum(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset);//declare a function called "read_hum" with parameters that is activated when the characteristic is read by central device

//Callback that is activated when the characteristic is read by central
static ssize_t read_hum(struct bt_conn *conn, const struct bt_gatt_attr *attr, void *buf, uint16_t len, uint16_t offset)//create the function called "read_hum"
{
	printf("Got a read %p\n", attr);                                                        //print out the Humidity attribute to terminal

	const char *value = (const char *)&hum_value;                                           //before we can return the attribute values, we must use a pointer to point at the value in memory
	return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(hum_value));      //this can now be passed into 'bt_gatt_attr_read()', which will pass the read in attribute values for Humidity back up through the BLE stack
}

#define BT_GATT_CHAR3 BT_GATT_CHARACTERISTIC(&hum_id.uuid, BT_GATT_CHRC_READ, BT_GATT_PERM_READ, read_hum, NULL, &hum_value)//define a GATT characteristic for Humidity eg."BT_GATT_CHAR3", using the characteristic and value declaration macro 'BT_GATT_CHARACTERISTIC()' function to set the values.     
// ********************[ End of Humidity characteristic ]****************************************












// ********************[ Service definition ]********************
#define BT_UUID_CUSTOM_SERVICE_VAL BT_UUID_128_ENCODE(1, 2, 3, 4, (uint64_t)0)            //creating a C macro 128 bit UUID for service definition 
static struct bt_uuid_128 my_service_uuid = BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_VAL);//creating a C struct 128 bit UUID for service definition
struct bt_conn *active_conn = NULL;                                                     //using this to maintain a reference to the connection with the central device (if any)

BT_GATT_SERVICE_DEFINE(my_service_svc, BT_GATT_PRIMARY_SERVICE(&my_service_uuid),   //define all the created BT_GATT_CHARACTERISTIC's

					   BT_GATT_CHAR1,  //This is the GATT characteristic for co2
					   BT_GATT_CHAR2, //This is the GATT GATT characteristic for Temp  
					   BT_GATT_CHAR3 //This is the GATT GATT characteristic for Humidity 

);

// ********************[ Advertising configuration ]********************
/* The bt_data structure type:
 * {
 * 	uint8_t type : The kind of data encoded in the following structure
 * 	uint8_t data_len : the length of the data encoded
 * 	const uint8_t *data : a pointer to the data
 * }
 * This is used for encoding advertising data
*/
/* The BT_DATA_BYTES macro
 * #define BT_DATA_BYTES(_type, _bytes...) BT_DATA(_type, ((uint8_t []) { _bytes }), sizeof((uint8_t []) { _bytes }))
 * #define BT_DATA(_type, _data, _data_len) \
 *       { \
 *               .type = (_type), \
 *               .data_len = (_data_len), \
 *               .data = (const uint8_t *)(_data), \
 *       }
 * BT_DATA_UUID16_ALL : value indicates that all UUID's are listed in the advertising packet
*/
// bt_data is an array of data structures used in advertising. Each data structure is formatted as described above
static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)), // specify BLE advertising flags = discoverable, BR/EDR not supported (BLE only)
	BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_CUSTOM_SERVICE_VAL),		  // A 128 Service UUID for the custom service
};

static void connected(struct bt_conn *conn, uint8_t err)//Callback that is activated when a connection with a central device is established
{ //checking if connection is working
	if (err)
	{
		printf("Connection failed (err 0x%02x)\n", err);
	}
	else
	{
		printf("Connected\n");
		active_conn = conn;
	}
}

// Callback that is activated when a connection with a central device is taken down
static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
	active_conn = NULL;
}

// structure used to pass connection callback handlers to the BLE stack
static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
};

// This is called when the BLE stack has finished initializing
static void bt_ready(void)
{
	int err;
	printf("Bluetooth initialized\n");

	// start advertising see https://developer.nordicsemi.com/nRF_Connect_SDK/doc/latest/zephyr/reference/bluetooth/gap.html
	/*
 * Excerpt from zephyr/include/bluetooth/bluetooth.h
 * 
 * #define BT_LE_ADV_CONN_NAME BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE | \
                                            BT_LE_ADV_OPT_USE_NAME, \
                                            BT_GAP_ADV_FAST_INT_MIN_2, \
                                            BT_GAP_ADV_FAST_INT_MAX_2, NULL)

 Also see : zephyr/include/bluetooth/gap.h for BT_GAP_ADV.... These set the advertising interval to between 100 and 150ms
 
 */
	// Start BLE advertising using the ad array defined above
	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err)
	{
		printf("Advertising failed to start (err %d)\n", err);
		return;
	}
	printf("Advertising successfully started\n");
}











//-------------------------------------------Main-----------------------------



void main(void)
{
	ret = matrix_begin();                            //set up LED matrix
	int err = 0;                                     //used for initializing the lsm303
	uint16_t interval_in_seconds = 2;			   //set up the interval rate for the SCD30 sensor to measure at in seconds.  
	float co2_ppm, temperature, relative_humidity; //create float variables to store the values of temp, humidity and c02
	co2_ppm = 0.0;
	temperature = 0.0;
	relative_humidity = 0.0;
	sensirion_i2c_select_bus(1);                 //set the I²C bus 
	sensirion_i2c_init();

	//Busy loop for initialization, because the main loop does not work without a sensor.
	while (scd30_probe() != NO_ERROR)
	{
		printf("SCD30 sensor probing failed\n");
		sensirion_sleep_usec(1000000u);
	}
	printf("SCD30 sensor probing successful\n");


	//now that sensor probing is successful, pass in these parameters  
	scd30_set_measurement_interval(interval_in_seconds);
	sensirion_sleep_usec(20000u);
	scd30_start_periodic_measurement(0);

	//checking till the lsm303 is initialized
	if (err < 0)
	{
		printf("\nError initializing lsm303.  Error code = %d\n", err);
		while (1)
			;
	}
	err = bt_enable(NULL);
	if (err)
	{
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}
	bt_ready();                               // This function starts advertising
	bt_conn_cb_register(&conn_callbacks);     //Register callbacks to monitor the state of connections. 

	printf("Zephyr Microbit CO2 sensor %s\n", CONFIG_BOARD); //print out configuration of Zephyr Microbit CO2 sensor
	while (1)
	{
		k_sleep(K_SECONDS(1));
		uint16_t data_ready = 0;
		err = scd30_get_data_ready(&data_ready);
		if (err)
		{
			printf("Error reading data_ready flag: %i\n", err);
		}
		if (data_ready)
		{
			err = scd30_read_measurement(&co2_ppm, &temperature, &relative_humidity);
			if (err)
			{
				printf("error reading measurement\n");
			}
			else
			{
				printf("measured co2 concentration: %0.2f ppm, "
					   "measured temperature: %0.2f degreeCelsius, "
					   "measured humidity: %0.2f %%RH\n",
					   co2_ppm, temperature, relative_humidity);
			}

			co2_value = co2_ppm;

			

			if (0 < co2_ppm && co2_ppm < 680) //if co2 is in range: 0<co2<680
			{
				matrix_put_pattern_smile();   //call the smile function in mathrix.h 
			}
			if (co2_ppm > 681 )                  //if co2 is in range: co2>681
			{
				matrix_put_pattern_patternUnhappy();//call the unhappy function in mathrix.h 
			}
			else
			{
				matrix_put_pattern(0, ~0);
			} //turns off all leds 

			temp_value = temperature;
			hum_value = relative_humidity;
		}
	}
};






/*
This is the smile emoji we want to show 
each pixel has been labeled with a letter eg X-a. 
This will allow us to easily create the corresponding binary code.
eg for X-a, the rows and cols are:
        rows = 0b00010;	
		cols = 0b00010;
											   rows
		-------------------------------------
		|      |      |      |      |       |   0
		-------------------------------------
		|      |  X-b |      |  X-a |       |   1
		-------------------------------------
		|      |      |      |      |       |   0
		-------------------------------------
		|   X-d|      |      |      |   X-c |   0
		-------------------------------------
		|      |   X-g|  X-f |   X-e|       |   0
		-------------------------------------
cols:      0      0      0      1      0
*/




