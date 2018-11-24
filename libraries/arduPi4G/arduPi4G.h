/*! \file arduPi4G.h
 *  \brief Library for managing Telit LE910
 *
 *  Copyright (C) 2016 Libelium Comunicaciones Distribuidas S.L.
 *  http://www.libelium.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 2.1 of the License, or
 *  (at your option) any later version.
 *   
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *  Version:        1.3
 *  Design:         David Gascon
 *  Implementation: A. Gallego, Y. Carmona, R. Martin
 */
 
/*! \def arduPi4G_h
    \brief The library flag    
 */
#ifndef arduPi4G_h
#define arduPi4G_h

/******************************************************************************
 * Includes
 *****************************************************************************/

#include <inttypes.h>
#include "../../arduPi-api/arduPiUART.h"
#include "./utility/arduPi4G_constants.h"

/******************************************************************************
 * Definitions & Declarations
 *****************************************************************************/

//! DEBUG_ARDUPI4G
/*! Possible values:
 *  0: No debug mode enabled
 *  1: debug mode enabled for error output messages
 *  2: debug mode enabled for both error and ok messages
 */
#define DEBUG_ARDUPI4G    0

// DESCRIPTION         PIN     PORT        PIN     ARDUINO PIN
// give names to arduino pinout for 4G module
#define RXD1_PIN         0  //  PD  0   **   0  **  RX
#define TXD1_PIN         1  //  PD  1   **   1  **  TX
#define GPRS_PW          2  //  PD  2   **   2  **  DIGITAL 2
#define RING_PIN         3  //  PD  3   **   3  **  DIGITAL 3

// define print message
#define PRINT_LE910(str)    printf((char*)"[LE910] "); printf(str);

// APN default settings
#define LE910_GPRS_APN      "APN"
#define LE910_GPRS_LOGIN    "user"
#define LE910_GPRS_PASSW    "password"

// Error Constants
#define LE910_ERROR_CME     (char*)"+CME ERROR:"
#define LE910_ERROR_CMS     (char*)"+CMS ERROR:"
#define LE910_ERROR_CODE    (char*)"ERROR:"
#define LE910_ERROR         (char*)"ERROR\r\n"
#define LE910_OK            (char*)"OK"

// LE910 Baud Rate
#define LE910_RATE  115200

#define LE910_GGA           0
#define LE910_GSA           1
#define LE910_GLL           2
#define LE910_VTG           3
#define LE910_RMC           4
#define LE910_GSV           5

// Incoming data options
#define LE910_INCOMING_SMS  1
#define LE910_INCOMING_IP   2

// Patterns for module communication
#define LE910_DATA_TO_MODULE (char*)">>>"
#define LE910_DATA_FROM_MODULE (char*)"<<<"

// Maximum packet size for FTP download
#define LE910_MAX_DL_PAYLOAD 490


//! Structure to define the info to be stored for all sockets
struct SocketInfo_t
{
    uint8_t id;         // from 1 to 6
    uint16_t sent;      // total num of bytes sent since socket was opened
    uint16_t received;  // total num of bytes received since socket was opened
    uint16_t size;      // total num of pending bytes to read which arrived through the socket
    uint16_t ack;       // total num of bytes sent and not yet acknowledged data since socket was opened
};


//! Structure to define the status to be stored for all sockets
struct SocketStatus_t
{
    uint8_t id;         // from 1 to 6
    uint8_t state;      // actual state of the socket
    char localIp[16];   // local IP address
    uint16_t localPort; // local port
    char remoteIp[16];  // remote IP address
    uint16_t remotePort;// remote port
};


//! Structure to define the status to be stored for all sockets
struct SocketStatusSSL_t
{
    uint8_t id;         // from 1 to 6
    uint8_t state;      // actual state of the socket
};


/******************************************************************************
 * Class
 *****************************************************************************/
//! arduPi4G class

class arduPi4G : public arduPiUART
{

private:
    /*! Attribute for  APN 
     */
    char _apn[30];
    
    /*! Attribute for  APN login
     */
    char _apn_login[30];
    
    /*! Attribute for  APN password
     */
    char _apn_password[30]; 
    
    /*! This function parses the error copde returned by the module. At the
     * point this function is called, the UART is supposed to have received: 
     * "+CME ERROR: <err>\r\n" and the first part of the response has been 
     * already detected: "+CME ERROR:", so this function needs to parse the 
     * error code.
     * 
     * If error code is parsed succesfully then the attribute _errorCode stores
     * this error information
     * 
     * @return '0' ok; '1' error
     */  
    uint8_t getErrorCode();
    
