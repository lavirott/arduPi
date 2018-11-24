/*! \file arduPi4G.cpp
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
 *  Version:           1.3
 *  Design:            David Gascón
 *  Implementation:    Alejandro Gállego, Yuri Carmona, Luis Miguel Marti
 *  Port to Raspberry: Ruben Martin
 */

#ifndef __WPROGRAM_H__
#include "../../arduPi-api/arduPiClasses.h"
#endif

#include "arduPi4G.h"
#include <inttypes.h>

#include <stdio.h>
#include <time.h>
#include <string.h>

#define pgm_read_word(addr) (*(const intptr_t*)(addr))

//! class constructor
arduPi4G::arduPi4G()
{
    memset(_apn, '\0', sizeof(_apn));
    memset(_apn_login, '\0', sizeof(_apn_login));
    memset(_apn_password, '\0', sizeof(_apn_password));
    strncpy(_apn, LE910_GPRS_APN, min(sizeof(_apn), strlen(LE910_GPRS_APN)));
    strncpy(_apn_login, LE910_GPRS_LOGIN, min(sizeof(_apn_login), strlen(LE910_GPRS_LOGIN)));
    strncpy(_apn_password, LE910_GPRS_PASSW, min(sizeof(_apn_password), strlen(LE910_GPRS_PASSW)));
}


// Private Methods ////////////////////////////////////////////////////////////

/*
 * Function: This function parses the error copde returned by the module. At the
 * point this function is called, the UART is supposed to have received:
 * "+CME ERROR: <err>\r\n" and the first part of the response has been already
 * detected: "+CME ERROR:", so this function needs to parse the error code
 *
 * If error code is parsed succesfully then the attribute _errorCode stores
 * this error information
 *
 * @return  '0' if ok; '1' if error
 */
uint8_t arduPi4G::getErrorCode()
{
    uint8_t status;
    char format[20];

    // wait for " <err>\r\n"
    status = waitFor((char*)"\r\n", 3000);

    if (status == 0)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("no response\n");
        #endif
        return 1;
    }
    else
    {
        parseUint32((uint32_t*)&_errorCode, (char*)" \r\n");

        #if DEBUG_ARDUPI4G > 0
            printErrorCode( _errorCode );
        #endif
        return 0;
    }

}


/* Function:    This function checks connection status and connect to data service
 * Parameters:  time: max allowed time in seconds to connect
 * Return:  0 If the module is connected to data service
 *          1 not registered, ME is not currently searching for a new operator to register to
 *          2 not registered, but ME is currently searching for a new operator to register to
 *          3 registration denied
 *          4 unknown
 *          6 not registered, ME is not currently searching for a new operator to register to
 *          8 not registered, but ME is currently searching for a new operator to register to
 *          9 registration denied
 *          10 unknown
 *          12 if error setting APN
 *          13 if error setting login
 *          14 if error setting password
 *          15 if error activating GPRS connection
 */
uint8_t arduPi4G::checkDataConnection(uint8_t time)
{

    uint8_t answer;
    uint32_t previous, max_time;

    char command_name[20];
    char buffer1[40];
    char buffer2[20];
    char buffer3[20];

    answer = 0;
    max_time = (unsigned long)time * 1000;
    previous = millis();

    //// 1. Check if the module has the GPRS connection active
    strcpy(buffer1, (char*)pgm_read_word(&(table_4G[26]))); // "AT#GPRS?\r"
    strcpy(buffer2, (char*)pgm_read_word(&(table_4G[27]))); // "GPRS: 1"
    strcpy(buffer3, (char*)pgm_read_word(&(table_4G[28]))); // "GPRS: 0"

    // send command
    answer = sendCommand(buffer1, buffer2, buffer3, 2000);
    if (answer == 1)
    {
        #if DEBUG_ARDUPI4G > 1
            printf("Connection active, nothing more to check\n");
        #endif
        return 0;
    }

    //// 2. Check Network Registration Report
    strcpy(buffer1, (char*)pgm_read_word(&(table_4G[0])));    //AT+CREG?\r
    strcpy(buffer2, (char*)pgm_read_word(&(table_4G[1])));    //+CREG: 0,
    do{
        // Sends the command and waits for the answer (0,1 for home network and 0,5 for roaming)
        answer = sendCommand(buffer1, buffer2, 2000);

        if (answer == 1)
        {
            answer = serialRead(UART0) - 0x30;
        }
        delay(2000);
        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();
    }while (((answer == 2) || (answer == 4) || (answer == 3) || (answer == 0)) && ((millis() - previous) < max_time));

    if (((answer != 1) && (answer != 5)) || ((millis() - previous) > max_time))
    {
        if (answer == 0)
        {
            return answer + 1;
        }
        return answer;
    }

    //// 3. Check GPRS Network Registration Status
    strcpy(buffer1, (char*)pgm_read_word(&(table_4G[2])));    //AT+CGREG?\r
    strcpy(buffer2, (char*)pgm_read_word(&(table_4G[3])));    //+CGREG: 0,

    do{
        // Sends the command and waits for the answer (0,1 for home network and 0,5 for roaming)
        answer = sendCommand(buffer1, buffer2, 2000);
        if (answer == 1)
        {
            answer = serialRead(UART0) - 0x30;
        }
        delay(2000);
        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();
    }while (((answer == 2) || (answer == 4) || (answer == 0)) && ((millis() - previous) < max_time));

    if (((answer != 1) && (answer != 5)) || ((millis() - previous) > max_time))
    {
        return answer + 6;
    }

    //// 4. Define PDP Context
    // AT+CGDCONT=1,"IP","<APN>"\r
    sprintf(buffer1, (char*)pgm_read_word(&(table_4G[29])), _apn); //"AT+CGDCONT=1,\"IP\",\"%s\"\r"
    answer = sendCommand(buffer1, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);
    if (answer != 1)
    {
        return 12;
    }

    //// 5. Set Authentication User ID
    // AT#USERID="<login>"\r
    sprintf(buffer1, (char*)pgm_read_word(&(table_4G[5])), _apn_login);
    answer = sendCommand(buffer1, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);
    if (answer != 1)
    {
        return 13;
    }

    //// 6. Set Authentication Password
    // AT#PASSW="<pass>"\r
    sprintf(buffer1, (char*)pgm_read_word(&(table_4G[6])), _apn_password);
    answer = sendCommand(buffer1, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);
    if (answer != 1)
    {
        return 14;
    }

/* oJo cambio por lo de yuri
    //// 7. GPRS Context Activation
    // "AT#GPRS=1\r"

    strcpy(buffer1, (char*)pgm_read_word(&(table_4G[31]))); // "AT#GPRS=1\r"
    strcpy(buffer2, (char*)pgm_read_word(&(table_4G[39]))); // "+IP: "

    answer = sendCommand(buffer1, buffer2, LE910_ERROR_CODE, LE910_ERROR, 15000);

    if (answer != 1)
    {
        return 15;
    }

    // delimiters <--- "+IP: \"\r\n"
    strcpy(buffer1, (char*)pgm_read_word(&(table_4G[32])));

    // get IP address in response
    answer = parseString(_ip, sizeof(_ip), buffer1);

    if (answer != 0)
    {
        return 16;
    }

    return 0;
*/

    //// 7. GPRS Context Activation
    // "AT#GPRS=1\r"
    strcpy(buffer1, (char*)pgm_read_word(&(table_4G[31])));
    answer = sendCommand(buffer1, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 15000);
    if (answer != 1)
    {
        return 15;
    }

    // delimiters <--- "+IP: \"\r\n"
    strcpy(buffer1, (char*)pgm_read_word(&(table_4G[32])));

    // get IP address in response
    answer = parseString(_ip, sizeof(_ip), buffer1);

    if (answer != 0)
    {
        return 16;
    }

    return 0;

}


/* This function configures the remote server and sends the request
 * Parameters;  method: selected HTTP method:   GET_METHOD
 *                                              HEAD_METHOD
 *                                              DELETE_METHOD
 *                                              POST_METHOD
 *                                              PUT_METHOD
 *              url: host name or IP address of the server
 *              port: server port
 *              resource: parameter indicating the HTTP resource, object of the
 *                  request
 *              data: data to send in POST/PUT method
 *              data_length: data length to send in POST/PUT method
 *  Return: 0 if OK
 *          1 if error setting URL and port
 *          2 if error sending the request
 *          3 if error sending POST / PUT data
 *          4 if worng method has been selected
 */
uint8_t arduPi4G::httpRequest(uint8_t method,
                              char* url,
                              uint16_t port,
                              char* resource,
                              char* data)
{

    return httpRequest( method,
                        url, 
                        port, 
                        resource, 
                        (uint8_t*)data, 
                        strlen(data));
}


/* This function configures the remote server and sends the request
 * Parameters;  method: selected HTTP method:   GET_METHOD
 *                                              HEAD_METHOD
 *                                              DELETE_METHOD
 *                                              POST_METHOD
 *                                              PUT_METHOD
 *              url: host name or IP address of the server
 *              port: server port
 *              resource: parameter indicating the HTTP resource, object of the
 *                  request
 *              data: data to send in POST/PUT method
 *              data_length: data length to send in POST/PUT method
 *  Return: 0 if OK
 *          1 if error setting URL and port
 *          2 if error sending the request
 *          3 if error sending POST / PUT data
 *          4 if worng method has been selected
 */
uint8_t arduPi4G::httpRequest(uint8_t method,
                              char* url,
                              uint16_t port,
                              char* resource,
                              uint8_t* data,
                              uint16_t length)
{
    uint8_t answer;
    char command_name[20];
    char command_buffer[500];
    char aux[3];
    memset( aux, 0x00, sizeof(aux) );

    // Step1: Configure HTTP parameters
    // Generate: AT#HTTPCFG=0,"<url>",<port>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_HTTP[0])), url, port);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR, LE910_ERROR_CODE, 2000);

    if (answer == 2)
    {
        _errorCode = ARDUPI4G_ERROR_MESSAGE;
        return 1;
    }
    else if (answer == 3)
    {
        getErrorCode();
        return 1;
    }
    else if (answer == 0)
    {
        // timeout
        _errorCode = ARDUPI4G_ERROR_TIMEOUT;
        return 1;
    }


    // Step2: Perform the request depending on the method selected as input
    // in the function: GET, HEAD, DELETE, POST or PUT
    if ((method == arduPi4G::HTTP_GET) ||
        (method == arduPi4G::HTTP_HEAD) ||
        (method == arduPi4G::HTTP_DELETE))
    {
        // AT#HTTPQRY=0,<method>,"<resource>"\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_HTTP[1])), method, resource);

        // send command
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR, 5000);

        if (answer == 1)
        {
            return 0;
        }
        else
        {
            return 2;
        }
    }
    else if ((method == arduPi4G::HTTP_POST) ||
             (method == arduPi4G::HTTP_PUT))
    {
        // 2a. Send HTTP POST or PUT request
        // AT#HTTPSND=0,<method>,"<resource>",<data_length>
        sprintf(command_buffer, (char*)pgm_read_word(&(table_HTTP[2])),
                method - 3,
                resource,
                length);

        // send command
        answer = sendCommand(command_buffer, LE910_DATA_TO_MODULE, LE910_ERROR, 5000);
        if (answer != 1)
        {
            return 2;
        }

        // 2b. Send POST/PUT data
        answer = sendCommand((char*)data, LE910_OK ,LE910_ERROR, 5000);
        if (answer != 1)
        {
            return 3;
        }

        return 0;
    }
    else if (method == arduPi4G::HTTP_POST_FRAME)
    {
        char php_file[27];

        // 2a. Perform the request
        // "/getpost_frame_parser.php"
        strcpy(php_file, (char*)pgm_read_word(&(table_HTTP[5])));

        // AT#HTTPSND=0,0,"<php_file>",<data_length>
        sprintf(command_buffer, (char*)pgm_read_word(&(table_HTTP[2])),
                0,
                php_file,
                6 + (length * 2));

        answer = sendCommand(command_buffer, LE910_DATA_TO_MODULE, LE910_ERROR, 5000);
        if (answer != 1)
        {
            return 2;
        }

        // Add "frame="
        strcpy(command_buffer, (char*)pgm_read_word(&(table_HTTP[6])));
        printString(command_buffer, UART0);

        // Add frame contents in ASCII representation: 3C3D3E...
        for(uint16_t x = 0; x < length; x++)
        {
            // make conversion from byte to hex representation in ASCII (2Bytes)
            Utils.hex2str((uint8_t*)&data[x], aux, 1);
            printByte(aux[0], UART0);
            printByte(aux[1], UART0);
        }


        // 2b. Send POST/PUT data
        answer = waitFor(LE910_OK ,LE910_ERROR, 5000);
        if (answer != 1)
        {
            return 3;
        }

        return 0;

    }

    // Wrong method
    return 4;

}


/* This function waits the URC code and reads the data availble
 * Parameters:  wait_timeout: timeout for URC *
 *  Return: 0 if OK
 *          1 if timeout waiting the URC
 *          2 if error reading the URC
 *          3 if error reading the HTTP data
 *          4 if error reading the HTTP data
 */
uint8_t arduPi4G::httpWaitResponse(uint32_t wait_timeout)
{
    char *pointer;
    uint8_t answer;
    uint16_t data_size;
    char command_buffer[50];

    // 1. Wait URC: "#HTTPRING: 0,"
    strcpy(command_buffer, (char*)pgm_read_word(&(table_HTTP[3])));

    answer = waitFor(command_buffer, wait_timeout);
    if (answer == 0)
    {
        return 1;
    }

    // 2. Read the whole response: "#HTTPRING: 0,<http_status_code>,<content_type>,<data_size>\r
    answer = waitFor((char*)"\r", 5000);
    if (answer == 0)
    {
        return 2;
    }

    // 3. Read <http_status_code>
    pointer = strtok((char*)_buffer, ",");

    if (pointer == NULL)
    {
        return 3;
    }
    else
    {
        // Store the HTTP code in attribute
        _httpCode = atoi(pointer);
    }


    // 4. Skip <content_type>
    strtok(NULL, ",");


    // 5. Read <data_size>
    pointer = strtok((char*)_buffer, ",");

    if (pointer == NULL)
    {
        return 5;
    }
    else
    {
        // Store the HTTP code in attribute
        data_size = atoi(pointer);
    }

    // 6. Read data received
    if (data_size > 0)
    {
        // AT#HTTPRCV=0,0\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_HTTP[4])), 0, 0);

        // send command
        answer = sendCommand(command_buffer, LE910_DATA_FROM_MODULE, LE910_ERROR, 2000);

        // check answer
        if (answer == 1)
        {
            // Read the data
            answer = waitFor((char*)"\r", 5000);
            if (answer == 0)
            {
                return 4;
            }

            // Add end of string in _buffer
            _buffer[_length] = '\0';

            return 0;
        }
        else if (answer == 2)
        {
            return 6;
        }
        else
        {
            // Timeout
            return 7;
        }
    }

    return 8;

}


/* Function:    This function reads the size of a file in a FTP server
 * Parameters:  ftp_file: file
 * Return:  0 if "ok"
 *          1 if error
 */
uint8_t arduPi4G::ftpFileSize( char* ftp_file)
{
    uint8_t answer;
    char *pointer;
    char command_buffer[50];

    // init variable
    _filesize = 0;

    // AT#FTPFSIZE=<ftp_file>\r
    memset(command_buffer,0x00,sizeof(command_buffer));
    sprintf(command_buffer,(char*)pgm_read_word(&(table_FTP[5])),ftp_file);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 15000);

    #if DEBUG_ARDUPI4G > 0
        printf("_buffer:");
        printf((char*)_buffer);
    #endif

    if (answer != 1)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error getting filesize\n");
        #endif

        if (answer == 2)
        {
            getErrorCode();
            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }
        return 1;
    }

    // parse file size
    char delimiters[50];
    memset(delimiters, 0x00, sizeof(delimiters));
    strcat(delimiters, "#FTPFSIZE: ");
    strcat(delimiters, "\r\n");

    answer = parseUint32(&_filesize, delimiters);

    if (answer == 0)
    {
        return 0;
    }

    return 1;
}


/* Function:    This function gets the status of a TCP or UDP socket
 * Parameters:  socketId: number of the socket Id
 * Return:      0: ok; 1: error
 */
uint8_t arduPi4G::getSocketStatus(uint8_t socketId)
{
    uint8_t answer;
    char delimiters[20];
    char command_name[20];
    char command_buffer[20];
    char *pointer;

    // clear structure
    socketStatus[socketId].id = 0;
    socketStatus[socketId].state = 0;
    socketStatus[socketId].localPort = 0;
    socketStatus[socketId].remotePort = 0;
    memset(socketStatus[socketId].localIp, 0x00, sizeof(socketStatus[socketId].localIp));
    memset(socketStatus[socketId].remoteIp, 0x00, sizeof(socketStatus[socketId].remoteIp));

    // AT#SS=<socketId>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[0])), socketId+1);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

    // check answer
    if(answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();

            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }

        #if DEBUG_ARDUPI4G > 0
            printf("Error getting socket status\n");
        #endif
        return 1;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("_buffer:");
        printf((char*)_buffer);
    #endif

    // delimiters <-- "#SS: "
    strcpy(delimiters, (char*)pgm_read_word(&(table_IP[20])));

    // seek pattern in module response
    // the response is something similar to this:
    // "#SS: <id>,<status>,<localIp>,<lPort>,<remoteIp>,<rPort>\r\n\r\nOK\r\n"
    //  #SS: 1,2,100.97.99.163,32770,213.97.9.230,15010
    pointer = strstr((char*)_buffer, delimiters);

    if (pointer == NULL)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error: null pointer1\n");
        #endif
        return 1;
    }

    // delimiters <-- "#SS: ,\r\n"
    strcpy(delimiters, (char*)pgm_read_word(&(table_IP[21])));

    // find first value skipping delimiters
    pointer = strtok(pointer, delimiters);

    // iterate through response
    for (int i = 0; i < 6; i++)
    {
        if (pointer == NULL)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error: null pointer2\n");
            #endif
            return 1;
        }

        if (i == 0)
        {
            socketStatus[socketId].id = (uint8_t) strtoul( pointer, NULL, 10);
        }
        else if (i == 1)
        {
            socketStatus[socketId].state = (uint8_t) strtoul( pointer, NULL, 10);

            // if state is closed then break
            if (socketStatus[socketId].state == 0)
            {
                break;
            }
        }
        else if (i == 2)
        {
            strncpy(socketStatus[socketId].localIp, pointer, sizeof(socketStatus[socketId].localIp));
        }
        else if (i == 3)
        {
            socketStatus[socketId].localPort = (uint16_t) strtoul( pointer, NULL, 10);
        }
        else if (i == 4)
        {
            strncpy(socketStatus[socketId].remoteIp, pointer, sizeof(socketStatus[socketId].remoteIp));
        }
        else if (i == 5)
        {
            socketStatus[socketId].remotePort = (uint16_t) strtoul( pointer, NULL, 10);
            break;
        }

        pointer = strtok (NULL, " ,\r\n");
    }

    #if DEBUG_ARDUPI4G > 1
        printf("socketStatus.id: %d\n", socketStatus[socketId].id);
        printf("socketStatus.state: %d\n", socketStatus[socketId].state);
        if (socketStatus[socketId].state != 0)
        {
            printf("socketStatus.localIp: %s\n", socketStatus[socketId].localIp);
            printf("socketStatus.localPort: %d\n", socketStatus[socketId].localPort, DEC);
            printf("socketStatus.remoteIp: %s\n", socketStatus[socketId].remoteIp);
            printf("socketStatus.remotePort: %d\n", socketStatus[socketId].remotePort, DEC);
        }
    #endif

    return 0;

}


