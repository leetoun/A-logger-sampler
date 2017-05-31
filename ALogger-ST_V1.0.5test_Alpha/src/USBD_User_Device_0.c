/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2016 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_Device_0.c
 * Purpose: USB Device User module
 * Rev.:    V6.8.1
 *----------------------------------------------------------------------------*/
/*
 * USBD_User_Device_0.c is a code template for the user specific 
 * Device events and Control Endpoint 0 requests handling. It allows user to 
 * handle all user level events as well as to intercept all Control Endpoint 0 
 * requests and handle them overriding the default USB Library handling.
 *
 * Uncomment "Example code" lines to override USB Device Descriptor.
 *
 * IMPORTANT NOTE!!!: Maximum packet size must not be set to less that 32 bytes!
 */
 
/**
 * \addtogroup usbd_coreFunctions_api
 *
 */
 
 
//! [code_USBD_User_Device]
 
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rl_usb.h"
#include "rtt_log.h"
 
 
//// Example code specifying USB Device Descriptor: 
//// Device 0 USB Device Descriptor
//const uint8_t device0_dev_desc[] = {
//  18,                                   /* bLength            = 18 bytes      */
//  USB_DEVICE_DESCRIPTOR_TYPE,           /* bDescriptorType    = Device Desc   */
//  0x00, 0x02,                           /* bcdUSB             = 2.00          */
//  0x00,                                 /* bDeviceClass       = Defined in IF */
//  0x00,                                 /* bDeviceSubClass    = Defined in IF */
//  0x00,                                 /* bDeviceProtocol    = Defined in IF */
//  64,                                   /* bMaxPacketSize0    = 64 bytes !!! Must be same as USBD0_MAX_PACKET0 in USBD_Config_0.c */
//  0x51, 0xC2,                           /* idVendor           = 0xC251        */
//  0x01, 0x00,                           /* idProduct          = 1             */
//  0x00, 0x01,                           /* bcdDevice          = 1.00          */
//  0x01,                                 /* iManufacturer      = String1       */
//  0x02,                                 /* iProduct           = String2       */
//  0x03,                                 /* iSerialNumber      = String3       */
//  0x01                                  /* bNumConfigurations = 1 config      */
//};

extern uint8_t usb_connected; 
extern uint8_t sensor_busy; 

 
// \brief Callback function called when VBUS level changes
// \param[in]     level                current VBUS level
//                                       - true: VBUS level is high
//                                       - false: VBUS level is low
void USBD_Device0_VbusChanged (bool level) {
	if ( sensor_busy == true )
		return;

	Log("USB %s.\n", level ? "connected" : "disconnected");

	if ( level == 1 ) {
		usb_connected = 1;
		USBD_MSC0_Initialize();
	} else {
		usb_connected = 0;
		USBD_MSC0_Uninitialize();  
	}
}