    //! This function configures the remote server and sends the request
    /*!
    //! This function configures the remote server and sends the request
    /*!
    \param  uint8_t method: selected HTTP method:   arduPi4G::HTTP_GET
                                                    arduPi4G::HTTP_HEAD
                                                    arduPi4G::HTTP_DELETE
                                                    arduPi4G::HTTP_POST
                                                    arduPi4G::HTTP_PUT
    \param  char* url: host name or IP address of the server
    \param  uint16_t port: server port
    \param  char* resource: parameter indicating the HTTP resource, object of the
                    request
    \param  char* data: data to send in POST/PUT method
    \param  uint16_t data_length: data length to send in POST/PUT method
    \return     0 if OK
                1 if error setting URL and port
                2 if error sending the request
                3 if error sending POST / PUT data
                4 if worng method has been selected
    */
    uint8_t httpRequest(uint8_t method,
                        char* url, 
                        uint16_t port,
                        char* resource,
                        char* data);
                        
    uint8_t httpRequest(uint8_t method,
                        char* url, 
                        uint16_t port,
                        char* resource,
                        uint8_t* data,
                        uint16_t length);
    
    //! This function waits the URC code and reads the data availble
    /*!
    \param  uint32_t wait_timeout: timeout for URC
    \return     0 if OK
                1 if timeout waiting the URC 
                2 if error reading the URC
                3 if error reading the HTTP data
                4 if error reading the HTTP data
    */
    uint8_t httpWaitResponse(uint32_t wait_timeout);
    

    

public:
    
    char _ip[16]; // xxx.yyy.zzz.www
    uint16_t _errorCode;
    int _temp;
    int _tempInterval;
    int _rssi;
    int _networkType;   
    uint8_t _incomingType;
    int _smsIndex;
    int _socketIndex;
    int _httpCode;
    uint32_t _filesize;
    char _smsStatus[12];
    char _smsNumber[20];
    char _smsDate[9];
    char _smsTime[12];
    SocketInfo_t socketInfo[6];
    SocketStatus_t socketStatus[6];
    SocketStatusSSL_t socketStatusSSL[1];
    
    //! Profile definition for multiple sockets
    enum ProfileSocketEnum
    {
        SOCKET_1 = 0,
        SOCKET_2 = 1,
        SOCKET_3 = 2,
        SOCKET_4 = 3,
        SOCKET_5 = 4,
        SOCKET_6 = 5,
    };
    
    //! Socket Status definition
    enum SocketStatusEnum
    {
        STATUS_CLOSED           = 0,
        STATUS_ACTIVE           = 1,
        STATUS_SUSPENDED        = 2,
        STATUS_SUSPENDED_DATA   = 3,
        STATUS_LISTENING        = 4,
        STATUS_INCOMING         = 5,
        STATUS_OPENING          = 6,
    };

    //! Wireless Network enumeration
    enum WirelessNetworkEnum
    {
        NETWORK_GSM             = 12,
        NETWORK_UTRAN           = 22,
        NETWORK_3GPP            = 25, // default
        NETWORK_EUTRAN_ONLY     = 28,
        NETWORK_GERAN_UTRAN     = 29,
        NETWORK_GERAN_EUTRAN    = 30,
        NETWORK_UTRAN_EUTRAN    = 31,
    };
    
    //! GPS Mode Enumeration
    enum GPSModeEnum
    {
        GPS_MS_ASSISTED     = 0,
        GPS_MS_BASED        = 1,
        GPS_AUTONOMOUS      = 3,
    };

    //! Module Information Enumeration
    enum ModuleInformationEnum
    {
        INFO_HW                 = 0,
        INFO_MANUFACTURER_ID    = 1,
        INFO_MODEL_ID           = 2,
        INFO_REV_ID             = 3,
        INFO_IMEI               = 4,
        INFO_IMSI               = 5,
        INFO_ICCID              = 6,
    };

    //! SMS Delete Flag
    enum DeleteFlagEnumeration
    {
        SMS_DELETE_MESSAGE  = 0,
        SMS_DELETE_ALL_1    = 1,
        SMS_DELETE_ALL_2    = 2,
        SMS_DELETE_ALL_3    = 3,
        SMS_DELETE_ALL_4    = 4,
    };

    //! HTTP method Enumeration
    enum HttpMethodEnumeration
    {
        HTTP_GET        = 0,
        HTTP_HEAD       = 1,
        HTTP_DELETE     = 2,
        HTTP_POST       = 3,
        HTTP_PUT        = 4,
        HTTP_POST_FRAME = 5,
    };
    
    //! TCP/UDP sockets protocol enumeration
    enum MultisocketProtocolEnumeration
    {
        TCP = 0,
        UDP = 1,
    };

    //! Security data actions enumeration
    enum SecurityDataActionsEnumeration
    {
        SSL_ACTION_DELETE   = 0,
        SSL_ACTION_STORE    = 1,
        SSL_ACTION_READ     = 2,
        SSL_EMAIL_DISABLED  = 0,
        SSL_EMAIL_ENABLES   = 1,
    };
    
    //! E-mail security enumeration
    enum EmailSecurityEnumeration
    {
        EMAIL_NONSSL    = 0,
        EMAIL_SSL       = 1,
    };