/* Function:    This function gets the status of all TCP/UDP socket from
 *              SOCKET_1 to SOCKET_6
 *
 * Return:      0: ok; 1: error
 */
uint8_t arduPi4G::getAllSocketStatus()
{
    uint8_t answer;
    char command_pattern[20];
    char command_buffer[20];
    char *pointer;

    // clear structure
    memset(socketStatus, 0x00, sizeof(socketStatus));

    // "AT#SS\r"
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[31])));

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();

            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }

        #if DEBUG_ARDUPI4G > 0
            printf("Error getting socket status\n");
        #endif
        return 1;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("_buffer: %s\n",(char*)_buffer);
    #endif

    // iterate through all profile from 1 to 6
    for (int profile = 0; profile < 6; profile++)
    {
        // "#SS: %u,"
        sprintf(command_pattern, (char*)pgm_read_word(&(table_IP[32])), profile+1);

        // seek start of correct index
        pointer = strstr((char*)_buffer, command_pattern);

        // delimiters <-- "#SS: ,\r\n"
        strcpy(command_pattern, (char*)pgm_read_word(&(table_IP[21])));

        // find first value skipping delimiters
        pointer = strtok(pointer, command_pattern);

        // iterate through response
        for (int i = 0; i < 6; i++)
        {
            if (pointer == NULL)
            {
                #if DEBUG_ARDUPI4G > 0
                    printf("Error: null pointer2\n");
                #endif
                return 1;
            }

            if (i == 0)
            {
                socketStatus[profile].id = (uint8_t) strtoul( pointer, NULL, 10);
            }
            else if (i == 1)
            {
                socketStatus[profile].state = (uint8_t) strtoul( pointer, NULL, 10);

                // if state is closed then break
                if (socketStatus[profile].state == 0)
                {
                    break;
                }
            }
            else if (i == 2)
            {
                strncpy(socketStatus[profile].localIp, pointer, sizeof(socketStatus[profile].localIp));
            }
            else if (i == 3)
            {
                socketStatus[profile].localPort = (uint16_t) strtoul( pointer, NULL, 10);
            }
            else if (i == 4)
            {
                strncpy(socketStatus[profile].remoteIp, pointer, sizeof(socketStatus[profile].remoteIp));
            }
            else if (i == 5)
            {
                socketStatus[profile].remotePort = (uint16_t) strtoul( pointer, NULL, 10);
                break;
            }

            pointer = strtok (NULL, " ,\r\n");
        }
    }

    #if DEBUG_ARDUPI4G > 1
        for (int i = 0; i < 6; i++)
        {
            printf("socketStatus.id: %d\n", socketStatus[i].id);
            printf("socketStatus.state: %d\n", socketStatus[i].state);
            if (socketStatus[i].state != 0)
            {
                printf("socketStatus.localIp: %s\n", socketStatus[i].localIp);
                printf("socketStatus.localPort: %d\n", socketStatus[i].localPort);
                printf("socketStatus.remoteIp: %s\n", socketStatus[i].remoteIp);
                printf("socketStatus.remotePort: %d\n", socketStatus[i].remotePort);
            }
            printf("---------------------------\n");
        }
    #endif

    return 0;

}


/* Function:    This function gets the status of a TCP or UDP socket
 * Parameters:  socketId: number of the socket Id
 * Return:      0: ok; 1: error
 */
uint8_t arduPi4G::getSocketStatusSSL(uint8_t socketId)
{
    uint8_t answer;
    char delimiters[20];
    char command_name[20];
    char command_buffer[20];
    char *pointer;

    // clear structure
    socketStatusSSL[socketId].id = 0;
    socketStatusSSL[socketId].state = 0;

    // AT#SSLS=<socketId>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[13])), socketId+1);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

    // check answer
    if(answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();

            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }

        #if DEBUG_ARDUPI4G > 0
            printf("Error getting socket status\n");
        #endif
        return 1;
    }

    // delimiters <-- "#SSLS: "
    strcpy(delimiters, (char*)pgm_read_word(&(table_IP[22])));

    // seek pattern in module response
    // the response is something similar to this:
    // "#SS: <id>,<status>,<localIp>,<lPort>,<remoteIp>,<rPort>\r\n\r\nOK\r\n"
    pointer = strstr((char*)_buffer, delimiters);

    if (pointer == NULL)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error: null pointer1\n");
        #endif
        return 1;
    }

    // delimiters <-- "#SSLS: ,\r\n"
    strcpy(delimiters, (char*)pgm_read_word(&(table_IP[23])));

    // find first value skipping delimiters
    pointer = strtok(pointer, delimiters);

    // iterate through response
    for (int i = 0; i < 2; i++)
    {
        if (pointer == NULL)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error: null pointer2\n");
            #endif
            return 1;
        }

        if (i == 0)
        {
            socketStatusSSL[socketId].id = (uint8_t) strtoul( pointer, NULL, 10);
        }
        else if (i == 1)
        {
            socketStatusSSL[socketId].state = (uint8_t) strtoul( pointer, NULL, 10);
            break;
        }

        pointer = strtok (NULL, " ,\r\n");
    }

    #if DEBUG_ARDUPI4G > 1
        printf("socketStatusSSL.id: %d\n", socketStatusSSL[socketId].id);
        printf("socketStatusSSL.state: %d\n", socketStatusSSL[socketId].state);
    #endif

    return 0;

}


/* Function:    This function gets info from a TCP or UDP socket
 * Parameters:  socketId: number of the socket Id
 *              parameter:  0 for total amount (in bytes) of sent data
 *                          1 for total amount (in bytes) of received data
 *                          2 for total amount (in bytes) of data just
 *                              arrived through the socket connection and
 *                              currently buffered, not yet read
 *                          3 for total amount (in bytes) of sent and not yet
 *                               acknowledged data
 * Return:  0 or positive value from the command
 *          -1 if error reading the info from the socket
 */
uint8_t arduPi4G::getSocketInfo(uint8_t socketId)
{
    uint8_t answer;
    char delimiters[20];
    char command_name[20];
    char command_buffer[20];
    char *pointer;

    // clear structure
    socketInfo[socketId].id = 0;
    socketInfo[socketId].sent = 0;
    socketInfo[socketId].received = 0;
    socketInfo[socketId].size = 0;
    socketInfo[socketId].ack = 0;

    // AT#SI=<socketId>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[1])), socketId+1);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }

    // delimiters <-- #SI: "
    strcpy(delimiters, (char*)pgm_read_word(&(table_IP[24])));

    // seek pattern in module response
    // the reponse is something similar to this:
    // "#SI: <v1>,<v2>,<v3>,<v4>,<v5>\r\n\r\nOK\r\n"
    pointer = strstr((char*)_buffer, delimiters);

    if (pointer == NULL)
    {
        return 1;
    }

    // delimiters <-- "#SI: ,\r\n"
    strcpy(delimiters, (char*)pgm_read_word(&(table_IP[25])));

    // find first value skipping delimiters
    pointer = strtok(pointer, delimiters);

    // iterate through response
    for (int i = 0; i < 5; i++)
    {
        if (pointer == NULL)
        {
            return 1;
        }

        if (i == 0)
        {
            socketInfo[socketId].id = (uint8_t) strtoul( pointer, NULL, 10);
        }
        else if (i == 1)
        {
            socketInfo[socketId].sent = (uint16_t) strtoul( pointer, NULL, 10);
        }
        else if (i == 2)
        {
            socketInfo[socketId].received = (uint16_t) strtoul( pointer, NULL, 10);
        }
        else if (i == 3)
        {
            socketInfo[socketId].size = (uint16_t) strtoul( pointer, NULL, 10);
        }
        else if (i == 4)
        {
            socketInfo[socketId].ack = (uint16_t) strtoul( pointer, NULL, 10);
            break;
        }

        pointer = strtok (NULL, " ,\r\n");
    }

    #if DEBUG_ARDUPI4G > 1
        printf("socketInfo.id: %d\n", socketInfo[socketId].id);
        printf("socketInfo.sent: %d\n", socketInfo[socketId].sent);
        printf("socketInfo.received: %d\n", socketInfo[socketId].received);
        printf("socketInfo.size: %d\n", socketInfo[socketId].size);
        printf("socketInfo.ack: %d\n", socketInfo[socketId].ack);
    #endif

    return 0;
}



// Public Methods /////////////////////////////////////////////////////////////

/* Function:    This function inits the LE910 module
 * Return:  0 if OK
 *          1 for no comunication
 *          2 if error switching CME errors to numeric response
 *          3 if error disabling the echo from the module
 *          4 if error enabling RTC update with network time
 */
uint8_t arduPi4G::ON()
{
    uint8_t counter;
    uint8_t answer;
    char command_buffer[50];

    // configure GPRS_PW pin
    pinMode(GPRS_PW, OUTPUT);

    // Set UART
    _uart = UART0;
    _baudrate = LE910_RATE;

    beginUART();

    // enable the pull-up resistor
    digitalWrite(RXD1_PIN, HIGH);

    // Power on the module
    digitalWrite(GPRS_PW, LOW);
    delay(500);
    digitalWrite(GPRS_PW, HIGH);
    delay(1500);
    digitalWrite(GPRS_PW, LOW);

    // Check communication with the module sending a basic AT command
    counter = 7;
    answer = 0;
    while ((counter > 0) && (answer == 0))
    {
        answer = sendCommand((char*)"AT\r", LE910_OK, 2000);
        counter--;
    }

    if (answer == 0)
    {
        // No comunication with the module Telit_LE910
        // Try sequence for Telit_LE910-V2 modules, ref:
        // "LE910_V2_Hardware_User_Guide_r5.pdf" 5.2 section
        digitalWrite(GPRS_PW, LOW);
        delay(500);
        digitalWrite(GPRS_PW, HIGH);
        delay(7000);
        digitalWrite(GPRS_PW, LOW);

        // Check communication with the module sending a basic AT command
        counter = 7;
        answer = 0;
        while ((counter > 0) && (answer == 0))
        {
            answer = sendCommand((char*)"AT\r", LE910_OK, 2000);
            counter--;      
        }

        if (answer == 0)
        {
            // No comunication with the module Telit_LE910_V2
        
            // Error code for no communication
            return 1; 
        }
    }

    // Set Report Mobile Equipment Error (CMEE)
    counter = 3;
    answer = 0;
    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[19])));    //AT+CMEE=1\r
    while ((counter > 0) && (answer == 0))
    {
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR, 500);
        counter--;
    }
    if (answer != 1)
    {
        // Error switching CME errors to numeric response

        // Error code for error switching CME errors to numeric response
        return 2;
    }

    // Disable command echo
    counter = 3;
    answer = 0;
    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[20])));    //ATE0\r
    while ((counter > 0) && (answer == 0))
    {
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR, 500);
        counter--;
    }
    if (answer != 1)
    {
        // Error disabling the echo from the module

        // Error code disabling the echo from the module
        return 3;
    }

    // Enable RTC update with network time
/*
    counter = 3;
    answer = 0;
    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[21])));    //AT#NITZ=7\r
    while ((counter > 0) && (answer == 0))
    {
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR, 500);
        counter--;
    }
    if (answer != 1)
    {
        // Error enabling RTC update with network time

        // Error code for error enabling RTC update with network time
        return 4;
    }
*/

    return 0;

}


/* Function:    This function powers off the LE910 module
 * Return:   nothing
 */
void arduPi4G::OFF()
{
    uint8_t counter;
    uint8_t answer;
    char command_buffer[50];

    pinMode(GPRS_PW, OUTPUT);

    // Check communication with the module sending a basic AT command
    counter = 17;
    answer = 0;
    while ((counter > 0) && (answer == 0))
    {
        answer = sendCommand((char*)"AT\r", LE910_OK, 1000);
        counter--;
    }

    if (answer == 0)
    {
    // No comunication with the module
        // Hardware Shutdown
        // Power on the module
        digitalWrite(GPRS_PW, LOW);
        delay(500);
        digitalWrite(GPRS_PW, HIGH);
        delay(2500);
        digitalWrite(GPRS_PW, LOW);
    }
    else
    {
    // Comunication with the module
        // Software Shutdown
        counter = 4;
        answer = 0;
        strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[38])));    //AT#SHDN\r
        while ((counter > 0) && (answer == 0))
        {
             answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR, 1000);
             counter--;
        }

        if (answer != 1)
        {
            // Error shoutdown the module by software
            // Force hardware power off
            digitalWrite(GPRS_PW, LOW);
            delay(500);
            digitalWrite(GPRS_PW, HIGH);
            delay(2500);
            digitalWrite(GPRS_PW, LOW);

            // Error code for fault in software shutdown
            // return 2; //no implemented
        }
    }


    // close UART0
    closeUART();

    digitalWrite(RXD1_PIN, HIGH);

    // wait finalization state above 15 seconds (max 25 seconds)
    delay(25000);


}


/* Function:    This function sets a PIN / PUK code
 * Parameters: code: string with the requested code
 * Return:  0 if OK
 *          1 if error
 */
uint8_t arduPi4G::enterPIN(char* code)
{
    return enterPIN(code, NULL);
}


/* Function:    This function sets a PIN / PUK code
 * Parameters:
 *      code: string with the requested code
 *      new_code: string with the new code (only for SIM PUK or SIM PUK2)
 * Return:  0 if OK, 1 if error
 */
uint8_t arduPi4G::enterPIN(char* code, char* new_code)
{
    uint8_t answer;
    char command_buffer[50];

    // generate command depending on selection
    if (new_code == NULL)
    {
        // AT+CPIN="<code>"\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_PIN[18])), code);
    }
    else
    {
        // AT+CPIN="<code>","<new_code>"\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_PIN[19])), code, new_code);
    }

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

    // check response
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }

    return 0;
}


/* Function: This function returns the result code for a PIN request
 * Return:
 *      0 for PIN ready
 *      1 LE910 is awaiting SIM PIN
 *      2 LE910 is awaiting SIM PUK
 *      3 LE910 is awaiting phone-to-SIM card password.
 *      4 LE910 is awaiting phone-to-very-first-SIM card password.
 *      5 LE910 is awaiting phone-to-very-first-SIM card unblocking password.
 *      6 LE910 is awaiting SIM PIN2
 *      7 LE910 is awaiting SIM PUK2
 *      8 LE910 is awaiting network personalization password
 *      9 LE910 is awaiting network personalization unblocking password
 *      10 LE910 is awaiting network subset personalization password
 *      11 LE910 is awaiting network subset personalization unblocking password
 *      12 LE910 is awaiting service provider personalization password
 *      13 LE910 is awaiting service provider personalization unblocking password
 *      14 LE910 is awaiting corporate personalization password
 *      15 LE910 is awaiting corporate personalization unblocking password
 *      254 if not expected code returned
 *      255 if command error
 */
uint8_t arduPi4G::checkPIN()
{
    char command_name[20];
    char command_buffer[20];
    char command_answer[20];
    uint8_t answer;

    // command: AT+CPIN?\r
    strcpy(command_buffer, (char*)pgm_read_word(&(table_PIN[0])));

    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 255;
    }

    // Seeks a code
    for (int x = 0; x < 17; x++ )
    {
        strcpy(command_answer, (char*)pgm_read_word(&(table_PIN[x+1])));
        answer = find( _buffer, _length, command_answer);
        if (answer == 1)
        {
            return x;
        }
    }

    return 254;
}


/* Function:    This function checks connection status
 * Parameters:  time: max allowed time in seconds to connect
 * Return:  0 If the module is connected to the network
 *          1 not registered, ME is not currently searching for a new operator to register to
 *          2 not registered, but ME is currently searching for a new operator to register to
 *          3 registration denied
 *          4 unknown
 */
uint8_t arduPi4G::checkConnection(uint8_t time)
{
    uint8_t answer;
    uint8_t status;
    uint8_t value;
    uint32_t previous, max_time;

    char command_buffer[40];
    char command_answer[20];

    answer = 0;
    max_time = (unsigned long)time * 1000;
    previous = millis();

    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[0])));     //AT+CREG?\r
    strcpy(command_answer, (char*)pgm_read_word(&(table_4G[1])));     //+CREG: 0,

    do{
        // Sends the command and waits for the answer (0,1 for home network and 0,5 for roaming)
        // Sends the command and waits for the answer. The status options in
        // "+CREG: 0,<status>" response are:
        //  0: not registered, ME is not currently searching a new operator to register to
        //  1: registered, home network
        //  2: not registered, but ME is currently searching a new operator to register to
        //  3: registration denied
        //  4: unknown
        //  5: registered, roaming
        answer = sendCommand(command_buffer, command_answer, 2000);

        if (answer == 1)
        {
            status = waitFor((char*)"OK",500);

            if (status == 1)
            {
                parseUint8(&answer, (char*)"\r\n OK");
            }
        }

        delay(2000);
        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();
    }while (((answer == 2) || (answer == 3) || (answer == 4) || (answer == 0)) && ((millis() - previous) < max_time));


    if (((answer != 1) && (answer != 5)) || ((millis() - previous) > max_time))
    {
        if (answer == 0)
        {
            return answer + 1;
        }
        return answer;
    }

    return 0;
}


/* Function:    This function checks EPS Network Registratio Status
 * Parameters:  time: max allowed time in seconds to connect
 * Return:      0 if OK
 */