/*
static bool handle_request;
 
// \brief Callback function called during USBD_Initialize to initialize the USB Device
void USBD_Device0_Initialize (void) {
  // Handle Device Initialization
 
  handle_request = false;
}
 
// \brief Callback function called during USBD_Uninitialize to de-initialize the USB Device
void USBD_Device0_Uninitialize (void) {
  // Handle Device De-initialization
}

 
// \brief Callback function called upon USB Bus Reset signaling
void USBD_Device0_Reset (void) {
}
 
// \brief Callback function called when USB Bus speed was changed to high-speed
void USBD_Device0_HighSpeedActivated (void) {
}
 
// \brief Callback function called when USB Bus goes into suspend mode (no bus activity for 3 ms)
void USBD_Device0_Suspended (void) {
}
 
// \brief Callback function called when USB Bus activity resumes
void USBD_Device0_Resumed (void) {
}
 
// \brief Callback function called when Device was successfully enumerated
// \param[in]     val                  current configuration value
//                                       - value 0: not configured
//                                       - value > 0: active configuration
void USBD_Device0_ConfigurationChanged (uint8_t val) {
}
 
// \brief Callback function called when Set Feature for Remote Wakeup comes over Control Endpoint 0
void USBD_Device0_EnableRemoteWakeup (void) {
}
 
// \brief Callback function called when Clear Feature for Remote Wakeup comes over Control Endpoint 0
void USBD_Device0_DisableRemoteWakeup (void) {
}
 
// \brief Callback function called when Device 0 received SETUP PACKET on Control Endpoint 0
// \param[in]     setup_packet             pointer to received setup packet.
// \param[out]    buf                      pointer to data buffer used for data stage requested by setup packet.
// \param[out]    len                      pointer to number of data bytes in data stage requested by setup packet.
// \return        usbdRequestStatus        enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed: request was not processed; processing will be done by USB library
// \return        usbdRequestOK:           request was processed successfully (send Zero-Length Packet if no data stage)
// \return        usbdRequestStall:        request was processed but is not supported (stall Endpoint 0)
usbdRequestStatus USBD_Device0_Endpoint0_SetupPacketReceived (const USB_SETUP_PACKET *setup_packet, uint8_t **buf, uint32_t *len) {
 
  switch (setup_packet->bmRequestType.Type) {
    case USB_REQUEST_STANDARD:
//      // Example code handling Get Device Descriptor request: 
//      if ((setup_packet->bmRequestType.Dir       == USB_REQUEST_DEVICE_TO_HOST) &&      // Request to get
//          (setup_packet->bmRequestType.Recipient == USB_REQUEST_TO_DEVICE     ) &&      // from device
//          (setup_packet->bRequest                == USB_REQUEST_GET_DESCRIPTOR) &&      // the descriptor
//         ((setup_packet->wValue >> 8)            == USB_DEVICE_DESCRIPTOR_TYPE) &&      // Device Descriptor Type
//          (setup_packet->wIndex                  == 0U                        )) {      // Index = 0
//        *buf = (uint8_t *)device0_dev_desc;
//        *len = sizeof    (device0_dev_desc);
//        handle_request = true;
//        return usbdRequestOK;
//      }
      break;
    case USB_REQUEST_CLASS:
      break;
    case USB_REQUEST_VENDOR:
      break;
    case USB_REQUEST_RESERVED:
      break;
  }
 
  return usbdRequestNotProcessed;
}
 
// \brief Callback function called when SETUP PACKET was processed by USB library
// \param[in]     setup_packet            pointer to processed setup packet.
void USBD_Device0_Endpoint0_SetupPacketProcessed (const USB_SETUP_PACKET *setup_packet) {
 
  switch (setup_packet->bmRequestType.Type) {
    case USB_REQUEST_STANDARD:
      break;
    case USB_REQUEST_CLASS:
      break;
    case USB_REQUEST_VENDOR:
      break;
    case USB_REQUEST_RESERVED:
      break;
  }
}
 
// \brief Callback function called when Device 0 received OUT DATA on Control Endpoint 0
// \param[in]     len                      number of received data bytes.
// \return        usbdRequestStatus        enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed: request was not processed; processing will be done by USB library
// \return        usbdRequestOK:           request was processed successfully (send Zero-Length Packet)
// \return        usbdRequestStall:        request was processed but is not supported (stall Endpoint 0)
// \return        usbdRequestNAK:          request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_Device0_Endpoint0_OutDataReceived (uint32_t len) {
 
  return usbdRequestNotProcessed;
}
 
// \brief Callback function called when Device 0 sent IN DATA on Control Endpoint 0
// \param[in]     len                      number of sent data bytes.
// \return        usbdRequestStatus        enumerator value indicating the function execution status
// \return        usbdRequestNotProcessed: request was not processed; processing will be done by USB library
// \return        usbdRequestOK:           request was processed successfully (return ACK)
// \return        usbdRequestStall:        request was processed but is not supported (stall Endpoint 0)
// \return        usbdRequestNAK:          request was processed but the device is busy (return NAK)
usbdRequestStatus USBD_Device0_Endpoint0_InDataSent (uint32_t len) {
 
//  // Example code Get Device Descriptor was sent: 
//  if (handle_request == true) {
//    handle_request = false;
//    return usbdRequestOK;               // Acknowledge custom handled request
//  }
 
  return usbdRequestNotProcessed;
}

*/

//! [code_USBD_User_Device]