    //! Security data types
    enum SecurityDataTypesEnumeration
    {
        SSL_TYPE_CERT       = 0,
        SSL_TYPE_CA_CERT    = 1,
        SSL_TYPE_RSA        = 2,
    };


    //! class constructor
    /*!
    It initializes some variables
    \param void
    \return void
    */ 
    arduPi4G();
    
    //! This function inits the LE910 module
    /*!
    \return     0 if OK
                1 for no comunication
                2 if error switching CME errors to numeric response
                3 if error disabling the echo from the module
                4 if error enabling RTC update with network time
    */ 
    uint8_t ON();
    
    //! This function powers off the LE910 module
    /*!
    \return nothing
    */ 
    void OFF();
    
    //! This function enters a PIN / PUK code
    /*!
    \param char* code: string with the requested code
    \return     0 if OK
                1 if error
    */
    uint8_t enterPIN(char* code);
    
    //! This function enters a PIN / PUK code
    /*!
    \param char* code: string with the requested code
    \param char* new_code: string with the new code (only for SIM PUK or SIM PUK2)
    \return '0' if OK; '1' if error
    */
    uint8_t enterPIN(char* code, char* new_code);
    
    //! This function returns the result code for a PIN request
    /*!
    \return 
        0 for PIN ready
        1 LE910 is awaiting SIM PIN
        2 LE910 is awaiting SIM PUK
        3 LE910 is awaiting phone-to-SIM card password.
        4 LE910 is awaiting phone-to-very-first-SIM card password.
        5 LE910 is awaiting phone-to-very-first-SIM card unblocking password.
        6 LE910 is awaiting SIM PIN2
        7 LE910 is awaiting SIM PUK2
        8 LE910 is awaiting network personalization password
        9 LE910 is awaiting network personalization unblocking password
        10 LE910 is awaiting network subset personalization password
        11 LE910 is awaiting network subset personalization unblocking password
        12 LE910 is awaiting service provider personalization password
        13 LE910 is awaiting service provider personalization unblocking password
        14 LE910 is awaiting corporate personalization password
        15 LE910 is awaiting corporate personalization unblocking password
        254 if not expected code returned
        255 if command error
    */
    uint8_t checkPIN();
    
    //! This function checks connection status
    /*!
    \param  uint8_t time: max allowed time in seconds to connect
    \return 
        0 If the module is connected to the network
        1 not registered, ME is not currently searching for a new operator to register to
        2 not registered, but ME is currently searching for a new operator to register to
        3 registration denied
        4 unknown
    */
    uint8_t checkConnection(uint8_t time);

    uint8_t checkConnectionEPS(uint8_t time);
    
    /*!
    \brief  This function checks connection status and connect to data service
    \param  uint8_t time: max allowed time in seconds to connect
    \return 0 If the module is connected to data service
            1 not registered, ME is not currently searching for a new operator to register to
            2 not registered, but ME is currently searching for a new operator to register to
            3 registration denied
            4 unknown
            6 not registered, ME is not currently searching for a new operator to register to
            8 not registered, but ME is currently searching for a new operator to register to
            9 registration denied
            10 unknown
            12 if error setting APN
            13 if error setting login
            14 if error setting password
            15 if error activating GPRS connection
    */
    uint8_t checkDataConnection(uint8_t time);
    
    
    /*!
    \brief  This function manages incoming data from serial port
    \param  unsigned long wait_time: maximum waiting time in milliseconds
    \return 0 for error or not data
            1 for SMS
            2 for IP data
    */
    uint8_t manageIncomingData(unsigned long wait_time);
    uint8_t manageSocketData(unsigned long wait_time);
    
    //! This function sets the parameters to use SMS
    /*!
    \return     0 if OK
                1 if error setting the SMS format
                2 if error selecting the storage
                3 if error setting the incoming SMS indication
    */  
    uint8_t configureSMS();
    
    /*!
    \brief  This function reads the last unread received message
    \return 0 if OK
            1 if error
    */
    uint8_t readNewSMS();
    uint8_t readNewSMS(uint32_t timeout);

    //! This function sends a SMS
    /*!
    \param  char* phone_number: number to send the SMS
    \param  char* sms_string: body of the SMS
    \return     0 if OK
                1 if error setting the phone number
                2 if error sending the body
    */  
    uint8_t sendSMS(char* phone_number, char* sms_string);
    
    //! This function reads a SMS
    /*!
    \param  uint8_t sms_index: index of the SMS into the memory
    \return     0 if OK
                1 if error getting the SMS
                2 if error storing the message
    */  
    uint8_t readSMS(uint8_t sms_index);
    
    //! This function deletes a SMS
    /*!
    \param  uint8_t sms_index: index of the SMS into the memory
    \return     0 if OK
                1 if error deleting the SMS
    */  
    uint8_t deleteSMS(uint8_t sms_index);
    