uint8_t arduPi4G::checkConnectionEPS(uint8_t time)
{
    uint8_t answer;
    uint8_t status;
    uint32_t previous;
    char command_buffer[40];
    char command_answer[20];

    // init variables
    status = 0;
    answer = 0;
    previous = millis();

    //// 1. Generate command
    // "AT+CEREG?\r"
    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[44])));

    //// 2. Generate answer
    // "+CEREG: 0,"
    strcpy(command_answer, (char*)pgm_read_word(&(table_4G[45])));

    //// 3. Iterate until status is correct
    while ((status != 1) && (status != 5))
    {
        // Sends the command and waits for the answer. The status options in
        // "+CEREG: 0,<status>" response are:
        //  0 - not registered, MT is not currently searching an operator to register to.
        //  1 - registered, home network.
        //  2 - not registered, but MT is currently trying to attach or searching an operator to register to.
        //  3 - registration denied.
        //  4 - unknown (e.g. out of E-UTRAN coverage).
        //  5 - registered, roaming.
        //  6 - registered for "SMS only", home network (not applicable)
        //  7 - registered for "SMS only", roaming (not applicable).
        //  8 - attached for emergency bearer services only (See NOTE 2).
        //  9 - registered for "CSFB not preferred", home network (not applicable).
        //  10 - registered for "CSFB not preferred", roaming (not applicable).

        answer = sendCommand(command_buffer, command_answer, LE910_ERROR_CODE, 2000);

        if (answer == 1)
        {
            // get whole response
            status = waitFor(LE910_OK, 500);

            if (status == 1)
            {
                parseUint8(&status, (char *)"\r\n OK");
                #if DEBUG_ARDUPI4G > 1
                    printf("----> status: %d\n", status);
                #endif
            }
        }

        // check status from response
        if ((status == 1) || (status == 5))
        {
            #if DEBUG_ARDUPI4G > 1
                printf("Break condition\n");
                printf("status: %d\n", status);
            #endif
            break;
        }
        else if (status == 2)
        {
            // continue waiting for correct response
            #if DEBUG_ARDUPI4G > 0
                printf("Continue waiting for correct response\n");
            #endif
        }

        delay(1000);

        // Condition to avoid an overflow (DO NOT REMOVE)
        if (millis() < previous) previous = millis();

        // check timeout error
        if ((millis() - previous) > (uint32_t)(time * 1000))
        {
            return status;
        }
    }

    // check bad responses
    if ((status != 1) &&
        (status != 5) &&
        (status != 6) &&
        (status != 7) &&
        (status != 8) &&
        (status != 9) &&
        (status != 10))
    {
        if (status == 0)
        {
            return status + 1;
        }

        return status;
    }

    return 0;
}


/* This function manages incoming data from the module
 * Parameters:  wait_time: maximum waiting time in milliseconds
 * Return:  0 if 'OK', 'x' if error
*/
uint8_t arduPi4G::manageIncomingData(unsigned long wait_time)
{

    uint8_t answer;
    char command_buffer[50];
    char sms_incoming[15];
    char IP_incoming[15];

    // set _incomingType to no data received
    _incomingType = 0;

    // "+CMTI"
    strcpy(sms_incoming, (char*)pgm_read_word(&(table_4G[22])));

    // "SRING: "
    strcpy(IP_incoming, (char*)pgm_read_word(&(table_4G[23])));

    // Wait for data
    answer = waitFor(sms_incoming, IP_incoming, wait_time);

    if (answer == 1)
    {
        // Received data: "+CMTI: <memr>,<index>\r"
        #if DEBUG_ARDUPI4G > 0
            printf("Incoming SMS\n");
        #endif

        // wait for first ','
        waitFor((char*)",", 1000);

        // wait for end of response
        answer = waitFor((char*)"\r", 2000);

        if (answer == 1)
        {
            // store index
            _smsIndex = atoi((char*) _buffer);

            #if DEBUG_ARDUPI4G > 1
                printf("SMS index: %d\n", _smsIndex);
            #endif

            // set attribute to SMS data
            _incomingType = LE910_INCOMING_SMS;

            return 0;
        }
        else
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error reading SMS index\n");
            #endif
        }

        return 1;
    }
    else if (answer == 2)
    {
        #if DEBUG_ARDUPI4G > 1
            printf("Incoming IP data\n");
        #endif

        answer = waitFor((char*)"\r", 2000);
        if (answer == 1)
        {
            _socketIndex = atoi((char*) _buffer);
            _socketIndex--;

            #if DEBUG_ARDUPI4G > 1
                printf("Socket Index: %d\n", _socketIndex);
            #endif

            // set attribute to IP data
            _incomingType = LE910_INCOMING_IP;

            // wait for get socket status:
            answer = getSocketStatus(_socketIndex);

            if (answer == 0)
            {
                #if DEBUG_ARDUPI4G > 1
                    printf("getSocketStatus OK: %d\n", socketStatus[_socketIndex].state);
                #endif
            }
            else
            {
                #if DEBUG_ARDUPI4G > 0
                    printf("getSocketStatus ERROR.\n");
                #endif
                return 1;
            }

            // check if socket state is a Socket with an incoming connection,
            // in that case module accepts the connection with the client
            if (socketStatus[_socketIndex].state == 5)
            {
                // accept connection in command mode
                // AT#SA=<socketId>,1\r
                sprintf(command_buffer, (char*)pgm_read_word(&(table_4G[37])), _socketIndex+1);

                // send command
                answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);

                // check answer
                if (answer != 1)
                {
                    if (answer == 2)
                    {
                        getErrorCode();
                    }
                    return 1;
                }

                // get new socket status
                answer = getSocketStatus(_socketIndex);

                if (answer == 0)
                {
                    #if DEBUG_ARDUPI4G > 1
                        printf("getSocketStatus OK: %d\n", socketStatus[_socketIndex].state);
                    #endif
                }
                else
                {
                    #if DEBUG_ARDUPI4G > 0
                        printf("getSocketStatus ERROR.\n");
                    #endif
                    return 1;
                }
            }
            else
            {
                return 1;
            }

            // return OK
            return 0;
        }
        else
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error reading SocketId\n");
            #endif
        }
    }

    // timeout error
    return 3;
}


/* This function manages incoming data from sockets
 * Return:  0 if 'OK', 'x' if error
*/
uint8_t arduPi4G::manageSockets()
{
    return manageSockets(0);
}


/* This function manages incoming data from sockets
 * Parameters:  wait_time: maximum waiting time in milliseconds
 * Return:  0 if 'OK', 'x' if error
*/
uint8_t arduPi4G::manageSockets(unsigned long wait_time)
{
    uint8_t answer;
    char command_buffer[50];
    uint32_t previous = millis();
    bool pending_accept = false;
    bool new_accepted = false;

    //// 1. Get socket status and wait until there is
    //  at least one pending connection to be accepted
    do
    {
        answer = getAllSocketStatus();

        if (answer != 0)
        {
            return 1;
        }

        for (int profile = 0; profile < 6; profile++)
        {
            if (socketStatus[profile].state == 5)
            {
                pending_accept = true;
            }
        }

        // check if there is any pending connection
        if (pending_accept == true)
        {
            break;
        }

        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();

        delay(500);

    } while (((millis() - previous) < wait_time));

    //// 2. Accept connections if needed
    for (int profile = 0; profile < 6; profile++)
    {
        // check if socket state is a Socket with an incoming connection,
        // in that case module accepts the connection with the client
        if (socketStatus[profile].state == 5)
        {
            // accept connection in command mode
            // AT#SA=<socketId>,1\r
            sprintf(command_buffer, (char*)pgm_read_word(&(table_4G[37])), profile+1);

            // send command
            answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);

            // check answer
            if (answer == 1)
            {
                new_accepted = true;
            }
            else
            {
                if (answer == 2)
                {
                    getErrorCode();
                }
                return 1;
            }
        }
    }

    //// 3. Get new socket status
    answer = getAllSocketStatus();

    if (answer == 0)
    {
        #if DEBUG_ARDUPI4G > 1
            printf("getSocketStatus OK\n");
        #endif
    }
    else
    {
        #if DEBUG_ARDUPI4G > 0
            printf("getSocketStatus ERROR.\n");
        #endif
        return 1;
    }

    // return OK if new incoming connections have been accepted
    if (new_accepted == true)
    {
        return 0;
    }

    return 1;
}


/* This function sets the parameters to use SMS
 * Return:  0 if OK
 *          1 if error setting the SMS format
 *          2 if error selecting the storage
 *          3 if error setting the incoming SMS indication
*/
uint8_t arduPi4G::configureSMS()
{
    uint8_t answer;
    char command_buffer[50];

    #if DEBUG_ARDUPI4G > 0
        printf("Setting SMS parameters\n");
    #endif

    // 1. Configure format, storage and indication parameters
    memset(command_buffer,0x00,sizeof(command_buffer));
    sprintf(command_buffer, (char*)pgm_read_word(&(table_SMS[0])));   //AT+CMGF=1
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR,1000);
    if (answer != 1)
    {
    #if DEBUG_ARDUPI4G > 0
        printf("Error CMGF\n");
    #endif
        return 1;
    }

    memset(command_buffer,0x00,sizeof(command_buffer));
    sprintf(command_buffer, (char*)pgm_read_word(&(table_SMS[1])));   //AT+CPMS="SM","SM","SM"
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR,1000);
    if (answer != 1)
    {
    #if DEBUG_ARDUPI4G > 0
        printf("Error CPMS\n");
    #endif
        return 2;
    }

    memset(command_buffer,0x00,sizeof(command_buffer));
    sprintf(command_buffer, (char*)pgm_read_word(&(table_SMS[2])));   //AT+CNMI=2,1,0,0,0
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR,1000);
    if (answer != 1)
    {
        printf("Error CNMI\n");
        return 3;
    }
    delay(500);

    return 0;
}


/* This function sends a SMS
 * Parameters:  phone_number: number to send the SMS
 *              sms_string: body of the SMS
 * Return:  0 if OK
 *          1 not registered, ME is not currently searching for a new operator to register to
 *          2 not registered, but ME is currently searching for a new operator to register to
 *          3 registration denied
 *          4 unknown connection error
 *          5 if error setting the phone number
 *          6 if error sending the body
*/
uint8_t arduPi4G::sendSMS(char* phone_number, char* sms_string)
{
    char command_name[40];
    char command_buffer[40];
    uint8_t answer;

    #if DEBUG_ARDUPI4G > 0
        printf("Checking connection\n");
    #endif

    answer = checkConnection(60);
    if (answer != 0)
    {
        return answer;
    }

    #if DEBUG_ARDUPI4G > 0
        printf("Sending SMS\n");
    #endif

    // AT+CMGS="<phone_number>"
    sprintf(command_buffer, (char*)pgm_read_word(&(table_SMS[4])), phone_number);

    // send command
    answer = sendCommand(command_buffer, (char*)">");

    // check answer
    if (answer != 1)
    {
        printByte(0x1B, UART0); //ESC ASCII
        return 5;
    }

    printString(sms_string, UART0);
    command_buffer[0] = 0x1A;
    command_buffer[1] = '\0';
    answer = sendCommand(command_buffer, LE910_OK);
    if (answer != 1)
    {
        printByte(0x1B, UART0); //ESC ASCII
        return 6;
    }

    return 0;
}


/* Function:    This function reads the last unread received message
 * Parameters:  none
 * Return:  0 if ok
 *          1 if error
 */
uint8_t arduPi4G::readNewSMS()
{   
    return readNewSMS(0);
}


/* Function:    This function reads the last unread received message
 * Parameters:  none
 * Return:  0 if ok
 *          1 if error
 */
uint8_t arduPi4G::readNewSMS(uint32_t timeout)
{   
    uint8_t answer;
    char command_buffer[50];
    char command_answer[10];
    char *pointer;
    uint32_t previous;
    
    //// 1. Check connection
    answer = checkConnection(60);
    if (answer != 0)
    {
        #if DEBUG_ARDUPI4G > 0
            PRINT_LE910("Error checking connection\n");
        #endif
        return answer;
    }
    
    //// 3. Read unread message
    // AT+CMGL="REC UNREAD"\r
    memset(command_buffer, 0x00, sizeof(command_buffer));
    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[36])));
    
    // "+CMGL: "
    memset(command_answer, 0x00, sizeof(command_answer));
    sprintf(command_answer, (char*)pgm_read_word(&(table_4G[42])));
    
    // get current time
    previous = millis();
    
    do
    {
        // send command
        answer = sendCommand(command_buffer, command_answer, LE910_ERROR_CODE, LE910_ERROR, LE910_OK, 2000);

        // check answer
        if (answer == 1)
        {
            // read first line
            answer = waitFor((char *)"\r\n", 1000);

            // _buffer stores a response like:
            //  19,"REC UNREAD","+345901000118833","","16/06/20,12:20:20+08"    
            pointer = strtok((char*)_buffer,"\",");
            
            for (int i = 0; i < 5; i++) 
            {   
                // Get SMS index
                if (i == 0)
                {
                    _smsIndex = atoi((char*)pointer);
                }
                
                // Get SMS status
                if (i == 1)
                {
                    strncpy(_smsStatus, pointer, sizeof(_smsStatus));           
                }
                    
                // Get phone number
                if (i == 2)
                {
                    strncpy(_smsNumber, pointer, sizeof(_smsNumber));           
                }
                
                // Get date
                if (i == 3)
                {
                    strncpy(_smsDate, pointer, sizeof(_smsDate));           
                }
                
                // Get time
                if (i == 4)
                {
                    strncpy(_smsTime, pointer, sizeof(_smsTime));       
                }
                
                pointer = strtok(NULL, "\",");  
            }
            
            // Get body
            waitFor((char *)"\r\n\r\nOK", 1000);
            
            // update buffer length
            _length -= strlen("\r\n\r\nOK");
            _buffer[_length] = 0x00;
            
            return 0;
        }
        else if (answer == 4)
        {
            // LE910_OK
            // do nothing because no incoming SMS was received
        }       
        else
        {
            #if DEBUG_ARDUPI4G > 0
                PRINT_LE910("Error getting response\n"); 
            #endif
            if (answer == 2)
            {
                getErrorCode();
                #if DEBUG_ARDUPI4G > 0
                    printErrorCode();
                #endif
            }
            return 1;
        }
        
        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();
        
    } while (millis()-previous < timeout);
    
    return 1;
}


/* This function reads a SMS
 * Parameters:  sms_index: index of the SMS into the memory
 *              sms_buffer: buffer to store the SMS
 * Return:  0 if OK
 *          1 if error getting the SMS
 *          2 if error storing the message
*/
uint8_t arduPi4G::readSMS(uint8_t sms_index)
{   
    char command_buffer[40];
    char command_answer[15];
    uint8_t answer;
    char* pointer;
    
    #if DEBUG_ARDUPI4G > 1
        printf("Reading SMS from index %d\n", sms_index);
    #endif      
        
    //// 1. Generate command request
    // AT+CMGR=<sms_index>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_SMS[3])), sms_index);
    
    //// 2. Generate expected answer
    // +CMGR: 
    memset(command_answer, 0x00, sizeof(command_answer));
    sprintf(command_answer,(char*)pgm_read_word(&(table_4G[43])));

    // send command
    answer = sendCommand(command_buffer, command_answer, LE910_ERROR_CODE, LE910_ERROR, LE910_OK, 5000);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }       
        return 1;
    }

    // read first line
    answer = waitFor((char *)"\r\n", 1000);

    // _buffer stores a response like:
    //  "REC READ","+34666666666","","16/06/20,10:12:34+08"
    
    pointer = strtok((char*)_buffer,"\",");
    
    for (int i = 0; i < 4; i++) 
    {   
        // Get SMS status
        if (i == 0)
        {
            strncpy(_smsStatus, pointer, sizeof(_smsStatus));           
        }       
        // Get phone number
        if (i == 1)
        {
            strncpy(_smsNumber, pointer, sizeof(_smsNumber));           
        }
        
        // Get date
        if (i == 2)
        {
            strncpy(_smsDate, pointer, sizeof(_smsDate));           
        }
        
        // Get time
        if (i == 3)
        {
            strncpy(_smsTime, pointer, sizeof(_smsTime));       
        }
        
        pointer = strtok(NULL, "\",");  
    }
    
    // Get body
    waitFor((char *)"\r\n\r\nOK", 1000);
    
    // update buffer length
    _length -= strlen("\r\n\r\nOK");
    _buffer[_length] = 0x00;    

    return 0;   
}


/* This function deletes a SMS
 * Parameters:  sms_index: index of the SMS into the memory
 * Return:  0 if OK
 *          1 if error deleting the SMS
 */
uint8_t arduPi4G::deleteSMS(uint8_t sms_index)
{
    return deleteSMS(sms_index, SMS_DELETE_MESSAGE);
}


/* This function deletes a SMS
 * Parameters:
 *  sms_index:  index of the SMS into the memory
 *  del_flag:
 *      0:  Delete message specified in <index>
 *      1:  Delete all read messages from <memr> storage, leaving unread messages
 *          and stored mobile originated messages (whether sent or not) untouched
 *      2:  Delete all read messages from <memr> storage and sent mobile originated
 *          messages, leaving unread messages and unsent mobile originated messages untouched
 *      3:  Delete all read messages from <memr> storage, sent and unsent mobile
 *          originated messages, leaving unread messages untouched
 *      4:  Delete all messages from <memr> storage
 * Return:  0 if OK
 *          1 if error deleting the SMS
*/
uint8_t arduPi4G::deleteSMS(uint8_t sms_index, uint8_t del_flag)
{
    char command_buffer[20];
    uint8_t answer;

    #if DEBUG_ARDUPI4G > 0
        printf("Deleting SMS from index %d\n", sms_index);
    #endif

    // AT+CMGD=<sms_index>,<del_flag>\r
    sprintf(command_buffer,(char*)pgm_read_word(&(table_SMS[6])), sms_index, del_flag);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }

    return 0;
}


/* Function:    This function performs a HTTP request
 * Parameters:  method: selected HTTP method: GET_METHOD
 *                                              HEAD_METHOD
 *                                              DELETE_METHOD
 *              url: host name or IP address of the server
 *              port: server port
 *              resource: parameter indicating the HTTP resource, object of the
 *                  request
 *              answer_storage: pointer to a string to save the HTTP response
 * Return:  0 if OK
 *          1 not registered, ME is not currently searching for a new operator to register to
 *          2 not registered, but ME is currently searching for a new operator to register to
 *          3 registration denied
 *          4 unknown
 *          6 not registered, ME is not currently searching for a new operator to register to
 *          8 not registered, but ME is currently searching for a new operator to register to
 *          9 registration denied
 *          10 unknown
 *          12 if error setting APN
 *          13 if error setting login
 *          14 if error setting password
 *          15 if error activating GPRS connection
 *          16 if error setting URL and port
 *          17 if error sending the request
 *          18 if error sending POST / PUT data
 *          19 if worng method has been selected
 *          20 if timeout waiting the URC
 *          21 if error reading the URC
 *          22 if error reading the HTTP data
 *          23 if error reading the HTTP data
 */
uint8_t arduPi4G::http(uint8_t method,
                       char* url,
                       uint16_t port,
                       char* resource)
{
    return http(method,
                url,
                port,
                resource,
                NULL);
}


/* Function:    This function performs a HTTP request
 * Parameters:  method: selected HTTP method:   GET_METHOD
 *                                              HEAD_METHOD
 *                                              DELETE_METHOD
 *                                              POST_METHOD
 *                                              PUT_METHOD
 *                                              POST_METHOD_FRAME
 *              url: host name or IP address of the server
 *              port: server port
 *              resource: parameter indicating the HTTP resource, object of the
 *                  request
 *              POST_PUT_data: data to send in POST/PUT method
 *              POST_PUT_length: data length to send in POST/PUT method
 * Return:  0 if OK
 *          1 not registered, ME is not currently searching for a new operator to register to
 *          2 not registered, but ME is currently searching for a new operator to register to
 *          3 registration denied
 *          4 unknown
 *          6 not registered, ME is not currently searching for a new operator to register to
 *          8 not registered, but ME is currently searching for a new operator to register to
 *          9 registration denied
 *          10 unknown
 *          12 if error setting APN
 *          13 if error setting login
 *          14 if error setting password
 *          15 if error activating GPRS connection
 *          16 if error setting URL and port
 *          17 if error sending the request
 *          18 if error sending POST / PUT data
 *          19 if worng method has been selected
 *          20 if timeout waiting the URC
 *          21 if error reading the URC
 *          22 if error reading the HTTP data
 *          23 if error reading the HTTP data
 */
uint8_t arduPi4G::http(uint8_t method,
                       char* url,
                       uint16_t port,
                       char* resource,
                       char* data)
{
    uint8_t answer;
    int16_t http_data;

    // 1. Check data connection
    answer = checkDataConnection(60);
    if (answer != 0)
    {
        return answer;  // 1 to 15 error codes
    }

    // 2. Configure parameters  and send the request
    answer = httpRequest(method, url, port, resource, data);
    if (answer != 0)
    {
        return answer+15;   // 16 to 19 error codes
    }

    // 3. Wait for the response
    answer = httpWaitResponse(LE910_HTTP_TIMEOUT);
    if (answer != 0)
    {
        return answer+19;   // 20 to 24 error codes
    }

    return 0;
}


/* Function:    This function configures FTP parameters and opens the connection
 * Parameters:  server: address of FTP server
 *              port: port of FTP server
 *              username: authentication user identification string for FTP
 *              password: authentication password for FTP
 * Return:  0 if OK
 *          1 not registered, ME is not currently searching for a new operator to register to
 *          2 not registered, but ME is currently searching for a new operator to register to
 *          3 registration denied
 *          4 unknown
 *          6 not registered, ME is not currently searching for a new operator to register to
 *          8 not registered, but ME is currently searching for a new operator to register to
 *          9 registration denied
 *          10 unknown
 *          12 if error setting APN
 *          13 if error setting login
 *          14 if error setting password
 *          15 if error activating GPRS connection
 *          16 if error opening the FTP connection
 *          17 if error setting the transfer type
 */
uint8_t arduPi4G::ftpOpenSession(char* server,
                                 uint16_t port,
                                 char* username,
                                 char* password)
{
    uint8_t answer;
    char command_buffer[100];

    #if DEBUG_ARDUPI4G > 1
        printf("Checking connection\n");
    #endif

    // 1. Check data connection
    answer = checkDataConnection(60);
    if (answer != 0)
    {
        return answer;  // 1 to 15 error codes
    }

    #if DEBUG_ARDUPI4G > 1
        printf("Opening FTP session\n");
    #endif

    // 2. Configure FTP parameters and open the connection
    // AT#FTPOPEN="<server>:<port>","<username>","<password>",1\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_FTP[0])),
            server,
            port,
            username,
            password);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, LE910_FTP_CONF_TIMEOUT);
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }
        return 16;
    }


    // 3. Set binary transfer. Once connected we can call the AT#FTPTYPE command
    // AT#FTPTYPE=0\r
    strcpy(command_buffer, (char*)pgm_read_word(&(table_FTP[4])));

    // mandatory delay
    delay(2000);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }

        return 17;
    }


    return 0;
}


/* Function:    This function closes the FTP connection
 * Return:  0 if OK
 *          >1 if error
 */
uint8_t arduPi4G::ftpCloseSession()
{
    char command_buffer[50];
    uint8_t answer;
    uint8_t counter;

    #if DEBUG_ARDUPI4G > 1
        printf("Closing FTP session\n");
    #endif

    delay(1000);

    //AT#FTPCLOSE\r
    strcpy(command_buffer, (char*)pgm_read_word(&(table_FTP[1])));

    counter = 4;
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 10000);
    while ((counter > 0) && (answer != 1))
    {
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 10000);
        delay(2000);
        counter--;
    }

    if (answer == 1)
    {
        // ok
        return 0;
    }
    else if (answer == 2)
    {
        // error code
        getErrorCode();
        #if DEBUG_ARDUPI4G > 0
            printf("Error code: %s\n", (char *)_buffer);
        #endif
        return 1;
    }
    else
    {
        return 2;
    }
}


/* Function:    This function uses PUT to send a file to a FTP server
 * Parameters:  ftp_file: destiny file
 *              sd_file: origin file
 * Return:  0 if OK
 *          1 if error reading the file size
 *          2 if error openning the file
 *          3 if error setting the pointer of the file
 *          4 if error openning the PUT connection
 *          5 if error exiting from the data mode
 *          6 if error sending data
 */
uint8_t arduPi4G::ftpUpload( char* ftp_file, char* sd_file)
{

    uint8_t answer;
    uint8_t counter;
    char command_answer[20];
    char command_buffer[512];
    int32_t file_size = 0;
    int nBytes = 0;
    uint8_t error_counter = 5;

    FILE *file;

    file_size = 0;
    error_counter = 5;

    // search file in current working directory and open it
    // exit if error and modify the general flag with FILE_OPEN_ERROR
    file = fopen((char*)sd_file,"rb");
    if (file == NULL)
    {
        // SD error
        #if DEBUG_ARDUPI4G > 0
            printf("Error: opening file\n");
        #endif
        return 3;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    if (file_size < 0)
    {
        // Return the SD to the original state

        return 5;
    }

    // set pointer to the beginning of the file
    if (fseek(file, 0, SEEK_SET))
    {
        // SD error
        #if DEBUG_ARDUPI4G > 0
            printf("Error: setting initial offset in file\n");
        #endif
        return 4;
    }

    // mandatory delay so the module works ok
    delay(1000);

    // 5. Open the PUT connection
    // AT#FTPPUT=<ftp_file>,0\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_FTP[2])), ftp_file);

    // send command
    answer = sendCommand(command_buffer, (char *)"CONNECT", (char *)"NO CARRIER", 15000);
    if (answer != 1)
    {
        // Close file
        fclose(file);

        return 6;
    }

    #if DEBUG_ARDUPI4G > 0
        printf("File size: %d\n", file_size);
        printf("error_counter: %d\n", error_counter);
    #endif

    // 6. Send data to the server
    while ((file_size > 0) && (error_counter > 0))
    {
        // 6a. Read data from SD
        nBytes = fread(command_buffer, 1, 500, file);

        // 6b. Send the data if no errors
        if (nBytes == -1)
        {
            // SD error
            #if DEBUG_ARDUPI4G > 0
                printf("Error reading the file\n");
            #endif
            error_counter--;
        }
        else
        {
            for (int i = 0; i < nBytes; i++)
            {
                printByte(command_buffer[i], UART0);
            }

            file_size -= nBytes;
        }

        #if DEBUG_ARDUPI4G > 0
            printf("Remains: %d Bytes\n", file_size);
        #endif
    }

    // 7. Close file
    fclose(file);

    // 8. Exit from data mode
    delay(2000);

    // "+++"
    strcpy(command_buffer, (char*)pgm_read_word(&(table_FTP[11])));
    // "NO CARRIER"
    strcpy(command_answer, (char*)pgm_read_word(&(table_FTP[12])));

    counter = 3;
    answer = 0;
    while ((counter > 0) && (answer == 0))
    {
        answer = sendCommand(command_buffer, command_answer, LE910_ERROR_CODE, 15000);
        counter--;
    }

    if (answer != 1)
    {
        return 7;
    }

    if (error_counter == 0)
    {
        return 8;
    }

    return 0;
}


/* Function:    This function uses GET to read a file from a FTP server
 * Parameters:  sd_file: destiny file
 *              ftp_file: origin file
 * Return:  0 if OK
 *          1 if error reading the file size
 *          2 if error going to the root directory in SD
 *          3 if error creating the file in SD
 *          4 if error opening the file
 *          5 if error setting the pointer of the file
 *          6 if error openning the GET connection
 *          7 if error reading the data
 */
uint8_t arduPi4G::ftpDownload( char* sd_file, char* ftp_file)
{
    uint8_t error;
    uint8_t answer;
    char command_name[50];
    char command_buffer[50];
    char command_answer[50];
    bool sd_state;
    uint32_t server_filesize = 0;
    uint32_t sd_filesize = 0;
    int32_t packet_size = LE910_MAX_DL_PAYLOAD;
    int nBytes = 0;
    uint8_t error_counter = 5;
    int data_available;

    // init error code variable
    _errorCode = 0;

    delay(2000);

    /// 1. Get filesize in FTP server
    error = ftpFileSize(ftp_file);

    if (error == 0)
    {
        server_filesize = _filesize;

        if (server_filesize < LE910_MAX_DL_PAYLOAD)
        {
            packet_size = server_filesize;
        }
    }
    else
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error retrieving file size\n");
        #endif
        return 1;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("File size in FTP: %d\n", server_filesize);
    #endif

    //// 2. Prepare Raspberry SD card for downloading
    // define file variable
    FILE *file;

    // search file in current directory and open it in write mode
    file = fopen((char*)sd_file,"wb");
    if (file == NULL)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("error opening file\n");
        #endif
        return 4;
    }

    // jump over zero 'offset'
    if (fseek(file, 0, SEEK_SET))
    {
        #if DEBUG_ARDUPI4G > 0
            printf("setting file offset\n");
        #endif
        fclose(file);

        return 5;
    }

    /// 3. Open the GET connection
    // AT#FTPGETPKT="<ftp_file>"\r
    memset(command_buffer,0x00,sizeof(command_buffer));
    sprintf(command_buffer, (char*)pgm_read_word(&(table_FTP[3])),ftp_file);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, LE910_FTP_TIMEOUT);

    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }

        // Close file
        fclose(file);

        return 6;
    }

    // calculate delay for retrieving packets depending on file size
    uint32_t delay_ms;

    if (_filesize > 50000)
    {
        delay_ms = 500;
    }
    else if (_filesize > 25000)
    {
        delay_ms = _filesize/100;
    }
    else
    {
        delay_ms = _filesize/160;
    }

    setDelay(delay_ms);
    delay(1000);

    // "#FTPRECV: "
    sprintf(command_answer, (char*)pgm_read_word(&(table_FTP[13])));

    /// 4. Read data from the module
    while ((server_filesize > 0) && (error_counter > 0))
    {
        if (server_filesize > LE910_MAX_DL_PAYLOAD)
        {
            packet_size = LE910_MAX_DL_PAYLOAD;
        }
        else
        {
            packet_size = server_filesize;
        }

        //// 6b Request data
        // AT#FTPRECV=<packet_size>\r
        memset(command_buffer, 0x00, sizeof(command_buffer));
        sprintf(command_buffer,(char*)pgm_read_word(&(table_FTP[6])),packet_size);

        // send command
        answer = sendCommand(command_buffer, command_answer, LE910_ERROR_CODE, LE910_ERROR, 2000);
        delay(500);

        if (answer == 2)
        {
            getErrorCode();
            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
            printf("_buffer: %s\n", (char*)_buffer);

            return 7;
        }
        else if (answer != 1)
        {
            error_counter--;
            // Error could be that no data in the buffer, wait one second
            delay(1000);
            #if DEBUG_ARDUPI4G > 0
                printf("Error getting data\n");
            #endif
        }
        else
        {
            //// 6c Read the number of bytes of the packet
            waitFor((char*)"\r\n", 100);
            error = parseInt32(&packet_size,(char *)"\r\n");

            if (error == 1)
            {
                #if DEBUG_ARDUPI4G > 0
                    printf("Error getting packet size\n");
                #endif
                fclose(file);

                return 8;
            }

            #if DEBUG_ARDUPI4G > 1
                printf("Packet size: %i\n", packet_size);
            #endif

            // Read the data from the UART and stores in _buffer
            nBytes = readBuffer(packet_size);

            if (nBytes != packet_size)
            {
                #if DEBUG_ARDUPI4G > 0
                    printf("Error in packet size mismatch\n");
                    printf("packet size: %i\n", packet_size);
                    printf("nbytes: %i\n", nBytes);
                    printf("buffer: %s\n", _buffer);
                #endif

                fclose(file);
                return 9;
            }

            // Write the data into the Raspberry SD
            if (fwrite(_buffer, 1, nBytes, file) != (int)nBytes)
            {
                #if DEBUG_ARDUPI4G > 0
                    printf("Writing SD error");
                #endif
                fclose(file);

                return 10;
            }

            // decrement filesize
            server_filesize -= nBytes;

            #if DEBUG_ARDUPI4G > 1
                printf("Remaining server_filesize: %i\n", server_filesize);
                printf("_buffer: %s\n",_buffer);
            #endif
        }
    }

    // update file size in SD card
    fseek(file, 0, SEEK_END);
    sd_filesize = ftell(file);

    // Close file
    fclose(file);

    // save the actual server file size
    server_filesize = _filesize;

    // check size mismatch
    if (sd_filesize != server_filesize)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("File size mismatch\n");
            printf("sd_filesize: %d\n", sd_filesize);
            printf("server_filesize: %d\n", server_filesize);
        #endif
        return 11;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("DOWNLOAD OK\n");
        printf("sd_filesize: %d\n", sd_filesize);
        printf("server_filesize: %d\n", server_filesize);
    #endif


    // 8. Return the SD to the original state
    setDelay( DEF_COMMAND_DELAY );

    if (error_counter == 0)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error counter=0\n");
        #endif
        return 12;
    }

    return 0;
}


/* Function:    This function configures and open a socket
 * Parameters:  socketId: number of the socket Id
 *              protocol: 0 for TCP and 1 for UDP
 *              remote_IP: address of the remote host (IPv6 allowed)
 *              remote_port: remote host port to contact
 * Return:  0 if OK
 *          1 not registered, ME is not currently searching for a new operator to register to
 *          2 not registered, but ME is currently searching for a new operator to register to
 *          3 registration denied
 *          4 unknown
 *          6 not registered, ME is not currently searching for a new operator to register to
 *          8 not registered, but ME is currently searching for a new operator to register to
 *          9 registration denied
 *          10 unknown
 *          12 if error setting APN
 *          13 if error setting login
 *          14 if error setting password
 *          15 if error activating GPRS connection
 *          16 Socket with an active data transfer connection
 *          17 Socket suspended
 *          18 Socket suspended with pending data
 *          19 Socket listening
 *          20 Socket with an incoming connection. Waiting for the user
 *              accept or shutdown command
 *          21 Socket in opening process. The socket is not in Closed state
 *              but still not in Active or Suspended or Suspended with
 *              pending data state
 *          22 if error reading the status of the socket
 *          23 if error configuring of the socket
 *          24 if error configuring of the socket
 *          25 if error sending the open command
 *          26 if timeout openning the socket
 *          27 if timeout openning the socket
 */
uint8_t arduPi4G::openSocketClient(uint8_t socketId,
                                   bool protocol,
                                   char* remote_IP,
                                   uint16_t remote_port)
{
    return openSocketClient(socketId,
                            protocol,
                            remote_IP,
                            remote_port,
                            0,
                            0);
}


/* Function:    This function configures and opens a socket
 * Parameters:  socketId: number of the socket Id
 *              protocol: 0 for TCP and 1 for UDP
 *              remote_IP: address of the remote host (IPv6 allowed)
 *              remote_port: remote host port to contact
 *              local_port: UDP connections local port
 * Return:  0 if OK
 *          1 not registered, ME is not currently searching for a new operator to register to
 *          2 not registered, but ME is currently searching for a new operator to register to
 *          3 registration denied
 *          4 unknown
 *          6 not registered, ME is not currently searching for a new operator to register to
 *          8 not registered, but ME is currently searching for a new operator to register to
 *          9 registration denied
 *          10 unknown
 *          12 if error setting APN
 *          13 if error setting login
 *          14 if error setting password
 *          15 if error activating GPRS connection
 *          16 Socket with an active data transfer connection
 *          17 Socket suspended
 *          18 Socket suspended with pending data
 *          19 Socket listening
 *          20 Socket with an incoming connection. Waiting for the user
 *              accept or shutdown command
 *          21 Socket in opening process. The socket is not in Closed state
 *              but still not in Active or Suspended or Suspended with
 *              pending data state
 *          22 if error reading the status of the socket
 *          23 if error configuring of the socket
 *          24 if error configuring of the socket
 *          25 if error sending the open command
 *          26 if timeout openning the socket
 *          27 if timeout openning the socket
 */
uint8_t arduPi4G::openSocketClient(uint8_t socketId,
                                   bool protocol,
                                   char* remote_IP,
                                   uint16_t remote_port,
                                   uint16_t local_port)
{
    return openSocketClient(socketId,
                            protocol,
                            remote_IP,
                            remote_port,
                            local_port,
                            0);
}


/* Function:    This function configures and opens a socket
 * Parameters:  socketId: number of the socket Id (from 1 to 6)
 *              protocol: 0 for TCP and 1 for UDP
 *              remote_IP: address of the remote host (IPv6 allowed)
 *              remote_port: remote host port to contact
 *              local_port: UDP connections local port
 *              keep_alive: From 0 (disabled) to 240 minutes
 * Return:
 *      0 if OK
 *      1 not registered, ME is not currently searching for a new operator to register to
 *      2 not registered, but ME is currently searching for a new operator to register to
 *      3 registration denied
 *      4 unknown
 *      6 not registered, ME is not currently searching for a new operator to register to
 *      8 not registered, but ME is currently searching for a new operator to register to
 *      9 registration denied
 *      10 unknown
 *      12 if error setting APN
 *      13 if error setting login
 *      14 if error setting password
 *      15 if error activating GPRS connection
 *      16 error getting socket status
 *      17 Socket with an active data transfer connection
 *      18 Socket suspended
 *      19 Socket suspended with pending data
 *      20 Socket listening
 *      21 Socket with an incoming connection. Waiting for the user accept or shutdown command
 *      21 Socket in opening process. The socket is not in Closed state but
 *          still not in Active or Suspended or Suspended with pending data state
 *      22 if error in Socket Configuration
 *      23 if error in Socket Configuration Extended 3
 *      24 if error sending the open command
 *      25 if timeout openning the socket
 */
uint8_t arduPi4G::openSocketClient(uint8_t socketId,
                                   bool protocol,
                                   char* remote_IP,
                                   uint16_t remote_port,
                                   uint16_t local_port,
                                   uint8_t keep_alive)
{
    uint8_t error;
    uint8_t answer;
    char command_buffer[100];

    //// 1. Check data connection
    answer = checkDataConnection(60);
    if (answer != 0)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error checking data connection\n");
        #endif
        return answer;  // 1 to 15 error codes
    }

    //// 2. Check socket status
    answer = getSocketStatus(socketId);

    if (answer != 0)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error checking socket status\n");
        #endif
        return 16;
    }

    // check if socket is not closed before opening it and return error code
    if (socketStatus[socketId].state != 0)
    {
        return socketStatus[socketId].state+16;
    }

    //// 3. Socket Configuration Extended
    #if DEBUG_ARDUPI4G > 1
        printf("Configuring connection\n");
    #endif

    // AT#SCFGEXT=<socketId>,0,0,<keep_alive>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[3])),
                socketId+1,
                0,
                0,
                keep_alive);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();

            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }

        return 22;
    }

    //// 4. Socket Configuration Extended 3
    // AT#SCFGEXT3=<socketId>,1\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[5])), socketId+1);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();

            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }
        return 23;
    }

    //// 5. Socket Dial
    #if DEBUG_ARDUPI4G > 0
        printf("Opening the socket\n");
    #endif

    if (local_port == 0)
    {
        local_port = remote_port;
    }

    // AT#SD=<socketId>,<protocol>,<remote_port>,"<remote_IP>",0,<local_port>,1\r
    // AT#SD=1,0,15010,"intranet.libelium.com",0,4000,1
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[6])), //"AT#SD=%u,%d,%u,\"%s\",0,%u,1\r"
            socketId+1,
            protocol,
            remote_port,
            remote_IP,
            local_port);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();

            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }
        return 24;
    }

    //// 6. Wait for opening the socket
    uint32_t previous = millis();

    do
    {
        // get socket status
        answer = getSocketStatus(socketId);
        if (answer == 0)
        {
            if (socketStatus[socketId].state == 2)
            {
                // Socket suspended
                return 0;
            }
            else if (socketStatus[socketId].state == 3)
            {
                // Socket suspended with pending data
                return 0;
            }
        }

        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();

        delay(1000);

    } while (((millis() - previous) < LE910_IP_TIMEOUT));

    return 25;
}