    //! This function deletes a SMS
    /*!
    \param  uint8_t sms_index: index of the SMS into the memory
    \return 0 if OK
            1 if error deleting the SMS
    */  
    uint8_t deleteSMS(uint8_t sms_index, uint8_t del_flag);

    //! This function performs a HTTP request
    /*!
    \param  uint8_t method: selected HTTP method:   arduPi4G::HTTP_GET
                                                    arduPi4G::HTTP_HEAD
                                                    arduPi4G::HTTP_DELETE
    \param  char* url: host name or IP address of the server
    \param  uint16_t port: server port
    \param  char* resource: parameter indicating the HTTP resource, object of the request
    \return 0 if OK
            1 not registered, ME is not currently searching for a new operator to register to
            2 not registered, but ME is currently searching for a new operator to register to
            3 registration denied
            4 unknown
            6 not registered, ME is not currently searching for a new operator to register to
            8 not registered, but ME is currently searching for a new operator to register to
            9 registration denied
            10 unknown
            12 if error setting APN
            13 if error setting login
            14 if error setting password
            15 if error activating GPRS connection
            16 if error setting URL and port
            17 if error sending the request
            18 if error sending POST / PUT data
            19 if worng method has been selected
            20 if timeout waiting the URC 
            21 if error reading the URC
            22 if error reading the HTTP data
            23 if error reading the HTTP data
    */
    uint8_t http(uint8_t method,
                 char* url,
                 uint16_t port, 
                 char* resource);
    
    //! This function performs a HTTP request
    /*!
    \param  uint8_t method: selected HTTP method:   arduPi4G::HTTP_GET
                                                    arduPi4G::HTTP_HEAD
                                                    arduPi4G::HTTP_DELETE
                                                    arduPi4G::HTTP_POST
                                                    arduPi4G::HTTP_PUT
                                                    arduPi4G::HTTP_POST_FRAME
    \param  char* url: host name or IP address of the server
    \param  uint16_t port: server port
    \param  char* resource: parameter indicating the HTTP resource, object of the request
    \param  char* POST_PUT_data: data to send in POST/PUT method
    \param  uint16_t POST_PUT_length: data length to send in POST/PUT method
    \return 0 if OK
            1 not registered, ME is not currently searching for a new operator to register to
            2 not registered, but ME is currently searching for a new operator to register to
            3 registration denied
            4 unknown
            6 not registered, ME is not currently searching for a new operator to register to
            8 not registered, but ME is currently searching for a new operator to register to
            9 registration denied
            10 unknown
            12 if error setting APN
            13 if error setting login
            14 if error setting password
            15 if error activating GPRS connection
            16 if error setting URL and port
            17 if error sending the request
            18 if error sending POST / PUT data
            19 if worng method has been selected
            20 if timeout waiting the URC 
            21 if error reading the URC
            22 if error reading the HTTP data
            23 if error reading the HTTP data
    */  
    uint8_t http(uint8_t method,
                 char* url,
                 uint16_t port, 
                 char* resource,
                 char* data);
    
    //! This function configures FTP parameters and opens the connection
    /*!
    \param  uint8_t method: selected HTTP method:   arduPi4G::HTTP_GET
                                                    arduPi4G::HTTP_HEAD
                                                    arduPi4G::HTTP_DELETE
                                                    arduPi4G::HTTP_POST
                                                    arduPi4G::HTTP_PUT
                                                    arduPi4G::HTTP_POST_FRAME
    \param  char* server: address of FTP server
    \param  uint16_t port: port of FTP server
    \param  char* username: authentication user identification string for FTP
    \param  char* password: authentication password for FTP
    \return     0 if OK
                1 not registered, ME is not currently searching for a new operator to register to
                2 not registered, but ME is currently searching for a new operator to register to
                3 registration denied
                4 unknown
                6 not registered, ME is not currently searching for a new operator to register to
                8 not registered, but ME is currently searching for a new operator to register to
                9 registration denied
                10 unknown
                12 if error setting APN
                13 if error setting login
                14 if error setting password
                15 if error activating GPRS connection
                16 if error opening the FTP connection
                17 if error setting the transfer type
    */  
    uint8_t ftpOpenSession(char* server,
                           uint16_t port,
                           char* username,
                           char* password);    
    
    //! This function closes the FTP connection
    /*!
    \return     0 if OK
                1 if error
    */  
    uint8_t ftpCloseSession();  
    
    //! This function uses PUT to send a file to a FTP server
    /*!
    \param  char* FTP_file: destiny file
    \param  char* SD_file: origin file
    \return     0 if OK
                1 if error reading the file size
                2 if error openning the file
                3 if error setting the pointer of the file
                4 if error openning the PUT connection
                5 if error exiting from the data mode
                6 if error sending data
    */
    uint8_t ftpUpload(char* ftp_file, char* sd_file);
    