/* Function:    This function opens a socket listening for an incoming 
 *              connection on a specified port
 * Parameters:  socketId: number of the socket Id (from 1 to 6)
 *              protocol: 0 for TCP_SERVER and 1 for UDP_SERVER
 *              local_port: TCP/UDP local port
 * Return:  
 */
uint8_t arduPi4G::openSocketServer(uint8_t socketId,
                                   bool protocol,
                                   uint16_t local_port)
{
    return openSocketServer(socketId, protocol, local_port, 0);
}


/* Function:    This function opens a socket listening for an incoming 
 *              connection on a specified port
 * Parameters:  socketId: number of the socket Id (from 1 to 6)
 *              protocol: 0 for TCP_SERVER and 1 for UDP_SERVER
 *              local_port: TCP/UDP local port
 *              keep_alive: From 0 (disabled) to 240 minutes
 * Return:  
 */
uint8_t arduPi4G::openSocketServer(uint8_t socketId,
                                   bool protocol,
                                   uint16_t local_port,
                                   uint8_t keep_alive)
{
    uint8_t error;
    uint8_t answer;
    char command_buffer[100];
    
    //// 1. Check data connection
    answer = checkDataConnection(60);
    if (answer != 0)
    {
        #if DEBUG_ARDUPI4G > 0
            PRINT_LE910("Error checking data connection\n");
        #endif
        return answer;  // 1 to 15 error codes
    }
    
    //// 2. Check socket status 
    answer = getSocketStatus(socketId);
    
    if (answer != 0)
    {
        #if DEBUG_ARDUPI4G > 0
            PRINT_LE910("Error checking socket status\n");
        #endif
        return 16;
    }
    
    //// 3. Socket Configuration Extended
    #if DEBUG_ARDUPI4G > 1
        PRINT_LE910("Configuring connection\n");
    #endif
    
    // AT#SCFGEXT=<socketId>,0,0,<keep_alive>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[3])), 
            socketId+1, 
            0,
            0,
            keep_alive);
    
    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);
    
    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
            
            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }
        
        return 22;
    }
    
    //// 4. Socket Listen TCP / Socket Listen UDP
    #if DEBUG_ARDUPI4G > 0
        PRINT_LE910("Opening the socket listen\n");
    #endif
    
    if (protocol == arduPi4G::TCP)
    {
        // AT#SL=<socketId>,1,<local_port>,255\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[29])), 
                socketId+1,
                1,
                local_port,
                255);
    }
    else if (protocol == arduPi4G::UDP)
    {
        // AT#SLUDP=<socketId>,1,<local_port>\r     
        sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[30])), 
                socketId+1,
                1,
                local_port);
    }
    else
    {
        return 23;
    }
    
    // send command 
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
    
    // check answer 
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
            
            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }
        return 24;
    }
    
//~     //// 5. Check Socket Listen status (should be 4)
//~     answer = getSocketStatus(socketId);
//~     
//~     if (answer != 0)
//~     {
//~         #if DEBUG_ARDUPI4G > 0
//~             PRINT_LE910("Error checking socket status\n");
//~         #endif
//~         return 25;
//~     }
//~     
//~     // check if socket is listening
//~     if (socketStatus[socketId].state != 4)
//~     {
//~         return 26;
//~     }   
    
    return 0;   
}


/* Function:    This function configures and opens a socket SSL
 * Parameters:  
 *      socketId: Secure Socket Identifier (must be 1 because until now SSL block manage only 1 socket)
 *      remote_IP: address of the remote host (IPv6 allowed)
 *      remote_port: remote host port to contact
 * Return:  
 *      0 if OK
 *      1 not registered, ME is not currently searching for a new operator to register to
 *      2 not registered, but ME is currently searching for a new operator to register to
 *      3 registration denied
 *      4 unknown
 *      6 not registered, ME is not currently searching for a new operator to register to
 *      8 not registered, but ME is currently searching for a new operator to register to
 *      9 registration denied
 *      10 unknown
 *      12 if error setting APN
 *      13 if error setting login
 *      14 if error setting password
 *      15 if error activating GPRS connection
 *      16 Socket with an active data transfer connection
 *      17 Socket suspended
 *      18 Socket suspended with pending data
 *      19 Socket listening
 *      20 Socket with an incoming connection. Waiting for the user accept or shutdown command
 *      21 Socket in opening process. The socket is not in Closed state but 
 *          still not in Active or Suspended or Suspended with pending data state
 *      22 if error reading the status of the socket
 *      23 if error configuring of the socket
 *      24 if error configuring of the socket
 *      25 if error sending the open command
 *      26 if timeout openning the socket
 *      27 if timeout openning the socket
 */
uint8_t arduPi4G::openSocketSSL(uint8_t socketId,
                                char* remote_IP,
                                uint16_t remote_port)
{
    uint8_t answer;
    char command_buffer[100];
        
    //// 1. Enable a SSL socket
    // "AT#SSLEN=<socketId>,1\r"
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[33])), socketId+1, 1);
    
    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
        
    // check response
    // If the socket has already been enabled for SSL, 
    // the module returns error, so we continue if error
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
    }
    
    //// 2. Check data connection
    answer = checkDataConnection(60);
    if (answer != 0)
    {
        #if DEBUG_ARDUPI4G > 0
            PRINT_LE910("Error in checkDataConnection()\n");
        #endif
        return answer;  // 1 to 15 error codes
    }
    
    //// 3. Check socket status 
    answer = getSocketStatusSSL(socketId);
    
    if (answer != 0)
    {
        #if DEBUG_ARDUPI4G > 0
            PRINT_LE910("Error in getSocketStatusSSL()\n");
        #endif
        return 16;
    }
    
    // check if socket is not closed before opening it and return error code
    // <state> available values are:
    //   0: Socket Disabled
    //   1: Connection closed
    //   2: Connection open
    if (socketStatusSSL[socketId].state != 1)
    {
        return socketStatusSSL[socketId].state+16;
    }
    
    //// 4. Socket Dial
    // AT#SSLD=1,<remote_port>,"<remote_IP>",0,1\r
    sprintf(command_buffer, 
            (char*)pgm_read_word(&(table_IP[12])),
            remote_port,
            remote_IP);
    
    // send command 
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 15000);
    
    // check answer 
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
            
            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }
        return 26;
    }
    
    //// 5. Wait for opening the socket 
    // <state> available values are:
    //   0: Socket Disabled
    //   1: Connection closed
    //   2: Connection open
    uint32_t previous = millis();
    
    do
    {
        // get socket status
        answer = getSocketStatusSSL(socketId);
        
        if (answer == 0)
        {
            if (socketStatusSSL[socketId].state == 2)
            {
                // Socket suspended
                return 0;
            }
        }
        
        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();
        
        delay(1000);
        
    } while (((millis() - previous) < LE910_IP_TIMEOUT));
    
    return 27;
}


/* Function:    This function closes a socket previously opened
 * Parameters:  socketId: number of the socket Id
 * Return:  0 Socket Closed
 *          1 if error sending the command
 *          2 if timeout shutting down the socket
 */
uint8_t arduPi4G::closeSocketClient(uint8_t socketId)
{
    uint8_t answer;
    char command_name[10];
    char command_buffer[10];

    //// 1. Check socket status
    answer = getSocketStatus(socketId);

    if ((answer == 0) && (socketStatus[socketId].state == 0))
    {
        #if DEBUG_ARDUPI4G > 1
            printf("Socket status: closed\n");
        #endif
        return 0;
    }

    //// 2. Configure connection
    // AT#SH=<socketId>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[7])), socketId+1);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

    // check answer
    if(answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }

        #if DEBUG_ARDUPI4G > 0
            printf("Error closing connection\n");
        #endif
        return 1;
    }

    //// 3. Wait the shutdown of the socket
    uint32_t previous = millis();

    do
    {
        // get socket status
        answer = getSocketStatus(socketId);
        if (answer != 0)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error getting socket status\n");
            #endif
            return 2;
        }

        if (socketStatus[socketId].state == 0)
        {
            #if DEBUG_ARDUPI4G > 1
                printf("Socket closed\n");
            #endif
            return 0;
        }

        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();

        delay(1000);

    } while ((millis() - previous) < LE910_IP_TIMEOUT);

    return 3;
}


/* Function:    This function closes a socket previously opened
 * Parameters:  socketId: number of the socket Id
 * Return:  0 Socket Closed
 *          1 if error sending the command
 *          2 if timeout shutting down the socket
 */
uint8_t arduPi4G::closeSocketServer(uint8_t socketId, uint8_t protocol)
{
    uint8_t answer;
    char command_buffer[10];
                
    //// 1. Check socket status 
    answer = getSocketStatus(socketId);
    
    if ((answer == 0) && (socketStatus[socketId].state == 0))
    {
        #if DEBUG_ARDUPI4G > 1
            PRINT_LE910("Socket status: closed\n");
        #endif
        return 0;
    }
    
    
    //// 2. Close socket listening
    if (protocol == arduPi4G::TCP)
    {
        // AT#SL=<socketId>,0,<local_port>,255\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[29])), 
                socketId+1,
                0,
                socketStatus[socketId].localPort,
                255);
    }
    else if (protocol == arduPi4G::UDP)
    {
        // AT#SLUDP=<socketId>,0,<local_port>\r     
        sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[30])), 
                socketId+1,
                0,
                socketStatus[socketId].localPort);
    }
    else
    {
        return 1;
    }   
    
    // send command 
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
    
    // check answer 
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
            
            #if DEBUG_ARDUPI4G > 0
                printErrorCode();
            #endif
        }
        return 1;
    }
    
    //// 3. Configure connection    
    // AT#SH=<socketId>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[7])), socketId+1);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
    
    // check answer
    if(answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        
        #if DEBUG_ARDUPI4G > 0
            PRINT_LE910("Error closing connection\n");
        #endif
        return 1;
    }

    //// 3. Wait the shutdown of the socket 
    uint32_t previous = millis();
    
    do
    {
        // get socket status
        answer = getSocketStatus(socketId);
        if (answer != 0)
        {
            #if DEBUG_ARDUPI4G > 0
                PRINT_LE910("Error getting socket status\n");
            #endif
            return 2;
        }
        
        if (socketStatus[socketId].state == 0)
        {
            #if DEBUG_ARDUPI4G > 1
                PRINT_LE910("Socket closed\n");
            #endif
            return 0;
        }
        
        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();
        
        delay(1000);
        
    } while ((millis() - previous) < LE910_IP_TIMEOUT);
    
    return 1;
}


/* Function:    This function closes a socket previously opened
 * Parameters:  socketId: number of the socket Id
 * Return:  0 Socket Closed
 *          1 if error sending the command
 *          2 if timeout shutting down the socket
 */
uint8_t arduPi4G::closeSocketSSL(uint8_t socketId)
{
    uint8_t answer, counter;
    char command_name[10];
    char command_buffer[10];

    //// 1. Check socket status
    answer = getSocketStatusSSL(socketId);

    if ((answer == 0) && (socketStatusSSL[socketId].state == 1))
    {
        #if DEBUG_ARDUPI4G > 1
            printf("Socket status: closed\n");
        #endif
        return 0;
    }

    //// 2. Configure connection
    // AT#SSLH=<socketId>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[19])), socketId+1);

    // send command
    counter = 3;
    answer = 0;
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
    while ((counter > 0) && (answer != 1))
    {
        delay(3000);
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
        counter--;
    }

    // check answer
    if(answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }

        #if DEBUG_ARDUPI4G > 0
            printf("Error closing connection\n");
        #endif
        return 1;
    }

    //// 3. Wait the shutdown of the socket
    uint32_t previous = millis();

    do
    {
        // get socket status
        answer = getSocketStatusSSL(socketId);
        if (answer != 0)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error getting socket status\n");
            #endif
            return 2;
        }

        if (socketStatusSSL[socketId].state == 1)
        {
            #if DEBUG_ARDUPI4G > 1
                printf("Socket closed\n");
            #endif

            //// 4. disable SSL channel
            // AT#SSLEN=<SSId>, <Enable>\r
            sprintf( command_buffer,
                     (char*)pgm_read_word(&(table_IP[33])),
                     0); // Disable

            // send command
            answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

            // check response
            if (answer != 1)
            {
                if (answer == 2)
                {
                    getErrorCode();
                }
                return 1;
            }

            return 0;
        }

        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();

        delay(1000);

    } while ((millis() - previous) < LE910_IP_TIMEOUT);

    return 3;
}


/* Function:    This function sends data through a socket previously opened
 * Parameters:  socketId: number of the socket Id
 *              data: string with the data
 * Return:  0 if OK
 *          1 if error with the socket
 *          2 if error sending the command
 *          3 if error sending the command
 *          4 if timeout shutting down the socket
 */
uint8_t arduPi4G::send(uint8_t socketId, char* data)
{
    return send(socketId, (uint8_t*)data, strlen(data));
}

/* Function:    This function sends data through a socket previously opened
 * Parameters:  socketId: number of the socket Id
 *              data: buffer with the data
 *              data_length: length of the data buffer
 * Return:      0 if OK, 'x' error
 */
uint8_t arduPi4G::send(uint8_t socketId,
                       uint8_t* data,
                       uint16_t data_length)
{
    uint8_t answer;
    char command_name[20];
    char command_buffer[25];

    //// 1. Check socket status
    answer = getSocketStatus(socketId);

    // check answer error
    if (answer != 0)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error checking socket status\n");
        #endif
        return 1;
    }

    // check correct socket status
    if ((socketStatus[socketId].state != 1) &&
        (socketStatus[socketId].state != 2) &&
        (socketStatus[socketId].state != 3))
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Socket status incorrect\n");
        #endif
        return 2;
    }

    //// 2. Send data
    // AT#SSENDEXT=<socketId>,<data_length>\r
    sprintf(command_buffer,
            (char*)pgm_read_word(&(table_IP[8])),
            socketId+1,
            data_length);

    // send command
    answer = sendCommand(command_buffer, (char*)">", LE910_ERROR_CODE, LE910_ERROR);

    // check answer
    if(answer != 1)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error sending data\n");
        #endif
        return 3;
    }

    // send array of data
    for (int i = 0; i < data_length; i++)
    {
        printByte(data[i], UART0);
    }

    // wait for "OK"
    answer = waitFor(LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

    // check answer
    if (answer != 1)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error waiting OK\n");
        #endif
        return 4;
    }

    //// 3. Wait that all data have been sent
    uint32_t previous = millis();

    do
    {
        // get socket status
        answer = getSocketStatus(socketId);
        if (answer != 0)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error getting socket status\n");
            #endif
            return 5;
        }

        if (socketStatus[socketId].state == 2)
        {
            #if DEBUG_ARDUPI4G > 1
                printf("Data sent\n");
            #endif
            return 0;
        }

        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();

        delay(1000);

    } while ((millis() - previous) < LE910_IP_TIMEOUT);

    return 6;
}


/* Function:    This function sends data through a socket previously opened
 * Parameters:  socketId: number of the socket Id
 *              data: string with the data
 * Return:  0 if OK
 *          1 if error with the socket
 *          2 if error sending the command
 *          3 if error sending the command
 *          4 if timeout shutting down the socket
 */
uint8_t arduPi4G::sendSSL(uint8_t socketId, char* data)
{
    return sendSSL(socketId, (uint8_t*)data, strlen(data));
}


/* Function:    This function sends data through a socket previously opened
 * Parameters:  socketId: number of the socket Id
 *              data: buffer with the data
 *              data_length: length of the data buffer
 * Return:      0 if OK, 'x' error
 */
uint8_t arduPi4G::sendSSL(uint8_t socketId,
                          uint8_t* data,
                          uint16_t data_length)
{
    uint8_t answer, counter;
    char command_name[20];
    char command_buffer[25];

    //// 1. Check socket status
    answer = getSocketStatusSSL(socketId);

    // check answer error
    if (answer != 0)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error checking socket status\n");
        #endif
        return 1;
    }

    // check correct socket status
    if (socketStatusSSL[socketId].state != 2)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Socket status incorrect\n");
        #endif
        return 2;
    }

    //// 2. Send data
    // AT#SSLSEND=<socketId>\r
    sprintf(command_buffer,
            (char*)pgm_read_word(&(table_IP[14])),
            socketId+1,
            data_length);

    // send command
    answer = sendCommand(command_buffer, (char*)">", LE910_ERROR_CODE, LE910_ERROR);
    delay(500);

    // check answer
    if(answer != 1)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error sending data\n");
        #endif
        return 3;
    }

    // send array of data
    for (int i = 0; i < data_length; i++)
    {
        printByte(data[i], UART0);
    }

    // send  data with 0x1A (Ctl+Z)
    printByte(0x1A, UART0);
    printString((char *)"\r", UART0);

    // wait for "OK"
    answer = waitFor(LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

    // check answer
    if (answer != 1)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error waiting OK\n");
        #endif
        return 4;
    }

    //// 3. Wait that all data have been sent
    uint32_t previous = millis();

    do
    {
        // get socket status
        answer = getSocketStatusSSL(socketId);
        if (answer != 0)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error getting socket status\n");
            #endif
            return 5;
        }

        if (socketStatusSSL[socketId].state == 2)
        {
            #if DEBUG_ARDUPI4G > 1
                printf("Data sent\n");
            #endif
            return 0;
        }

        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();

        delay(1000);

    } while ((millis() - previous) < LE910_IP_TIMEOUT);

    return 6;
}


/* Function:    This function read data received in the module
 * Parameters:  socketId: number of the socket Id
 * Return: length of the data readed
 *          -1 if answer_buffer is full
 *          -2 if error reading the amount of incoming bytes
 */
uint8_t arduPi4G::receive(uint8_t socketId)
{
    return receive(socketId, 0);
}


/* Function:    This function read data received in the module
 * Parameters:  socketId: number of the socket Id
 *              timeout: number of ms to wait for incoming bytes
 * Return: length of the data readed
 *          -1 if answer_buffer is full
 *          -2 if error reading the amount of incoming bytes
 */
uint8_t arduPi4G::receive(uint8_t socketId, uint32_t timeout)
{
    uint8_t answer;
    int incoming_bytes;
    char command_name[25];
    char command_buffer[25];
    char command_answer[25];
    uint32_t nBytes = 0;
    uint32_t readBufferBytes = 0;
    uint32_t previous;

    previous = millis();

    //// 1. Get socket pending bytes to be read for specified 'timeout' in the
    // case the timeout is zero, do it once
    do
    {
        // get socket info
        answer = getSocketInfo(socketId);

        if (answer != 0)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error getting socket info\n");
            #endif
            return 1;
        }

        if (socketInfo[socketId].size > 0)
        {
            break;
        }

        if (timeout == 0)
        {
            return 1;
        }

        delay(500);
    }
    while (millis()-previous < timeout);

    if (socketInfo[socketId].size == 0)
    {
        return 1;
    }

    // get number of pending bytes to be read in socket
    incoming_bytes = socketInfo[socketId].size;

    //// 2. Send command to read received data and save it
    // generate the expected response
    // AT#SRECV: <socketId>,
    sprintf(command_answer,(char*)pgm_read_word(&(table_IP[9])), socketId+1);

    // generate command
    // AT#SRECV=<socketId>,<LE910_MAX_DL_PAYLOAD>\r
    sprintf(command_buffer,(char*)pgm_read_word(&(table_IP[26])),
            socketId+1,
            LE910_MAX_DL_PAYLOAD);

    // send command
    answer = sendCommand(command_buffer, command_answer, 2000);

    // check answer
    if (answer == 1)
    {
        // wait for end of response "<nBytes>\r\n"
        answer = waitFor((char*)"\r\n", 1000);

        if (answer != 1)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error waiting EOL\n");
            #endif
            return 1;
        }

        // parse <nBytes>
        answer = parseUint32(&nBytes, (char*)" \r\n");

        if (answer != 0)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error parsing nBytes\n");
            #endif
            return 1;
        }

        #if DEBUG_ARDUPI4G > 1
            printf("nBytes: %d\n", nBytes);
        #endif

        // wait for number of bytes in data received
        readBufferBytes = readBuffer(nBytes);

        #if DEBUG_ARDUPI4G > 1
            printf("readBufferBytes: %d\n", readBufferBytes);
        #endif

        if (readBufferBytes != nBytes)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error getting received bytes\n");
            #endif
            return 1;
        }
    }

    // update attribute length
    _length = nBytes;

    return 0;
}


/* Function:    This function read data received in the module
 * Parameters:  socketId: number of the socket Id
 * Return:  0 if ok
 *          'x' if error
 */
uint8_t arduPi4G::receiveSSL(uint8_t socketId)
{
    return receiveSSL(socketId, 0);
}


/* Function:    This function read data received in the module
 * Parameters:  socketId: number of the socket Id
 *              timeout: number of ms to wait for incoming bytes
 * Return:  0 if ok
 *          'x' if error
 */
uint8_t arduPi4G::receiveSSL(uint8_t socketId, uint32_t timeout)
{
    uint8_t answer;
    int incoming_bytes;
    char command_name[25];
    char command_buffer[50];
    char answer1[25];
    char answer2[25];
    char answer3[25];
    uint32_t nBytes = 0;
    uint32_t readBufferBytes = 0;
    uint32_t previous;

    previous = millis();

    // generate command
    // AT#SSLRECV=<socketId>,<MaxNumBytes>\r
    sprintf(command_buffer,
            (char*)pgm_read_word(&(table_IP[15])),
            socketId+1,
            LE910_MAX_DL_PAYLOAD);

    // Generate answers
    strcpy(answer1, (char*)pgm_read_word(&(table_IP[16])));   //"#SSLRECV: "
    strcpy(answer2, (char*)pgm_read_word(&(table_IP[17])));   //"TIMEOUT\r\n"
    strcpy(answer3, (char*)pgm_read_word(&(table_IP[18])));   //"DISCONNECTED\r\n"

    //// 1. Send command to read received data and save it
    do
    {
        // send command to read data from SSL socket
        answer = sendCommand(command_buffer, answer1, answer2, answer3, LE910_ERROR_CODE, 5000);

        // check timeout with no answer
        if (answer == 0)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("No answer from module\n");
            #endif
            return 1;
        }

        // cehck OK response with incoming data, then break
        if (answer == 1)
        {
            break;
        }

        // check OK response with incoming data, then break
        if (answer == 3)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("SSL Socket DISCONNECTED\n");
            #endif
            return 2;
        }

        // check error code
        if (answer == 4)
        {
            getErrorCode();
            return 3;
        }

        delay(500);
    }
    while ((millis()-previous < timeout));

    //// 2. If bytes have been received, then proceed to read them
    if (answer == 1)
    {
        // wait for end of response "<nBytes>\r\n"
        answer = waitFor((char*)"\r\n", 1000);

        if (answer != 1)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error waiting EOL\n");
            #endif
            return 4;
        }

        // parse <nBytes>
        answer = parseUint32(&nBytes, (char*)" \r\n");

        if (answer != 0)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error parsing nBytes\n");
            #endif
            return 5;
        }

        #if DEBUG_ARDUPI4G > 1
            printf("nBytes: %d\n", nBytes);
        #endif

        // wait for number of bytes in data received
        readBufferBytes = readBuffer(nBytes);

        #if DEBUG_ARDUPI4G > 1
            printf("readBufferBytes: %d\n", readBufferBytes);
        #endif

        if (readBufferBytes != nBytes)
        {
            #if DEBUG_ARDUPI4G > 0
                printf("Error getting received bytes\n");
            #endif
            return 6;
        }
    }
    else
    {
        #if DEBUG_ARDUPI4G > 0
            printf("Error waiting LE910_OK\n");
        #endif
        return 7;
    }

    // update attribute length
    _length = nBytes;

    return 0;
}


/* Function:    This function allows to store, delete and read security data
 *              (Certificate, CAcertificate, private key) into the non-volatile
 *              memory of the module
 *
 * Parameters:  socketId: Secure Socket Identifier (must be 1)
 *              action: Action to do:
 *                  0 – Delete data from NVM
 *                  1 – Store data into NVM
 *                  2 – Read data from NVM
 *              dataType:
 *                  0 – Certificate
 *                  1 – CA certificate
 *                  2 – RSA Private key
 *              data: string with the security data
 *
 * Return:      0 if OK; 'x' if error
 */
uint8_t arduPi4G::manageSSL(uint8_t socketId,
                            uint8_t action,
                            uint8_t dataType)
{
    return manageSSL(socketId, action, dataType, NULL);
}


/* Function:    This function allows to store, delete and read security data
 *              (Certificate, CAcertificate, private key) into the non-volatile
 *              memory of the module
 *
 * Parameters:  socketId: Secure Socket Identifier (must be 1)
 *              action: Action to do:
 *                  0 – Delete data from NVM
 *                  1 – Store data into NVM
 *                  2 – Read data from NVM
 *              dataType:
 *                  0 – Certificate
 *                  1 – CA certificate
 *                  2 – RSA Private key
 *              data: string with the security data
 *
 * Return:      0 if OK; 'x' if error
 */
uint8_t arduPi4G::manageSSL(uint8_t socketId,
                            uint8_t action,
                            uint8_t dataType,
                            char *data)
{
    uint8_t answer;
    char command_buffer[50];
    char command_answer[50];

    // clear buffers
    memset(command_buffer,0x00,sizeof(command_buffer));
    memset(command_answer,0x00,sizeof(command_answer));

    //// 1. Enable a SSL socket
    // AT#SSLEN=<SSId>, <Enable>\r
    sprintf( command_buffer,
             (char*)pgm_read_word(&(table_IP[33])),
             1); // Enable

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

    // check response
    // If the socket has already been enabled for SSL,
    // the module returns error, so we continue if error
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
    }

//oJo esto no lo pone Yuri
/*
    //// Configure security parameters of a SSL socket
    // AT#SSLSECCFG=<SSId>,<CipherSuite>,<SecLevel>
    sprintf( command_buffer,
             (char*)pgm_read_word(&(table_IP[34])),
             0,  // Chiper Suite is chosen by remote Server [default]
             1); // Manage server authentication

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);

    // check response
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        //return 1;
    }
*/
    //// 2. Set SSL settings
    if ((action == SSL_ACTION_STORE) && (data != NULL))
    {
        // AT#SSLSECDATA=<socketId>,<action>,<dataType>,<data_length>\r
        sprintf(  command_buffer,
                    (char*)pgm_read_word(&(table_IP[10])),
                    socketId+1,
                    action,
                    dataType,
                    strlen(data));

        // send command
        answer = sendCommand(command_buffer, (char*)">", LE910_ERROR_CODE, LE910_ERROR);

        // check response
        if (answer != 1)
        {
            if (answer == 2)
            {
                getErrorCode();
            }
            return 2;
        }

        // send certificate data
        printString(data, UART0);
        printByte(0x1A, UART0);

        waitFor(LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);

        // check response
        if (answer != 1)
        {
            if (answer == 2)
            {
                getErrorCode();
            }
            return 3;
        }

        return 0;
    }
    else if (action == SSL_ACTION_READ)
    {
        // command_buffer <-- AT#SSLSECDATA=<socketId>,<action>,<dataType>\r
        sprintf(  command_buffer,
                  (char*)pgm_read_word(&(table_IP[11])),
                  socketId+1,
                  action,
                  dataType);

        // command_answer <-- "#SSLSECDATA: "
        strcpy( command_answer, (char*)pgm_read_word(&(table_IP[27])));

        // send command
        answer = sendCommand(command_buffer, command_answer, LE910_ERROR_CODE, LE910_ERROR);

        // check response
        if (answer != 1)
        {
            if (answer == 2)
            {
                getErrorCode();
            }
            return 4;
        }

        waitFor((char*)"\r\n");
        waitFor(LE910_OK);

        return 0;

    }
    else if (action == SSL_ACTION_DELETE)
    {
        // AT#SSLSECDATA=<socketId>,<action>,<dataType>\r
        sprintf(  command_buffer,
                  (char*)pgm_read_word(&(table_IP[11])),
                  socketId+1,
                  action,
                  dataType);

        // send command
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);

        // check response
        if (answer != 1)
        {
            if (answer == 2)
            {
                getErrorCode();
            }
            return 5;
        }

        return 0;
    }

    return 6;
}


/* Function:    Starts the GPS engine with hot start and stand alone mode
 * Parameters:
 *  reset_mode:
 *      1 Coldstart
 *      2 Warmstart
 *      3 Hotstart
 *  gps_mode:
 *      0: Pure MS Assisted - Location estimate from the network (MS Assisted mode).
 *      1: MS Based - Assistance Data from the network (MS Based mode).
 *      2: MS Assisted Based - Combination of MS-A and MS-B modes, location
 *      estimate computed both at UE and Network.
 *      3: Autonomous – Autonomous GPS mode of operation.
 *
 * Return:  0 if OK
 *          1 if GPS is powered yet
 *          2 if error setting the reset mode
 *          3 if error starting the GPS engine
 */
uint8_t arduPi4G::gpsStart()
{
    return gpsStart(arduPi4G::GPS_AUTONOMOUS, 3);
}


/* Function:    Starts the GPS engine with the reset and location mode desired
 * Parameters:
 *  reset_mode:
 *      1 Coldstart
 *      2 Warmstart
 *      3 Hotstart
 *  gps_mode:
 *      0: Pure MS Assisted - Location estimate from the network (MS Assisted mode).
 *      1: MS Based - Assistance Data from the network (MS Based mode).
 *      2: MS Assisted Based - Combination of MS-A and MS-B modes, location
 *      estimate computed both at UE and Network.
 *      3: Autonomous – Autonomous GPS mode of operation.
 *
 * Return:  0 if OK
 *          1 if GPS is powered yet
 *          2 if error setting the reset mode
 *          3 if error starting the GPS engine
 */
uint8_t arduPi4G::gpsStart(uint8_t gps_mode)
{
    return gpsStart(gps_mode,3);
}


/* Function:    Starts the GPS engine with the reset and location mode desired
 * Parameters:
 *  reset_mode:
 *      1 Coldstart
 *      2 Warmstart
 *      3 Hotstart
 *  gps_mode:
 *      0: Pure MS Assisted - Location estimate from the network (MS Assisted mode).
 *      1: MS Based - Assistance Data from the network (MS Based mode).
 *      2: MS Assisted Based - Combination of MS-A and MS-B modes, location
 *      estimate computed both at UE and Network.
 *      3: Autonomous – Autonomous GPS mode of operation.
 *
 * Return:  0 if OK
 *          1 if GPS is powered yet
 *          2 if error setting the reset mode
 *          3 if error starting the GPS engine
 */
uint8_t arduPi4G::gpsStart(uint8_t gps_mode, uint8_t reset_mode)
{
    uint8_t answer;
    char command_name[20];
    char command_buffer[50];
    char command_answer[10];

    //// 1. Set reset mode
    // AT$GPSR=<reset_mode>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[2])), reset_mode);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }

    //// 2. Check if the GPS engine is powered on
    //// In that case, we return OK; if not we start the gps power
    // AT$GPSP?\r
    strcpy(command_buffer, (char*)pgm_read_word(&(table_GPS[13])));

    // Generate answer to be parsed
    // "GPSP: 1"
    strcpy(command_answer, (char*)pgm_read_word(&(table_GPS[14])));

    // send command
    answer = sendCommand(command_buffer, command_answer, LE910_ERROR_CODE, LE910_ERROR, 1000);

    // check answer
    if (answer == 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 0;
    }
    else if ((answer == 2) || (answer == 3))
    {
        return 2;
    }

    //// 3. Switch GPS to on
    // Send corresponding command depending on gps mode
    if (gps_mode == arduPi4G::GPS_AUTONOMOUS)
    {
        //// GPS Power Management: GPS controller is powered up
        // "AT$GPSP=1\r"
        sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[0])), 1);

        // send command
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 1000);

        // check answer
        if (answer != 1)
        {
            if (answer == 2)
            {
                getErrorCode();
            }
            return 3;
        }

        // return OK
        return 0;
    }
    else if ((gps_mode == arduPi4G::GPS_MS_ASSISTED) || (gps_mode == arduPi4G::GPS_MS_BASED))
    {
        //// 4. setup NETWORK_UTRAN
        answer = setWirelessNetwork(arduPi4G::NETWORK_UTRAN);
        if (answer != 0)
        {
            return 5;
        }

        //// 5. Define PDP Context
        // AT+CGDCONT=1,"IP","<APN>"\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_4G[29])), _apn);
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);
        if (answer != 1)
        {
            return 6;
        }

        //// 6. Set Authentication User ID
        // AT#USERID="<login>"\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_4G[5])), _apn_login);
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);
        if (answer != 1)
        {
            return 7;
        }

        //// 7. Set Authentication Password
        // AT#PASSW="<pass>"\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_4G[6])), _apn_password);
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 5000);
        if (answer != 1)
        {
            return 8;
        }

        //// 8.
        // AT#SCFG=1,1,300,90,600,50\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_IP[2])), 1, 1, 300, 90, 600, 50);
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
        if (answer != 1)
        {
            return 9;
        }

        //// 3.2. Set QoS for GPS:
        //  horiz_accuracy  = 5
        //  vertic_accuracy = 5
        //  rsp_time = 100
        //  age_of_location_info = 0
        //  location_type = 0 (current location)
        //  nav_profile = 0 Car navigation profile (default)
        //  velocity_request = TRUE
        answer = gpsSetQualityOfService(5, 5, 100, 0, 0, 0, 1);

        if (answer != 0)
        {
            return 10;
        }

        answer = sendCommand((char *)"AT$SLP=1,\"supl.nokia.com:7275\"\r", LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
        if (answer != 1)
        {
            return 11;
        }

        //// 3.3. Set the version of supported SUPL
        // AT$SUPLV=%u\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[20])), 1);

        // send command
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 1000);
        if (answer != 1)
        {
            if (answer == 2)
            {
                getErrorCode();
            }
            return 12;
        }

        answer = sendCommand((char *)"AT$LCSTER=1,,,1\r", LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
        if (answer != 1)
        {
            return 13;
        }

        answer = sendCommand((char *)"AT$LICLS=1\r", LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
        if (answer != 1)
        {
            return 14;
        }

        answer = sendCommand((char *)"AT$LCSLK=1,1\r", LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
        if (answer != 1)
        {
            return 15;
        }

        answer = sendCommand((char *)"AT$GPSGLO=1\r", LE910_OK, LE910_ERROR_CODE, LE910_ERROR);
        if (answer != 1)
        {
            return 16;
        }

        //// 3.4. GPS Start Location Service Request:
        // AT$GPSSLSR=<transport_protocol>,<gps_mode>,,,,,1\r
        sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[1])),
                    1,
                    gps_mode,
                    1);

        // send command
        answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 1000);
        if (answer != 1)
        {
            if (answer == 2)
            {
                getErrorCode();
            }
            return 17;
        }

        // check connection
        answer = checkDataConnection(60);
        if (answer != 0)
        {
            return 18;
        }

        // return OK
        return 0;
    }
    else
    {
        return 7;
    }

    return 0;
}


/* Function: Stops the GPS engine
 * Return:  '0' if OK; '1' error
 */
uint8_t arduPi4G::gpsStop()
{
    uint8_t answer;
    char command_buffer[20];

    // AT$GPSP=0\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[0])), 0);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 1000);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }

    return 0;
}


/* Function:    Checks the GPS and parses GPS data if the sats are fixed
 * Return:  '0' if OK; 'x' error
 */