    //! This function uses GET to read a file from a FTP server
    /*!
    \param  char* SD_file: destiny file
    \param  char* FTP_file: origin file
    \return     0 if OK
                1 if error reading the file size
                2 if error going to the root directory in SD
                3 if error creating the file in SD
                4 if error opening the file
                5 if error setting the pointer of the file
                6 if error openning the GET connection
                7 if error reading the data
    */
    uint8_t ftpDownload(char* sd_file, char* ftp_file);
    
    //! This function reads the size of a file in a FTP server
    /*!
    \param  char* FTP_file: file
    \return 0 if OK
            1 if error
    */
    uint8_t ftpFileSize(char* ftp_file);
    
    
    //! This function configures and opens a socket
    /*!
    \param  uint8_t socketId: number of the socket Id
    \param  bool protocol: 0 for TCP_CLIENT and 1 for UDP_CLIENT
    \param  char* remote_IP: address of the remote host (IPv6 allowed)
    \param  uint16_t remote_port: remote host port to contact
    \return 0 if OK
            1 not registered, ME is not currently searching for a new operator to register to
            2 not registered, but ME is currently searching for a new operator to register to
            3 registration denied
            4 unknown
            6 not registered, ME is not currently searching for a new operator to register to
            8 not registered, but ME is currently searching for a new operator to register to
            9 registration denied
            10 unknown
            12 if error setting APN
            13 if error setting login
            14 if error setting password
            15 if error activating GPRS connection
            16 Socket with an active data transfer connection
            17 Socket suspended
            18 Socket suspended with pending data
            19 Socket listening
            20 Socket with an incoming connection. Waiting for the user accept or shutdown command
            21 Socket in opening process. The socket is not in Closed state but still not in Active or Suspended or Suspended with pending data state
            22 if error reading the status of the socket
            23 if error configuring of the socket
            24 if error configuring of the socket
            25 if error sending the open command
            26 if timeout openning the socket
    */  
    uint8_t openSocketClient(uint8_t socketId,
                            bool protocol,
                            char* remote_IP,
                            uint16_t remote_port);
    
    //! This function configures and opens a socket
    /*!
    \param  uint8_t socketId: number of the socket Id
    \param  bool protocol: 0 for TCP_CLIENT and 1 for UDP_CLIENT
    \param  char* remote_IP: address of the remote host (IPv6 allowed)
    \param  uint16_t remote_port: remote host port to contact
    \param  uint16_t local_port: UDP connections local port
    \return 0 if OK
            1 not registered, ME is not currently searching for a new operator to register to
            2 not registered, but ME is currently searching for a new operator to register to
            3 registration denied
            4 unknown
            6 not registered, ME is not currently searching for a new operator to register to
            8 not registered, but ME is currently searching for a new operator to register to
            9 registration denied
            10 unknown
            12 if error setting APN
            13 if error setting login
            14 if error setting password
            15 if error activating GPRS connection
            16 Socket with an active data transfer connection
            17 Socket suspended
            18 Socket suspended with pending data
            19 Socket listening
            20 Socket with an incoming connection. Waiting for the user accept or shutdown command
            21 Socket in opening process. The socket is not in Closed state but still not in Active or Suspended or Suspended with pending data state
            22 if error reading the status of the socket
            23 if error configuring of the socket
            24 if error configuring of the socket
            25 if error sending the open command
            26 if timeout openning the socket
    */  
    uint8_t openSocketClient(uint8_t socketId,
                            bool protocol,
                            char* remote_IP,
                            uint16_t remote_port,
                            uint16_t local_port);
    
    //! This function configures and opens a socket
    /*!
    \param  uint8_t socketId: number of the socket Id
    \param  bool protocol: 0 for TCP and 1 for UDP
    \param  char* remote_IP: address of the remote host (IPv6 allowed)
    \param  uint16_t remote_port: remote host port to contact
    \param  uint16_t local_port: UDP connections local port
    \param  bool keep_alive: 0 disable, 1 enable
    \param  uint8_t keep_alive: From 0 (disabled) to 240 minutes
    \return 0 if OK
            1 not registered, ME is not currently searching for a new operator to register to
            2 not registered, but ME is currently searching for a new operator to register to
            3 registration denied
            4 unknown
            6 not registered, ME is not currently searching for a new operator to register to
            8 not registered, but ME is currently searching for a new operator to register to
            9 registration denied
            10 unknown
            12 if error setting APN
            13 if error setting login
            14 if error setting password
            15 if error activating GPRS connection
            16 Socket with an active data transfer connection
            17 Socket suspended
            18 Socket suspended with pending data
            19 Socket listening
            20 Socket with an incoming connection. Waiting for the user accept or shutdown command
            21 Socket in opening process. The socket is not in Closed state but still not in Active or Suspended or Suspended with pending data state
            22 if error reading the status of the socket
            23 if error configuring of the socket
            24 if error configuring of the socket
            25 if error sending the open command
            26 if timeout openning the socket
    */  
    uint8_t openSocketClient(uint8_t socketId,
                            bool protocol,
                            char* remote_IP,
                            uint16_t remote_port,
                            uint16_t local_port,
                            uint8_t keep_alive);
    