int8_t arduPi4G::checkGPS()
{
    char *pointer;
    uint8_t answer;
    char command_name[20];
    char command_buffer[20];
    char command_pattern[20];

    //// 1. Check if the GPS position is fixed
    // AT$GPSACP\r
    strcpy(command_buffer, (char*)pgm_read_word(&(table_GPS[4])));

    // pattern <--- "$GPSACP: ,"
    strcpy(command_pattern, (char*)pgm_read_word(&(table_GPS[15])));

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }

    // check if there is not any available data in _buffer
    // If so, _buffer has the following string within it:
    // "$GPSACP: ,,,...."
    if (find(_buffer, _length, command_pattern) == true)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("pattern not found\n");
        #endif
        return 2;
    }

    //// 2. Parses the data
    // Example below:
    /* $GPSACP: <hhmmss.sss>,       // field 1: UTC time
     *          <ddmm.mmmm><N/S>,   // field 2: latitude
     *          <dddmm.mmmm><E/W>,  // field 3: longitude
     *          <x.x>,              // field 4: HDOP
     *          <xxxx.x>,           // field 5: altitude
     *          <fix>,              // field 6: fix mode
     *          <ddd.mm>,           // field 7: Course over ground
     *          <xxxx.x>,           // field 8: Speed over ground (Km/hr)
     *          <xxxx.x>,           // field 9: Speed over ground (knots)
     *          <ddmmyy>,           // field 10: Date
     *          <nsat>              // field 11: Number of satellites
     */
    // $GPSACP: 073041.000,4139.7780N,00051.3417W,500.0,-394.4,2,0.0,0.0,0.0,050515,00

    // pattern <--- "\r\n$GPSACP: .,"
    strcpy(command_pattern, (char*)pgm_read_word(&(table_GPS[16])));

    // skip first characters
    pointer = strtok((char*)_buffer, command_pattern);
    if (pointer == NULL)
    {
        return 3;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("UTC: %s\n", pointer);
    #endif

    /// field 1: Get UTC time
    memset(_time, 0x00, sizeof(_time));
    strncpy(_time, pointer, 6);

    // skip ','
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 4;
    }

    // skip '.000' after time field
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 5;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("Latitude: %s\n", pointer);
    #endif

    /// field 2: Get latitude
    memset(_latitude, '\0', sizeof(_latitude));
    strcpy(_latitude, pointer);

    /// Get NS_indicator
    _latitudeNS = _latitude[strlen(_latitude)-1];
    _latitude[strlen(_latitude)-1] = '\0';

    // skip ','
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 6;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("Longitude: %s\n", pointer);
    #endif

    /// field 3: Get longitude
    memset(_longitude, 0x00, sizeof(_longitude));
    strcpy(_longitude, pointer);

    /// Get EW_indicator
    _longitudeEW = _longitude[strlen(_longitude)-1];
    _longitude[strlen(_longitude)-1] = '\0';

    // skip ','
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 7;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("HDOP: %s\n", pointer);
    #endif

    /// field 4: Get HDOP
    _hdop = atof(pointer);

    // skip ','
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 8;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("Altitude: %s\n", pointer);
    #endif

    /// field 5: Get altitude
    _altitude = atof(pointer);

    // skip ','
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 9;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("Fix mode: %s\n", pointer);
    #endif

    /// field 6: Get fix mode
    _fixMode = atoi(pointer);

    // skip ','
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 10;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("Course over ground: %s\n", pointer);
    #endif

    /// field 7: Get course over ground
    memset(_courseOG, 0x00, sizeof(_courseOG));
    strcpy(_courseOG, pointer);

    // skip ','
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 11;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("Speed over ground (kmh): %s\n", pointer);
    #endif

    /// field 8: Get Speed over ground (Km/hr)
    _speedOG = atof(pointer);

    // skip ','
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 12;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("Speed over ground (knots): %s\n", pointer);
    #endif

    // skip ','
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 13;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("Get date: %s\n", pointer);
    #endif

    /// field 10: Get date
    memset(_date, 0x00, sizeof(_date));
    strncpy(_date, pointer, 6);

    // skip ','
    pointer = strtok(NULL, ",");
    if (pointer == NULL)
    {
        return 14;
    }

    #if DEBUG_ARDUPI4G > 1
        printf("Num Satellites: %s\n", pointer);
    #endif

    /// field 11: Get satellites in use
    _numSatellites = (uint8_t)atoi(pointer);

    return 0;
}


/*
 * convert2Degrees ( input , indicator ) - performs the conversion from input
 * parameters in  DD°MM.mmm’ notation to DD.dddddd° notation.
 *
 * Sign '+' is set for positive latitudes/longitudes (North, East)
 * Sign '-' is set for negative latitudes/longitudes (South, West)
 *
 * parameter: 'input' is a string indicating the latitude/longitude in
 * DDDmm.mmmm' notation for latitude and DDDmm.mmmm' notation for longitude
 * parameter: 'indicator' is a char indicating 'N' for North, 'S' for South, 'E'
 * for East and 'W' for West
 * Returns: a float indicating the latitude in DD.dddddd° notation
 *
 */
float arduPi4G::convert2Degrees(char* input, char indicator)
{
    // final latitude expresed in degrees
    float degrees;
    float minutes;

    //auxiliar variable
    char aux[10] ="";

    // check if 'indicator' is a valid input
    if ( indicator != 'N' && indicator != 'S' && indicator != 'E' && indicator != 'W' )
    {
        // invalid indicator
        return 0;
    }

    // get 'degrees' from input parameter
    if ( indicator=='N' || indicator=='S' )
    {
        //latitude format: DDmm.mmmm'
        aux[0] = input[0];
        aux[1] = input[1];
        aux[2] = '\0';
    }
    else if( indicator == 'E' || indicator == 'W')
    {
        //longitude format: DDDmm.mmmm'
        aux[0]=input[0];
        aux[1]=input[1];
        aux[2]=input[2];
        aux[3]='\0';
    }

    // convert string to integer and add it to final float variable
    degrees = atoi(aux);

    // get 'minutes' from input parameter
    if ( indicator == 'N' || indicator == 'S' )
    {
        //latitude format: DDmm.mmmm'
        for ( int i=0; i<7; i++ )
        {
            aux[i] = input[i+2];
        }
        aux[7] = '\0';
    }
    else if( indicator == 'E' || indicator == 'W')
    {
        //latitude format: DDmm.mmmm'
        for ( int i = 0; i < 7; i++ )
        {
            aux[i] = input[i+3];
        }
        aux[7] = '\0';
    }

    // convert string to integer and add it to final float variable
    minutes = atof(aux);

    // add minutes to degrees
    degrees = degrees+minutes/60;

    // add sign: '+' for North/East; '-' for South/West
    if( indicator == 'S' || indicator == 'W')
    {
        degrees *= -1.0;
    }

    return degrees;
}


/* Function:    It waits for satellites signal
 * Return:
 */
uint8_t arduPi4G::waitForSignal()
{
    return waitForSignal(60000, -1.0);
}


/* Function:    It waits for satellites signal
 * Parameters:  timeout: milliseconds to wait for signal
 * Return:
 */
uint8_t arduPi4G::waitForSignal(uint32_t timeout)
{
    return waitForSignal(timeout, -1.0);
}


/* Function:    It waits for satellites signal
 * Parameters:  timeout: milliseconds to wait for signal
 * Return:
 */
uint8_t arduPi4G::waitForSignal(uint32_t timeout, float desired_HDOP)
{
    int8_t answer = 0;
    uint32_t previous;

    // get current time
    previous = millis();

    // update variable status
    _fixMode = 0;

    //// 1. get current gps status
    while((_fixMode != 2) && (_fixMode != 3))
    {
        answer = checkGPS();

        if (answer == 0)
        {
            if ((_fixMode == 2) || (_fixMode == 3))
            {
                // Good options:
                //  2 - 2D fix
                //  3 - 3D fix
                #if DEBUG_ARDUPI4G > 1
                    printf("_fixMode ok\n");
                #endif
                break;
            }
        }

        #if DEBUG_ARDUPI4G > 1
            printf("_fixMode NOT ok\n");
        #endif

        delay(500);

        if (millis()-previous > timeout)
        {
            // timeout error
            #if DEBUG_ARDUPI4G > 0
                printf("_fixMode error, exit function\n");
            #endif
            return 1;
        }

        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();
    }

    //// 2. Check if desired HDOP is correct
    if (desired_HDOP != -1.0)
    {
        while(((millis() - previous) < timeout) && (_hdop > desired_HDOP))
        {
            answer = checkGPS();

            if (answer == 0)
            {
                if ((_fixMode == 2) || (_fixMode == 3))
                {
                    // Good options:
                    //  2 - 2D fix
                    //  3 - 3D fix
                    break;
                }
            }

            delay(1000);
        }

        // if down the threshold then return ok
        if (_hdop <= desired_HDOP)
        {
            return 0;
        }

        // Condition to avoid an overflow (DO NOT REMOVE)
        if( millis() < previous) previous = millis();
    }

    if (_hdop < desired_HDOP)
    {
        #if DEBUG_ARDUPI4G > 0
            printf("_hdop not valid\n");
        #endif
        return 1;
    }

    return 0;
}


/*
 *
 *
 *
 *
 */
uint8_t arduPi4G::gpsSetQualityOfService( uint32_t horiz_accuracy,
                                          uint16_t vertic_accuracy,
                                          uint16_t rsp_time,
                                          uint32_t age_of_location_info,
                                          uint8_t location_type,
                                          uint8_t nav_profile,
                                          uint8_t velocity_request)
{
    uint8_t answer;
    char command_buffer[80];

    // "AT$GPSQOS=%lu,%u,%u,%lu,%u,%u,%u\r"
    sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[19])),
                horiz_accuracy,
                vertic_accuracy,
                rsp_time,
                age_of_location_info,
                location_type,
                nav_profile,
                velocity_request);

    // send command
    answer = (int)sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }

        // error
        return 1;
    }

    return 0;
}


bool arduPi4G::getNMEAString(uint8_t NMEA)
{
    char command_answer[20];
    char command_buffer[30];
    char NMEA_string[100];
    uint16_t NMEA_length;
    uint8_t answer;

    switch(NMEA)
    {
        case LE910_GGA:
            // AT$GPSNMUN=3,1,0,0,0,0,0\r
            sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[17])), 1, 0, 0, 0, 0, 0);
            strcpy(command_answer, (char*)pgm_read_word(&(table_GPS[10])));   //$GPGGA
            break;
        case LE910_GSA:
            // AT$GPSNMUN=3,0,0,1,0,0,0\r
            sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[17])), 0, 0, 1, 0, 0, 0);
            strcpy(command_answer, (char*)pgm_read_word(&(table_GPS[9])));    //$GPGSA
            break;
        case LE910_GLL:
            // AT$GPSNMUN=3,0,1,0,0,0,0\r
            sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[17])), 0, 1, 0, 0, 0, 0);
            strcpy(command_answer, (char*)pgm_read_word(&(table_GPS[7])));    //$GPGLL
            break;
        case LE910_VTG:
            // AT$GPSNMUN=3,0,0,0,0,1,0\r
            sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[17])), 0, 0, 0, 0, 1, 0);
            strcpy(command_answer, (char*)pgm_read_word(&(table_GPS[11])));   //$GPRMC
            break;
        case LE910_RMC:
            // AT$GPSNMUN=3,0,0,0,0,0,1\r
            sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[17])), 0, 0, 0, 0, 0, 1);
            strcpy(command_answer, (char*)pgm_read_word(&(table_GPS[8])));    //$GPVTG
            break;
        case LE910_GSV:
            // AT$GPSNMUN=3,0,0,0,1,0,0\r
            sprintf(command_buffer, (char*)pgm_read_word(&(table_GPS[17])), 0, 0, 0, 1, 0, 0);
            strcpy(command_answer, (char*)pgm_read_word(&(table_GPS[12])));   //$GPGSV
            break;
    }

    //command_answer <-- "CONNECT\r\n"
    strcpy(command_answer, (char*)pgm_read_word(&(table_GPS[6])));
    answer = sendCommand(command_buffer, command_answer, 5000);
    if (answer != 1)
    {
        return 1;
    }

    // "+++"
    strcpy(command_buffer, (char*)pgm_read_word(&(table_GPS[18])));

    answer = waitFor((char*)"\r\n", 5000);
    if (answer != 1)
    {
        // Exit from data mode
        sendCommand(command_buffer, LE910_OK, 2000);
        return 1;
    }

    memset(NMEA_string, '\0', sizeof(NMEA_string));
    NMEA_length = (strchr((char*)_buffer, '*') - (char*)_buffer);
    memcpy(NMEA_string, _buffer, NMEA_length);

    // Exit from data mode
    sendCommand(command_buffer, LE910_OK, 2000);

    printf(NMEA_string);

    return 0;
}


/* Function: This function gets the temperature interval or the temperature value
 * Parameters:  mode:    0 for read the temperature interval
 *                       1 for read the temperature value
 * Return:  If interval mode selected
 *          -2 Extreme temperature lower bound.
 *          -1 Operating temperature lower bound.
 *          0 normal temperature.
 *          1 Operating temperature upper bound.
 *          2 Extreme temperature upper bound.
 *          -1000 if error
 *
 *          If temperature value selected return temperature in Celsius
 *          -1000 if error
*/
uint8_t arduPi4G::getTemp()
{
    int answer;
    char *ptr;
    char command_answer[20];
    char command_buffer[20];

    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[8]))); //AT#TEMPMON=1
    strcpy(command_answer, (char*)pgm_read_word(&(table_4G[9]))); //#TEMPMEAS:

    // send command
    answer = (int)sendCommand(command_buffer, command_answer, LE910_ERROR, 2000);
    if (answer != 1)
    {
        return 1;
    }

    answer = waitFor(LE910_OK, 2000);
    if (answer != 1)
    {
        return 1;
    }

    // Get temperature interval
    ptr = strtok((char*)_buffer, ",");

    if (ptr == NULL)
    {
        return 1;
    }

    _tempInterval = atoi(ptr);

    // Get temperature in Celsius degrees
    ptr = strtok(NULL, "\r");

    if (ptr == NULL)
    {
        return 1;
    }
    _temp = atoi(ptr);

    return 0;
}


/* Function:    This function gets the RSSI level
 * Return:  0 if OK; 1 if error
*/
uint8_t arduPi4G::getRSSI()
{
    uint8_t answer;
    uint8_t error;
    char command_buffer[30];
    char delimiters[30];

    // AT+CSQ\r
    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[10])));

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR, 2000);

    // check answer
    if (answer == 1)
    {
        // delimiters <-- "AT+CSQ\r\n: ,"
        strcpy(delimiters, (char*)pgm_read_word(&(table_4G[33])));

        error = parseInt(&_rssi, delimiters);

        if (error != 0)
        {
            return 1;
        }

        #if DEBUG_ARDUPI4G > 0
            printf("Raw RSSI: %d\n", _rssi);
        #endif

        // converts the RSSI from the command to dBm
        switch(_rssi)
        {
            case 0:
            case 99:
                _rssi = -113;
                break;
            case 1:
                _rssi = -111;
                break;
            case 33:
                _rssi = 0;
                break;
            default:
                _rssi = (_rssi * 2) - 113;
        }

        return 0;
    }

    return 1;
}


/* Function:    This function gets the packet service network type
 * Return:  '0' if OK; '1' if error
 *
 * Deprecated:
 *          0 for GPRS network
 *          1 for EGPRS network
 *          2 for WCDMA network
 *          3 for HSDPA network
 *          4 for LTE network
 *          5 for unknown or not registered
 *          -1 if error
*/
uint8_t arduPi4G::getNetworkType()
{
    uint8_t answer;
    uint8_t error;
    char command_buffer[50];
    char command_name[30];
    char delimiters[30];

    // AT#PSNT?\r
    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[11])));

    // send command
    answer =  sendCommand(command_buffer, LE910_OK, LE910_ERROR, 2000);

    if (answer == 1)
    {
        // delimiters <-- "AT#PSNT?\r: ,"
        strcpy(delimiters, (char*)pgm_read_word(&(table_4G[34])));

        // parse response
        error = parseInt(&_networkType, delimiters);

        if (error != 0)
        {
            return 1;
        }

        return 0;
    }

    return 1;
}


/* Function: This function gets the operator name
 * Return:  '0' if OK; 'x' if error
*/
uint8_t arduPi4G::getOperator(char* answer_storage)
{
    uint8_t answer;
    char command_buffer[50];
    char* pointer;

    // AT+COPS?\r
    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[24])));

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }

    // parse response: "+COPS: 0,0,"<answer>",7"\r\n
    pointer = strtok((char*)_buffer, "\"");

    if (pointer == NULL)
    {
        return 2;
    }

    strcpy(answer_storage, (char*) strtok(NULL, "\""));

    return 0;
}


/* Function:    This function gets info from the module
 * Parameters:  info_req:
 *                  arduPi4G::INFO_HW
 *                  arduPi4G::INFO_MANUFACTURER_ID
 *                  arduPi4G::INFO_MODEL_ID
 *                  arduPi4G::INFO_REV_ID
 *                  arduPi4G::INFO_IMEI
 *                  arduPi4G::INFO_IMSI
 *                  arduPi4G::INFO_ICCID
 * Return:  0 if OK; 1 if error
*/
int8_t arduPi4G::getInfo(uint8_t info_req)
{
    uint8_t answer;
    char command_buffer[50];

    switch (info_req)
    {
        case 0:
            // Hardware revision
            // "AT#HWREV\r"
            strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[12])));
            break;
        case 1:
            // Manufacturer identification
            // "AT#CGMI\r"
            strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[13])));
            break;
        case 2:
            // Model identification
            // "AT#CGMM\r"
            strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[14])));
            break;
        case 3:
            // Revision identification
            // "AT#CGMR\r"
            strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[15])));
            break;
        case 4:
            // IMEI
            // "AT#CGSN\r"
            strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[16])));
            break;
        case 5:
            // IMSI
            // "AT#CIMI\r"
            strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[17])));
            break;
        case 6:
            // ICCID
            // "AT#CCID\r"
            strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[18])));
            break;
    }

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR, 2000);

    // check answer
    if (answer == 1)
    {
        char aux[30];
        char* pointer;
        pointer = strchr((char*)_buffer, ':');

        // skip command name within response buffer
        if (pointer != NULL)
        {
            pointer++;
            _length = strlen(pointer);
            memmove((char*)_buffer, pointer, strlen(pointer));
        }

        // success response format: "\r\n<info>\r\n"
        answer = parseString(aux, sizeof(aux), (char *)" \r\n");

        if (answer == 0)
        {
            memset(_buffer, 0x00 ,sizeof(_buffer));
            _length = strlen(aux);
            strncpy((char*)_buffer, aux, strlen(aux));
            return 0;
        }
        return 1;
    }
    return 1;
}


/*
 * This function selects the cellular network (Wireless Data Service, WDS) to
 * operate with the TA (WDS-Side Stack Selection).
 *
 *      NETWORK_GSM
 *      NETWORK_UTRAN
 *      NETWORK_3GPP
 *      NETWORK_EUTRAN_ONLY
 *      NETWORK_GERAN_UTRAN
 *      NETWORK_GERAN_EUTRAN
 *      NETWORK_UTRAN_EUTRAN
 *
 * @return '0' if OK; '1' if error
 *
 */
uint8_t arduPi4G::setWirelessNetwork(uint8_t n)
{
    uint8_t answer;
    char command_buffer[20];

    // "AT+WS46=<n>\r"
    sprintf(command_buffer, (char*)pgm_read_word(&(table_4G[41])), n);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);

    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }

    return 0;
}


/*
 * This function sets the RTC time and date from the Network's response
 *
 * @return '0' if OK; '1' if error
 *
 */
uint8_t arduPi4G::setTimeFrom4G()
{

    int8_t answer, networkType;
    char format[60];
    char command_buffer[50];
    uint8_t year, month, day;
    uint8_t hour, minute, second, timezone;
    bool RTC_status;

    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    char output[128];

    // AT+CCLK?\r
    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[25])));

    // send command
    answer =  sendCommand(command_buffer, (char*)"\"", LE910_ERROR, 2000);

    if (answer == 1)
    {
        waitFor((char*)"\"", 2000);

        // format <-- "%2hhu%*c%2hhu%*c%2hhu%*c%2hhu%*c%2hhu%*c%2hhu%hhd\""
        strcpy(format, (char*)pgm_read_word(&(table_4G[35])));

        sscanf( (char*)_buffer,
                format,
                &year,
                &month,
                &day,
                &hour,
                &minute,
                &second,
                &timezone);

        #if DEBUG_ARDUPI4G > 0
            printf("Network time:\n year:%d | month:%d | day:%d\n hour:%d | minute:%d | second:%d | timezone:%d\n",
                    year, month, day, hour, minute, second, timezone);
        #endif

        return 0;
    }

    return 1;
}