    /*!
    \brief  This function opens a socket listening for an incoming connection on 
            a specified port
    \param  uint8_t socketId: number of the socket Id 
    \param  bool protocol: 0 for TCP_SERVER and 1 for UDP_SERVER
    \param  uint16_t local_port: TCP/UDP connections local port
    \param  keep_alive: From 0 (disabled) to 240 minutes
    \return     0 if OK
                1 not registered, ME is not currently searching for a new 
                    operator to register to
                2 not registered, but ME is currently searching for a new
                    operator to register to
                3 registration denied
                4 unknown
                6 not registered, ME is not currently searching for a new 
                    operator to register to
                8 not registered, but ME is currently searching for a new 
                    operator to register to
                9 registration denied
                10 unknown
                12 if error setting APN
                13 if error setting login
                14 if error setting password
                15 if error activating GPRS connection
                16 Socket with an active data transfer connection
                17 Socket suspended
                18 Socket suspended with pending data
                19 Socket listening
                20 Socket with an incoming connection. Waiting for the user 
                    accept or shutdown command
                21 Socket in opening process. The socket is not in Closed state 
                    but still not in Active or Suspended or Suspended with 
                    pending data state
                22 if error reading the status of the socket
                23 if error configuring of the socket
                24 if error configuring of the socket
                25 if error sending the open command
                26 if timeout openning the socket
    */  
    uint8_t openSocketServer(uint8_t socketId,
                            bool protocol,
                            uint16_t local_port);
                            
    uint8_t openSocketServer(uint8_t socketId,
                            bool protocol,
                            uint16_t local_port,
                            uint8_t keep_alive);
    
       
    //! This function configures and opens a socket SSL
    /*!
    \param  uint8_t socketId: number of the socket Id
    \param  char* remote_IP: address of the remote host (IPv6 allowed)
    \param  uint16_t remote_port: remote host port to contact
    \return 0 if OK; 'x' if error
    */  
    uint8_t openSocketSSL(uint8_t socketId,
                          char* remote_IP,
                          uint16_t remote_port);
    
    //! This function closes a socket previously opened
    /*!
    \param  uint8_t socketId: number of the socket Id
    \return     0 Socket Closed
                1 if error sending the command
                2 if timeout shutting down the socket
    */  
    uint8_t closeSocketClient(uint8_t socketId);
    uint8_t closeSocketServer(uint8_t socketId, uint8_t protocol);
    
    //! This function closes a SSL socket previously opened
    /*!
    \param  uint8_t socketId: number of the socket Id
    \return     0 Socket Closed
                1 if error sending the command
                2 if timeout shutting down the socket
    */  
    uint8_t closeSocketSSL(uint8_t socketId);
    
    //! This function gets the status of a TCP or UDP socket
    /*!
    \param  uint8_t socketId: number of the socket Id
    \return 0 if OK; 1 if error
    */  
    uint8_t getSocketStatus(uint8_t socketId);
    uint8_t getAllSocketStatus();
    uint8_t getSocketStatusSSL(uint8_t socketId);
    
    /*! This function manages the TCP/UDP socket profiles
    *   If there is an incoming connection, it is accepted  
    */
    uint8_t manageSockets();
    uint8_t manageSockets(unsigned long wait_time);
    
    //! This function gets info from a TCP or UDP socket
    /*!
    \param  uint8_t socketId: number of the socket Id
    \return 0 if ok; 1 if error
    */  
    uint8_t getSocketInfo(uint8_t socketId);
    
    
    //! This function sends data through a socket previously opened
    /*!
    \param  uint8_t socketId: number of the socket Id
    \param  uint8_t* data: string with the data
    \return     0 if OK
                1 if error with the socket
                2 if error sending the command
                3 if error sending the command
                4 if timeout shutting down the socket
    */  
    uint8_t send(uint8_t socketId, char* data);
    
    //! This function sends data through a socket previously opened
    /*!
    \param  uint8_t socketId: number of the socket Id
    \param  uint8_t* data: buffer with the data
    \param  uint16_t data_length: length of the data buffer
    \return     0 if OK
                1 if error with the socket
                2 if error sending the command
                3 if error sending the command
                4 if timeout shutting down the socket
    */  
    uint8_t send(uint8_t socketId, uint8_t* data, uint16_t data_length);
            
            
    //! This function sends data through a SSL socket previously opened
    /*!
    \param  uint8_t socketId: number of the socket Id
    \param  uint8_t* data: string with the data
    \return 0 if OK, 'x' if error
    */  
    uint8_t sendSSL(uint8_t socketId, char* data);  
    
    //! This function sends data through a SSL socket previously opened
    /*!
    \param  uint8_t socketId: number of the socket Id
    \param  uint8_t* data: buffer with the data
    \param  uint16_t data_length: length of the data buffer
    \return 0 if OK, 'x' if error
    */  
    uint8_t sendSSL(uint8_t socketId, uint8_t* data, uint16_t data_length);
    
    //! This function read data received in the module
    /*!
    \param  uint8_t socketId: number of the socket Id
    \return '0' if ok; 'x' if error
    */  
    uint8_t receive(uint8_t socketId);
    
    //! This function read data received in the module
    /*!
    \param  uint8_t socketId: number of the socket Id
    \param  uint32_t timeout: timeout in ms to wait for response
    \return '0' if ok; 'x' if error
    */  
    uint8_t receive(uint8_t socketId, uint32_t timeout);
    
        
    //! This function reads data received in the module through SSL socket
    /*!
    \param  uint8_t socketId: number of the socket Id
    \return '0' if ok; 'x' if error
    */  
    uint8_t receiveSSL(uint8_t socketId);
    
    //! This function reads data received in the module through SSL socket
    /*!
    \param  uint8_t socketId: number of the socket Id
    \param  uint32_t timeout: timeout in ms to wait for response
    \return '0' if ok; 'x' if error
    */  
    uint8_t receiveSSL(uint8_t socketId, uint32_t timeout);
    
    
    
    /*! This function allows to store, delete and read security data 
     * (Certificate, CAcertificate, private key) into the non-volatile memory of 
     * the module
     * 
     * \param socketId: Secure Socket Identifier (must be 1)
     * \param action: Action to do:
     *                  0 – Delete data from NVM
     *                  1 – Store data into NVM
     *                  2 – Read data from NVM
     * \param dataType:
     *                  0 – Certificate
     *                  1 – CA certificate
     *                  2 – RSA Private key
     * \param data: string with the security data
     * 
     * \return  0 if OK; 1 if error
     */
    uint8_t manageSSL(uint8_t socketId, 
                      uint8_t action, 
                      uint8_t dataType);
                        
    uint8_t manageSSL(uint8_t socketId, 
                      uint8_t action, 
                      uint8_t dataType,
                      char *data);
    
    //! Latitude
    char _latitude[11];
    
    //! Variable : it stores the latitude indicator N: North; S: South
    char _latitudeNS;       //NS_indicator  

    char _longitude[12];    //longitude     
    
    //! Variable : it stores the longitude indicator E: East; W: West
    char _longitudeEW;      //EW_indicator

    float _altitude;        // altitude 
    char _time[7];          // hhmmss
    char _date[7];          // ddmmyy
    uint8_t _numSatellites; // satellites in use
    uint8_t _fixMode;       // satellites in use
    float _speedOG;         // speed over ground (Km/hr)
    char _courseOG[7];      // course over ground ddd.mm
    float _hdop;            // Horizontal Diluition of Precision
    
    /*!
    \brief Starts the GPS engine with hot start and stand alone mode
    \return 0 if OK
            1 if GPS is powered yet
            2 if error setting the reset mode
            3 if error starting the GPS engine
     */
    uint8_t gpsStart();
    
    /*!
    \brief Starts the GPS engine with hot start and stand alone mode
    \param uint8_t gps_mode: 
            0: Pure MS Assisted - Location estimate from the network (MS Assisted mode).
            1: MS Based - Assistance Data from the network (MS Based mode).
            2: MS Assisted Based - Combination of MS-A and MS-B modes, location 
            estimate computed both at UE and Network.
            3: Autonomous – Autonomous GPS mode of operation.
    \return 0 if OK
            1 if GPS is powered yet
            2 if error setting the reset mode
            3 if error starting the GPS engine
     */
    uint8_t gpsStart(uint8_t gps_mode);
    
    /*!
    \brief Starts the GPS engine with hot start and stand alone mode
    \param uint8_t gps_mode: 
            0: Pure MS Assisted - Location estimate from the network (MS Assisted mode).
            1: MS Based - Assistance Data from the network (MS Based mode).
            2: MS Assisted Based - Combination of MS-A and MS-B modes, location 
            estimate computed both at UE and Network.
            3: Autonomous – Autonomous GPS mode of operation.
    \param uint8_t reset_mode: 
            1 Coldstart
            2 Warmstart
            3 Hotstart
    \return 0 if OK
            1 if GPS is powered yet
            2 if error setting the reset mode
            3 if error starting the GPS engine
     */
    uint8_t gpsStart(uint8_t gps_mode, uint8_t reset_mode);
    

    /*!
    \brief Stops the GPS engine
    \return 0 if OK; 1 if error
     */
    uint8_t gpsStop();
    
    /*!
    \brief Checks GPS signal
    \return '1' if GPS status is unknown
        '2' if GPS status is not fixed
        '3' if GPS status is fixed in 2D mode
        '4' if GPS status is fixed in 3D mode
     */
    int8_t checkGPS();
    