/*
 * This function shows the RTC time and date from the Network's response
 *
 * @return '0' if OK; '1' if error
 *
 */
uint8_t arduPi4G::showTimeFrom4G()
{

    int8_t answer, networkType;
    char format[60];
    char command_buffer[50];
    uint8_t year, month, day;
    uint8_t hour, minute, second, timezone;

    // AT+CCLK?\r
    strcpy(command_buffer, (char*)pgm_read_word(&(table_4G[25])));

    // send command
    answer =  sendCommand(command_buffer, (char *)"\"", LE910_ERROR, 2000);

    if (answer == 1)
    {
        waitFor((char *)"\"", 2000);

        // format <-- "%2hhu%*c%2hhu%*c%2hhu%*c%2hhu%*c%2hhu%*c%2hhu%hhd\""
        strcpy(format, (char*)pgm_read_word(&(table_4G[35])));

        sscanf( (char*)_buffer,
                format,
                &year,
                &month,
                &day,
                &hour,
                &minute,
                &second,
                &timezone);

        // time from network
        printf("Network time:\n");
        printf("year: %d | month: %d | day: %d | hour: %d | minute: %d | second: %d | timezone: %d\n",
                year, month, day, hour, minute, second, timezone);

        return 0;
    }

    return 1;
}


/* This function sets the apn from operator
 * Parameters:  apn: operator APN
 * Return: nothing
*/
void arduPi4G::set_APN( char* apn)
{

    set_APN( apn, NULL, NULL);
}


/* This function sets the apn, login and password from operator
 * Parameters:  apn: operator APN
 *              login: login or user
 *              password: password
 * Return: nothing
*/
void arduPi4G::set_APN( char* apn, char* login, char* password)
{

    memset(_apn, '\0', sizeof(_apn));
    memset(_apn_login, '\0', sizeof(_apn_login));
    memset(_apn_password, '\0', sizeof(_apn_password));

    strncpy(_apn, apn, min(sizeof(_apn), strlen(apn)));
    strncpy(_apn_login, login, min(sizeof(_apn_login), strlen(login)));
    strncpy(_apn_password, password, min(sizeof(_apn_password), strlen(password)));

}


/* This function shows the apn, login and password constants
 *
 * Return: nothing
*/
void arduPi4G::show_APN()
{
    // APN parameters depends on SIM
    printf("*****************************\n");
    printf("APN: %s\n", _apn);
    printf("LOGIN: %s\n", _apn_login);
    printf("PASSWORD: %s\n", _apn_password);
    printf("*****************************\n");
}


/* Function: This function sets the SMTP server address, used for E-mail sending.
 * 
 * Parameter: SMTP server address, string type. This parameter can be either:
 *  - any valid IP address in the format: xxx.xxx.xxx.xxx
 *  - any host name to be solved with a DNS query in the format: <host name> 
 *  (factory default is the empty string “”)
 * 
 * Note: the max length for <smtp> is the output of Test command.
 * Return:  '0' if OK; 'x' if error
*/
uint8_t arduPi4G::emailSetServerSMTP(char* serverSMTP)
{
    uint8_t answer;
    char command_buffer[80];

    // AT#ESMTP="<SMTP server>"\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_EMAIL_LE910[0])),serverSMTP);

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);

    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }
    return 0;
}


/* Function: This function sets the email
 * Return:  '0' if OK; 'x' if error
*/
uint8_t arduPi4G::emailConfigureSMTP(uint8_t security, uint16_t port)
{
    uint8_t error;
    char command_buffer[50];
    char answer[20];
    
    // AT#SMTPCFG=<ssl_encryption>,<port>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_EMAIL_LE910[7])),security,port);
    
    // send command
    error = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);
    
    // check answer
    if (error != 1)
    {
        if (error == 2)
        {
            getErrorCode();
        }
        return 1;
    }
    return 0;
}


/* Function: This function sets the email
 * Return:  '0' if OK; 'x' if error
*/
uint8_t arduPi4G::emailSetSender(char* address, char* user, char* password)
{
    uint8_t answer;
    char command_buffer[100];
    
    //// 1. Set sender address
    // AT#EADDR="<address>"\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_EMAIL_LE910[1])),address);


    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);
    
    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }
      
    //// 2. Set sender user
    // AT#EUSER="<user>"\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_EMAIL_LE910[2])),user);
    
    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);
    
    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 2;
    }
        
    //// 3. Set sender password
    // AT#EPASSW="<passsword>"\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_EMAIL_LE910[3])),password);
    
    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);
    
    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 3;
    }   
    
    return 0;   
}


/* Function: This function activates context
 * Return:  '0' if OK; 'x' if error
 */
uint8_t arduPi4G::emailActivateContext(char* user, char* password)
{
    uint8_t error;
    char command_buffer[50];
    char answer[20];
    
    // AT#SGACT=1,1,"<APN user>","<APN pass>"\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_EMAIL_LE910[5])),user,password);

    // "#SGACT: "
    sprintf(answer, (char*)pgm_read_word(&(table_EMAIL_LE910[6])));
    
    // send command
    error = sendCommand(command_buffer, answer, LE910_ERROR_CODE, LE910_ERROR, 2000);
    
    // check answer
    if (error != 1)
    {
        if (error == 2)
        {
            getErrorCode();
        }
        return 1;
    }
    return 0;
}


/* Function: This function sends the email
 * Return:  '0' if OK; 'x' if error
*/

uint8_t arduPi4G::emailSend(char* address,char* subject,char* body)
{
    uint8_t error;
    char command_buffer[50];
    char answer[20];
    
    //// 1. Check data connection
    error = checkDataConnection(60);    
    if (error != 0)
    {
        return error;   // 1 to 15 error codes
    }
    
    //// 2. Send email
    // AT#EMAILD=<address>,<subject>\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_EMAIL_LE910[8])),address,subject);

    // send command
    error = sendCommand(command_buffer, (char *)">", LE910_ERROR_CODE, LE910_ERROR, 2000);
    
    // check answer
    if (error != 1)
    {
        if (error == 2)
        {
            getErrorCode();
        }
        return 1;
    }
        
    // send e-mail body to the module
    printString(body,UART0);
    printByte(0x1A,UART0);
    
    error = waitFor(LE910_OK,LE910_ERROR_CODE,LE910_ERROR,60000);
    
    // check answer
    if (error != 1)
    {
        if (error == 2)
        {
            getErrorCode();
        }
        return 1;
    }
    return 0;
}


/* Function: This function resets the actual e-mail parameters in the NVM of 
 * the module to the default ones. The values reset are:
 *      E-mail User Name
 *      E-mail Password
 *      E-mail Sender Address
 *      E-mail SMTP server
 * 
 * Return:  '0' if OK; 'x' if error
*/
uint8_t arduPi4G::emailReset()
{
    uint8_t answer;
    char command_buffer[15];
    
    // AT#ERST\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_EMAIL_LE910[9])));

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);
    
    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }
    return 0;
}


/* Function: This function saves the actual e-mail parameters in the NVM of the 
 * device. The values reset are:
 *      E-mail User Name
 *      E-mail Password
 *      E-mail Sender Address
 *      E-mail SMTP server
 * 
 * Return:  '0' if OK; 'x' if error
*/
uint8_t arduPi4G::emailSave()
{
    uint8_t answer;
    char command_buffer[15];
    
    // AT#ESAV\r
    sprintf(command_buffer, (char*)pgm_read_word(&(table_EMAIL_LE910[10])));

    // send command
    answer = sendCommand(command_buffer, LE910_OK, LE910_ERROR_CODE, LE910_ERROR, 2000);
    
    // check answer
    if (answer != 1)
    {
        if (answer == 2)
        {
            getErrorCode();
        }
        return 1;
    }
    return 0;
}


/* This function shows the error code number stored in the related attribute
 *
 * @return: nothing
 */
void arduPi4G::printErrorCode()
{
    printErrorCode(_errorCode);
}


#if DEBUG_ARDUPI4G > 0
/* This function shows the error code indicated as input
 *
 * @return: nothing
 */
void arduPi4G::printErrorCode( uint16_t error )
{
    printf("==> ERROR CODE: ");
    switch (error)
    {
        case ARDUPI4G_CME_ERROR_0000: printf("phone failure\n"); break;
        case ARDUPI4G_CME_ERROR_0001: printf("No connection to phone\n"); break;
        case ARDUPI4G_CME_ERROR_0002: printf("phone-adaptor link reserved\n"); break;
        case ARDUPI4G_CME_ERROR_0003: printf("operation not allowed\n"); break;
        case ARDUPI4G_CME_ERROR_0004: printf("operation not supported\n"); break;
        case ARDUPI4G_CME_ERROR_0005: printf("PH-SIM PIN required\n"); break;
        case ARDUPI4G_CME_ERROR_0010: printf("SIM not inserted\n"); break;
        case ARDUPI4G_CME_ERROR_0011: printf("SIM PIN required\n"); break;
        case ARDUPI4G_CME_ERROR_0012: printf("SIM PUK required\n"); break;
        case ARDUPI4G_CME_ERROR_0013: printf("SIM failure\n"); break;
        case ARDUPI4G_CME_ERROR_0014: printf("SIM busy\n"); break;
        case ARDUPI4G_CME_ERROR_0015: printf("SIM wrong\n"); break;
        case ARDUPI4G_CME_ERROR_0016: printf("incorrect password\n"); break;
        case ARDUPI4G_CME_ERROR_0017: printf("SIM PIN2 required\n"); break;
        case ARDUPI4G_CME_ERROR_0018: printf("SIM PUK2 required\n"); break;
        case ARDUPI4G_CME_ERROR_0020: printf("memory full\n"); break;
        case ARDUPI4G_CME_ERROR_0021: printf("invalid index\n"); break;
        case ARDUPI4G_CME_ERROR_0022: printf("not found\n"); break;
        case ARDUPI4G_CME_ERROR_0023: printf("memory failure\n"); break;
        case ARDUPI4G_CME_ERROR_0024: printf("text string too long\n"); break;
        case ARDUPI4G_CME_ERROR_0025: printf("invalid characters in text string\n"); break;
        case ARDUPI4G_CME_ERROR_0026: printf("dial string too long\n"); break;
        case ARDUPI4G_CME_ERROR_0027: printf("invalid characters in dial string\n"); break;
        case ARDUPI4G_CME_ERROR_0030: printf("no network service\n"); break;
        case ARDUPI4G_CME_ERROR_0031: printf("network time-out\n"); break;
        case ARDUPI4G_CME_ERROR_0032: printf("network not allowed - emergency calls only\n"); break;
        case ARDUPI4G_CME_ERROR_0040: printf("network personalization PIN required\n"); break;
        case ARDUPI4G_CME_ERROR_0041: printf("network personalization PUK required\n"); break;
        case ARDUPI4G_CME_ERROR_0042: printf("network subset personalization PIN required\n"); break;
        case ARDUPI4G_CME_ERROR_0043: printf("network subset personalization PUK required\n"); break;
        case ARDUPI4G_CME_ERROR_0044: printf("service provider personalization PIN required\n"); break;
        case ARDUPI4G_CME_ERROR_0045: printf("Service provider personalization PUK required\n"); break;
        case ARDUPI4G_CME_ERROR_0046: printf("corporate personalization PIN required\n"); break;
        case ARDUPI4G_CME_ERROR_0047: printf("corporate personalization PUK required\n"); break;
        case ARDUPI4G_CME_ERROR_0100: printf("unknown\n"); break;
        case ARDUPI4G_CME_ERROR_0770: printf("SIM invalid\n"); break;
        case ARDUPI4G_CME_ERROR_0103: printf("Illegal MS (#3)*\n"); break;
        case ARDUPI4G_CME_ERROR_0106: printf("Illegal ME (#6)*\n"); break;
        case ARDUPI4G_CME_ERROR_0107: printf("GPRS service not allowed (#7)*\n"); break;
        case ARDUPI4G_CME_ERROR_0111: printf("PLMN not allowed (#11)*\n"); break;
        case ARDUPI4G_CME_ERROR_0112: printf("Location area not allowed (#12)*\n"); break;
        case ARDUPI4G_CME_ERROR_0113: printf("Roaming not allowed in this location area (#13)*\n"); break;
        case ARDUPI4G_CME_ERROR_0132: printf("service option not supported (#32)*\n"); break;
        case ARDUPI4G_CME_ERROR_0133: printf("requested service option not subscribed (#33)*\n"); break;
        case ARDUPI4G_CME_ERROR_0134: printf("service option temporarily out of order (#34)*\n"); break;
        case ARDUPI4G_CME_ERROR_0148: printf("unspecified GPRS error\n"); break;
        case ARDUPI4G_CME_ERROR_0149: printf("PDP authentication failure\n"); break;
        case ARDUPI4G_CME_ERROR_0150: printf("invalid mobile class\n"); break;
        case ARDUPI4G_CME_ERROR_0550: printf("generic undocumented error\n"); break;
        case ARDUPI4G_CME_ERROR_0551: printf("wrong state\n"); break;
        case ARDUPI4G_CME_ERROR_0552: printf("wrong mode\n"); break;
        case ARDUPI4G_CME_ERROR_0553: printf("context already activated\n"); break;
        case ARDUPI4G_CME_ERROR_0554: printf("stack already active\n"); break;
        case ARDUPI4G_CME_ERROR_0555: printf("activation failed\n"); break;
        case ARDUPI4G_CME_ERROR_0556: printf("context not opened\n"); break;
        case ARDUPI4G_CME_ERROR_0557: printf("cannot setup socket\n"); break;
        case ARDUPI4G_CME_ERROR_0558: printf("cannot resolve DN\n"); break;
        case ARDUPI4G_CME_ERROR_0559: printf("time-out in opening socket\n"); break;
        case ARDUPI4G_CME_ERROR_0560: printf("cannot open socket\n"); break;
        case ARDUPI4G_CME_ERROR_0561: printf("remote disconnected or time-out\n"); break;
        case ARDUPI4G_CME_ERROR_0562: printf("connection failed\n"); break;
        case ARDUPI4G_CME_ERROR_0563: printf("tx error\n"); break;
        case ARDUPI4G_CME_ERROR_0564: printf("already listening\n"); break;
        case ARDUPI4G_CME_ERROR_0568: printf("wrong PDP\n"); break;
        case ARDUPI4G_CME_ERROR_0615: printf("FTP not connected\n"); break;
        case ARDUPI4G_CME_ERROR_0623: printf("FTP write data closed\n"); break;
        case ARDUPI4G_CME_ERROR_0643: printf("FTP communication timeout\n"); break;
        case ARDUPI4G_CME_ERROR_0657: printf("Network survey error (No Carrier)*\n"); break;
        case ARDUPI4G_CME_ERROR_0658: printf("Network survey error (Busy)*\n"); break;
        case ARDUPI4G_CME_ERROR_0659: printf("Network survey error (Wrong request)*\n"); break;
        case ARDUPI4G_CME_ERROR_0660: printf("Network survey error (Aborted)*\n"); break;
        case ARDUPI4G_CME_ERROR_0257: printf("network rejected request\n"); break;
        case ARDUPI4G_CME_ERROR_0258: printf("retry operation\n"); break;
        case ARDUPI4G_CME_ERROR_0259: printf("invalid deflected to number\n"); break;
        case ARDUPI4G_CME_ERROR_0260: printf("deflected to own number\n"); break;
        case ARDUPI4G_CME_ERROR_0261: printf("unknown subscriber\n"); break;
        case ARDUPI4G_CME_ERROR_0262: printf("service not available\n"); break;
        case ARDUPI4G_CME_ERROR_0263: printf("unknown class specified\n"); break;
        case ARDUPI4G_CME_ERROR_0264: printf("unknown network message\n"); break;
        case ARDUPI4G_CME_ERROR_0680: printf("LU processing\n"); break;
        case ARDUPI4G_CME_ERROR_0681: printf("Network search aborted\n"); break;
        case ARDUPI4G_CME_ERROR_0682: printf("PTM mode\n"); break;
        case ARDUPI4G_CME_ERROR_0683: printf("Active call state\n"); break;
        case ARDUPI4G_CME_ERROR_0684: printf("SSL already activated\n"); break;
        case ARDUPI4G_CMS_ERROR_0300: printf("ME failure\n"); break;
        case ARDUPI4G_CMS_ERROR_0301: printf("SMS service of ME reserved\n"); break;
        case ARDUPI4G_CMS_ERROR_0302: printf("operation not allowed\n"); break;
        case ARDUPI4G_CMS_ERROR_0303: printf("operation not supported\n"); break;
        case ARDUPI4G_CMS_ERROR_0304: printf("invalid PDU mode parameter\n"); break;
        case ARDUPI4G_CMS_ERROR_0305: printf("invalid text mode parameter\n"); break;
        case ARDUPI4G_CMS_ERROR_0310: printf("SIM not inserted\n"); break;
        case ARDUPI4G_CMS_ERROR_0311: printf("SIM PIN required\n"); break;
        case ARDUPI4G_CMS_ERROR_0312: printf("PH-SIM PIN required\n"); break;
        case ARDUPI4G_CMS_ERROR_0313: printf("SIM failure\n"); break;
        case ARDUPI4G_CMS_ERROR_0314: printf("SIM busy\n"); break;
        case ARDUPI4G_CMS_ERROR_0315: printf("SIM wrong\n"); break;
        case ARDUPI4G_CMS_ERROR_0316: printf("SIM PUK required\n"); break;
        case ARDUPI4G_CMS_ERROR_0317: printf("SIM PIN2 required\n"); break;
        case ARDUPI4G_CMS_ERROR_0318: printf("SIM PUK2 required\n"); break;
        case ARDUPI4G_CMS_ERROR_0320: printf("memory failure\n"); break;
        case ARDUPI4G_CMS_ERROR_0321: printf("invalid memory index\n"); break;
        case ARDUPI4G_CMS_ERROR_0322: printf("memory full\n"); break;
        case ARDUPI4G_CMS_ERROR_0330: printf("SMSC address unknown\n"); break;
        case ARDUPI4G_CMS_ERROR_0331: printf("no network service\n"); break;
        case ARDUPI4G_CMS_ERROR_0332: printf("network time-out\n"); break;
        case ARDUPI4G_CMS_ERROR_0340: printf("no +CNMA acknowledgement expected\n"); break;
        case ARDUPI4G_CMS_ERROR_0500: printf("unknown error\n"); break;
        case ARDUPI4G_CME_ERROR_1001: printf("SSL certs and keys wrong or not stored\n"); break;
        case ARDUPI4G_CME_ERROR_1003: printf("SSL already activated\n"); break;
        case ARDUPI4G_CME_ERROR_1008: printf("SSL not connected\n"); break;
        default: printf("UNKNOWN"); break;
    }
}
#else
void arduPi4G::printErrorCode( uint16_t error )
{
    printf("==> ERROR CODE: %d\n", error);
}
#endif


// Preinstantiate Objects /////////////////////////////////////////////////////

arduPi4G _4G = arduPi4G();

///////////////////////////////////////////////////////////////////////////////