    //! It converts from the NMEA message and indicator to degrees
    float convert2Degrees(char* input, char indicator);
    
    
    /*!
    \brief It checks if receiver is connected to some satellite until time out
    \return '1' if connected, 
            '0' if not
    */
    uint8_t waitForSignal();
        
    /*!
    \brief It checks if receiver is connected to some satellite until time out
    \param long timeout: timeout to fix the satellites defined in seconds
    \return '1' if connected, 
            '0' if not
    */
    uint8_t waitForSignal(uint32_t timeout);
    
    
    /*!
    \brief It checks if receiver is connected to some satellite until time out
    \param long timeout: timeout to fix the satellites defined in seconds
    \param float desired_HDOP: Desired HDOP accuracy to wait for signal
    \return '1' if connected, 
            '0' if not
    */
    uint8_t waitForSignal(uint32_t timeout, float desired_HDOP);
    
    bool getNMEAString(uint8_t NMEA);
    
    //! It sets the qualiti of service of GPS
    uint8_t gpsSetQualityOfService( uint32_t horiz_accuracy, 
                                    uint16_t vertic_accuracy,
                                    uint16_t rsp_time,
                                    uint32_t age_of_location_info,
                                    uint8_t location_type,
                                    uint8_t nav_profile,
                                    uint8_t velocity_request);
    
    
    //! This function gets the temperature interval or the temperature value
    /*!
    \return     If interval mode selected
                -2 Extreme temperature lower bound.
                -1 Operating temperature lower bound.
                0 normal temperature.
                1 Operating temperature upper bound.
                2 Extreme temperature upper bound.
                -1000 if error
                
                If temperature value selected return temperature in Celsius
                -1000 if error
    */
    uint8_t getTemp();
    
    //! This function gets the RSSI level
    /*!
    \return the value in dBm
            0 if error
    */
    uint8_t getRSSI();
    
    //! This function gets the packet service network type
    /*!
    \return     0 for GPRS network
                1 for EGPRS network
                2 for WCDMA network
                3 for HSDPA network
                4 for LTE network
                5 for unknown or not registered
                -1 if error
    */
    uint8_t getNetworkType();
    
    //! This function gets the Operator name
    uint8_t getOperator(char* answer_storage);
    
    /*! This function selects the Wireless Network 
    \param uint8_t n:  it is the WDS-Side Stack used by the TA:
        arduPi4G::NETWORK_GSM
        arduPi4G::NETWORK_UTRAN
        arduPi4G::NETWORK_3GPP
        arduPi4G::NETWORK_EUTRAN_ONLY
        arduPi4G::NETWORK_GERAN_UTRAN
        arduPi4G::NETWORK_GERAN_EUTRAN
        arduPi4G::NETWORK_UTRAN_EUTRAN
     */
    uint8_t setWirelessNetwork(uint8_t n);
    //! This function gets info from the module
    /*!
    \param  uint8_t info_req: 
        arduPi4G::INFO_HW
        arduPi4G::INFO_MANUFACTURER_ID
        arduPi4G::INFO_MODEL_ID
        arduPi4G::INFO_REV_ID
        arduPi4G::INFO_IMEI 
        arduPi4G::INFO_IMSI 
        arduPi4G::INFO_ICCID 
    \return     0 if OK
                1 if error
    */
    int8_t getInfo(uint8_t info_req);
    
    //! This function sets the RTC time from 4G network info
    /*!
    \return '0' if OK; '1' if error
     */
    uint8_t setTimeFrom4G();

    //! This function shows the RTC time from 4G network info
    /*!
    \return '0' if OK; '1' if error
     */
    uint8_t showTimeFrom4G();

    //! This function sets the apn from operator
    /*!
    \param char* apn: apn from operator
    \return void
     */
    void set_APN( char* apn);
    
    //! This function sets the apn, login and password from operator
    /*!
    \param char* apn: apn from operator
    \param char* login: login from operator
    \param char* password: password from operator
    \return void
     */
    void set_APN( char* apn, char* login, char* password);
    
    //! This function shows the apn, login and password constants
    /*!
    \return '1' on success, '0' if error
     */ 
    void show_APN();
    
    void parseLatitude(char* lat_str);
    
    void parseLongitude(char* lon_str);
    
    uint8_t ftpListFolder(char* ftp_folder);
    
    void printErrorCode();
    void printErrorCode(uint16_t error);
    
    uint8_t emailSetServerSMTP(char* address);
    uint8_t emailSetSender(char* address, char* user, char* password);
    uint8_t emailActivateContext(char* user, char* password);
    uint8_t emailConfigureSMTP(uint8_t security, uint16_t port);
    uint8_t emailSend(char* address,char* subject,char* body);
    uint8_t emailReset();
    uint8_t emailSave();
};

extern arduPi4G _4G;

#endif
