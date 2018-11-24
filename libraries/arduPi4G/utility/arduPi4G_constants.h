/*! \file   arduPi4G_contants.h
 *  \brief  Header file with constant definitions and flash strings
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
 *  Version:           1.1
 *  Design:            David Gascón
 *  Implementation:    Alejandro Gállego, Yuri Carmona, Luis Miguel Marti
 *  Port to Raspberry: Ruben Martin
 */

#ifndef arduPi4G_constants_h
#define arduPi4G_constants_h

/******************************************************************************
 * Includes
 *****************************************************************************/

#include "arduPi4G_error_codes.h"


/******************************************************************************
 * COMMANDS (FLASH Definitions)
 ******************************************************************************/


/// table_4G  //////////////////////////////////////////////////////////////////

const char LE910_string_00[]     = "AT+CREG?\r";                    //0
const char LE910_string_01[]     = "+CREG: 0,";                     //1
const char LE910_string_02[]     = "AT+CGREG?\r";                   //2
const char LE910_string_03[]     = "+CGREG: 0,";                    //3
const char LE910_string_04[]     = "+CGDCONT=1,";                   //4
const char LE910_string_05[]     = "AT#USERID=\"%s\"\r";            //5
const char LE910_string_06[]     = "AT#PASSW=\"%s\"\r";             //6
const char LE910_string_07[]     = "#GPRS";                         //7
const char LE910_string_08[]     = "AT#TEMPMON=1\r";                //8
const char LE910_string_09[]     = "#TEMPMEAS: ";                   //9
const char LE910_string_10[]     = "AT+CSQ\r";                      //10
const char LE910_string_11[]     = "AT#PSNT?\r";                    //11
const char LE910_string_12[]     = "AT#HWREV\r";                    //12
const char LE910_string_13[]     = "AT#CGMI\r";                     //13
const char LE910_string_14[]     = "AT#CGMM\r";                     //14
const char LE910_string_15[]     = "AT#CGMR\r";                     //15
const char LE910_string_16[]     = "AT#CGSN\r";                     //16
const char LE910_string_17[]     = "AT#CIMI\r";                     //17
const char LE910_string_18[]     = "AT#CCID\r";                     //18
const char LE910_string_19[]     = "AT+CMEE=1\r";                   //19
const char LE910_string_20[]     = "ATE0\r";                        //20
const char LE910_string_21[]     = "AT#NITZ=7\r";                   //21
const char LE910_string_22[]     = "+CMTI";                         //22
const char LE910_string_23[]     = "SRING: ";                       //23
const char LE910_string_24[]     = "AT+COPS?\r";                    //24
const char LE910_string_25[]     = "AT+CCLK?\r";                    //25
const char LE910_string_26[]     = "AT#GPRS?\r";                    //26
const char LE910_string_27[]     = "GPRS: 1";                       //27
const char LE910_string_28[]     = "GPRS: 0";                       //28
const char LE910_string_29[]     = "AT+CGDCONT=1,\"IP\",\"%s\"\r";  //29
const char LE910_string_30[]     = "AT%s\"%s\"\r";                  //30
const char LE910_string_31[]     = "AT#GPRS=1\r";                   //31
const char LE910_string_32[]     = "+IP: \"\r\n";                   //32
const char LE910_string_33[]     = "AT+CSQ\r\n: ,";                 //33
const char LE910_string_34[]     = "AT#PSNT?\r: ,";                 //34
const char LE910_string_35[]     = "%2hhu%*c%2hhu%*c%2hhu%*c%2hhu%*c%2hhu%*c%2hhu%hhd\"";//35
const char LE910_string_36[]     = "AT+CMGL=\"REC UNREAD\"\r";      //36
const char LE910_string_37[]     = "AT#SA=%u,1\r";                  //37
const char LE910_string_38[]     = "AT#SHDN\r";                     //38
const char LE910_string_39[]     = "AT#ENHRST=1,0\r";               //39
const char LE910_string_40[]     = "+IP: ";                         //40

const char LE910_string_41[]     = "AT+WS46=%u\r";                  //41
const char LE910_string_42[]     = "+CMGL: ";                       //42
const char LE910_string_43[]     = "+CMGR: ";                       //43
const char LE910_string_44[]     = "AT+CEREG?\r";                   //44
const char LE910_string_45[]     = "+CEREG: 0,";                    //45


const char* const table_4G[]  = 
{
    LE910_string_00,    //0
    LE910_string_01,    //1
    LE910_string_02,    //2
    LE910_string_03,    //3
    LE910_string_04,    //4
    LE910_string_05,    //5
    LE910_string_06,    //6 
    LE910_string_07,    //7
    LE910_string_08,    //8
    LE910_string_09,    //9
    LE910_string_10,    //10
    LE910_string_11,    //11
    LE910_string_12,    //12
    LE910_string_13,    //13
    LE910_string_14,    //14
    LE910_string_15,    //15
    LE910_string_16,    //16
    LE910_string_17,    //17
    LE910_string_18,    //18
    LE910_string_19,    //19
    LE910_string_20,    //20
    LE910_string_21,    //21
    LE910_string_22,    //22
    LE910_string_23,    //23
    LE910_string_24,    //24
    LE910_string_25,    //25
    LE910_string_26,    //26
    LE910_string_27,    //27
    LE910_string_28,    //28
    LE910_string_29,    //29
    LE910_string_30,    //30
    LE910_string_31,    //31
    LE910_string_32,    //32
    LE910_string_33,    //33
    LE910_string_34,    //34
    LE910_string_35,    //35
    LE910_string_36,    //36
    LE910_string_37,    //37
    LE910_string_38,    //38
    LE910_string_39,    //39
    LE910_string_40,    //40
    LE910_string_41,    //41
    LE910_string_42,    //42
    LE910_string_43,    //43
    LE910_string_44,    //44
    LE910_string_45,    //45
};


/// table_PIN  /////////////////////////////////////////////////////////////////

const char LE910_PIN_00[]    = "AT+CPIN?\r";                 //0
const char LE910_PIN_01[]    = "READY";                      //1
const char LE910_PIN_02[]    = "SIM PIN";                    //2
const char LE910_PIN_03[]    = "SIM PUK";                    //3
const char LE910_PIN_04[]    = "PH-SIM PIN";                 //4
const char LE910_PIN_05[]    = "PH-FSIM PIN";                //5
const char LE910_PIN_06[]    = "PH-FSIM PUK";                //6
const char LE910_PIN_07[]    = "SIM PIN2";                   //7
const char LE910_PIN_08[]    = "SIM PUK2";                   //8
const char LE910_PIN_09[]    = "PH-NET PIN";                 //9
const char LE910_PIN_10[]    = "PH-NET PUK";                //10
const char LE910_PIN_11[]    = "PH-NETSUB PIN";             //11
const char LE910_PIN_12[]    = "PH-NETSUB PUK";             //12
const char LE910_PIN_13[]    = "PH-SP PIN";                 //13
const char LE910_PIN_14[]    = "PH-SP PUK";                 //14
const char LE910_PIN_15[]    = "PH-CORP PIN";               //15
const char LE910_PIN_16[]    = "PH-CORP PUK";               //16
const char LE910_PIN_17[]    = "+CPIN:";                    //17
const char LE910_PIN_18[]    = "AT+CPIN=\"%s\"\r";          //18
const char LE910_PIN_19[]    = "AT+CPIN=\"%s\",\"%s\"\r";   //19

const char* const table_PIN[]  = 
{
    LE910_PIN_00,   //0
    LE910_PIN_01,   //1
    LE910_PIN_02,   //2
    LE910_PIN_03,   //3
    LE910_PIN_04,   //4
    LE910_PIN_05,   //5
    LE910_PIN_06,   //6
    LE910_PIN_07,   //7
    LE910_PIN_08,   //8
    LE910_PIN_09,   //9
    LE910_PIN_10,   //10
    LE910_PIN_11,   //11
    LE910_PIN_12,   //12
    LE910_PIN_13,   //13
    LE910_PIN_14,   //14
    LE910_PIN_15,   //15
    LE910_PIN_16,   //16
    LE910_PIN_17,   //17
    LE910_PIN_18,   //18
    LE910_PIN_19,   //19
};


/// table_SMS  /////////////////////////////////////////////////////////////////

const unsigned long LE910_SMS_TIMEOUT = 60000;      // Timeout for SMS functions in miliseconds

const char LE910_SMS_00[]    = "AT+CMGF=1\r";                       //0
const char LE910_SMS_01[]    = "AT+CPMS=\"SM\",\"SM\",\"SM\"\r";    //1
const char LE910_SMS_02[]    = "AT+CNMI=2,1,0,0,0\r";               //2
const char LE910_SMS_03[]    = "AT+CMGR=%u\r";                      //3
const char LE910_SMS_04[]    = "AT+CMGS=\"%s\"\r";                  //4
const char LE910_SMS_05[]    = "AT+CMGD=%u\r";                      //5
const char LE910_SMS_06[]    = "AT+CMGD=%u,%u\r";                   //6
const char LE910_SMS_07[]    = "+CMGR:";                            //7

const char* const table_SMS[]  = 
{
    LE910_SMS_00,   //0
    LE910_SMS_01,   //1
    LE910_SMS_02,   //2
    LE910_SMS_03,   //3
    LE910_SMS_04,   //4
    LE910_SMS_05,   //5
    LE910_SMS_06,   //6
    LE910_SMS_07,   //7
};


/// table_HTTP  ////////////////////////////////////////////////////////////////

const unsigned long LE910_HTTP_TIMEOUT = 60000;     // Timeout for HTTP and HTTPS functions in miliseconds
const unsigned long LE910_HTTP_CONF_TIMEOUT = 15000;    // Timeout for HTTP and HTTPS functions in miliseconds

const char LE910_HTTP_00[]   = "AT#HTTPCFG=0,\"%s\",%u\r";      //0
const char LE910_HTTP_01[]   = "AT#HTTPQRY=0,%u,\"%s\"\r";      //1
const char LE910_HTTP_02[]   = "AT#HTTPSND=0,%u,\"%s\",%u\r";   //2
const char LE910_HTTP_03[]   = "#HTTPRING: 0,";                 //3
const char LE910_HTTP_04[]   = "AT#HTTPRCV=%u,%u\r";            //4
const char LE910_HTTP_05[]   = "/getpost_frame_parser.php";     //5
const char LE910_HTTP_06[]   = "frame=";                        //6

const char* const table_HTTP[]  = 
{
    LE910_HTTP_00,  //0
    LE910_HTTP_01,  //1
    LE910_HTTP_02,  //2
    LE910_HTTP_03,  //3
    LE910_HTTP_04,  //4
    LE910_HTTP_05,  //5
    LE910_HTTP_06,  //6
};


/// table_FTP  /////////////////////////////////////////////////////////////////

const uint32_t LE910_FTP_TIMEOUT = 60000;       // Timeout for FTP and FTPS functions in miliseconds
const uint32_t LE910_FTP_CONF_TIMEOUT = 15000;  // Timeout for FTP and FTPS functions in miliseconds

const char LE910_FTP_00[]    = "AT#FTPOPEN=\"%s:%u\",\"%s\",\"%s\",1\r";    // 0
const char LE910_FTP_01[]    = "AT#FTPCLOSE\r";                 // 1
const char LE910_FTP_02[]    = "AT#FTPPUT=\"%s\",0\r";          // 2
const char LE910_FTP_03[]    = "AT#FTPGETPKT=\"%s\"\r";         // 3
const char LE910_FTP_04[]    = "AT#FTPTYPE=0\r";                // 4
const char LE910_FTP_05[]    = "AT#FTPFSIZE=\"%s\"\r";          // 5 
const char LE910_FTP_06[]    = "AT#FTPRECV=%u\r";               // 6
const char LE910_FTP_07[]    = "#FTPMSG";                       // 7
const char LE910_FTP_08[]    = "AT#FTPGETPKT?\r";               // 8
const char LE910_FTP_09[]    = "AT#FTPAPPEXT=%u\r";             // 9
const char LE910_FTP_10[]    = "#FTPGETPKT: %s,0,";             // 10
const char LE910_FTP_11[]    = "+++";                           // 11
const char LE910_FTP_12[]    = "NO CARRIER";                    // 12
const char LE910_FTP_13[]    = "#FTPRECV: ";                    // 13

const char* const table_FTP[]  = 
{
    LE910_FTP_00,   //0
    LE910_FTP_01,   //1
    LE910_FTP_02,   //2
    LE910_FTP_03,   //3
    LE910_FTP_04,   //4
    LE910_FTP_05,   //5
    LE910_FTP_06,   //6
    LE910_FTP_07,   //7
    LE910_FTP_08,   //8
    LE910_FTP_09,   //9
    LE910_FTP_10,   //10
    LE910_FTP_11,   //11
    LE910_FTP_12,   //12
    LE910_FTP_13,   //13
};


/// table_IP  //////////////////////////////////////////////////////////////////

const uint32_t LE910_IP_TIMEOUT = 10000;        // Timeout for IP functions in miliseconds
const uint32_t LE910_IP_CONF_TIMEOUT = 15000;   // Timeout for IP functions in miliseconds

const char LE910_IP_SOCKET_00[]  = "AT#SS=%u\r";                        //0
const char LE910_IP_SOCKET_01[]  = "AT#SI=%u\r";                        //1
const char LE910_IP_SOCKET_02[]  = "AT#SCFG=%u,%u,%u,%u,%u,%u\r";       //2
const char LE910_IP_SOCKET_03[]  = "AT#SCFGEXT=%u,%u,%u,%u\r";          //3
const char LE910_IP_SOCKET_04[]  = "#SCFGEXT2=";                        //4
const char LE910_IP_SOCKET_05[]  = "AT#SCFGEXT3=%u,1\r";                //5
const char LE910_IP_SOCKET_06[]  = "AT#SD=%u,%d,%u,\"%s\",0,%u,1\r";    //6
const char LE910_IP_SOCKET_07[]  = "AT#SH=%u\r";                        //7
const char LE910_IP_SOCKET_08[]  = "AT#SSENDEXT=%u,%u\r";               //8
const char LE910_IP_SOCKET_09[]  = "#SRECV: %u,";                       //9
const char LE910_IP_SOCKET_10[]  = "AT#SSLSECDATA=%u,%u,%u,%u\r";       //10
const char LE910_IP_SOCKET_11[]  = "AT#SSLSECDATA=%u,%u,%u\r";          //11
const char LE910_IP_SOCKET_12[]  = "AT#SSLD=1,%u,\"%s\",0,1\r";         //12
const char LE910_IP_SOCKET_13[]  = "AT#SSLS=%u\r";                      //13
const char LE910_IP_SOCKET_14[]  = "AT#SSLSEND=%u\r";                   //14
const char LE910_IP_SOCKET_15[]  = "AT#SSLRECV=%u,%u\r";                //15
const char LE910_IP_SOCKET_16[]  = "#SSLRECV: ";                        //16
const char LE910_IP_SOCKET_17[]  = "TIMEOUT\r\n";                       //17
const char LE910_IP_SOCKET_18[]  = "DISCONNECTED\r\n";                  //18
const char LE910_IP_SOCKET_19[]  = "AT#SSLH=%u\r";                      //19
const char LE910_IP_SOCKET_20[]  = "#SS: ";                             //20
const char LE910_IP_SOCKET_21[]  = "#SS: ,\r\n";                        //21
const char LE910_IP_SOCKET_22[]  = "#SSLS: ";                           //22
const char LE910_IP_SOCKET_23[]  = "#SSLS: ,\r\n";                      //23
const char LE910_IP_SOCKET_24[]  = "#SI: ";                             //24
const char LE910_IP_SOCKET_25[]  = "#SI: ,\r\n";                        //25
const char LE910_IP_SOCKET_26[]  = "AT#SRECV=%u,%u\r";                  //26
const char LE910_IP_SOCKET_27[]  = "#SSLSECDATA: ";                     //27
const char LE910_IP_SOCKET_28[]  = "AT#SCFGEXT=%u,%u,%u,%u,%u\r";       //28
const char LE910_IP_SOCKET_29[]  = "AT#SL=%u,%u,%u,%u\r";               //29
const char LE910_IP_SOCKET_30[]  = "AT#SLUDP=%u,%u,%u\r";               //30
const char LE910_IP_SOCKET_31[]  = "AT#SS\r";                           //31
const char LE910_IP_SOCKET_32[]  = "#SS: %u,";                          //32
const char LE910_IP_SOCKET_33[]  = "AT#SSLEN=1,%u\r";                   //33
const char LE910_IP_SOCKET_34[]  = "AT#SSLSECCFG=1,%u,%u\r";            //34

const char* const table_IP[]  = 
{
    LE910_IP_SOCKET_00,     //0
    LE910_IP_SOCKET_01,     //1
    LE910_IP_SOCKET_02,     //2
    LE910_IP_SOCKET_03,     //3
    LE910_IP_SOCKET_04,     //4
    LE910_IP_SOCKET_05,     //5
    LE910_IP_SOCKET_06,     //6
    LE910_IP_SOCKET_07,     //7
    LE910_IP_SOCKET_08,     //8
    LE910_IP_SOCKET_09,     //9
    LE910_IP_SOCKET_10,     //10
    LE910_IP_SOCKET_11,     //11
    LE910_IP_SOCKET_12,     //12
    LE910_IP_SOCKET_13,     //13    
    LE910_IP_SOCKET_14,     //14
    LE910_IP_SOCKET_15,     //15
    LE910_IP_SOCKET_16,     //16
    LE910_IP_SOCKET_17,     //17
    LE910_IP_SOCKET_18,     //18
    LE910_IP_SOCKET_19,     //19
    LE910_IP_SOCKET_20,     //20
    LE910_IP_SOCKET_21,     //21
    LE910_IP_SOCKET_22,     //22
    LE910_IP_SOCKET_23,     //23
    LE910_IP_SOCKET_24,     //24
    LE910_IP_SOCKET_25,     //25
    LE910_IP_SOCKET_26,     //26
    LE910_IP_SOCKET_27,     //27
    LE910_IP_SOCKET_28,     //28
    LE910_IP_SOCKET_29,     //29
    LE910_IP_SOCKET_30,     //30
    LE910_IP_SOCKET_31,     //31
    LE910_IP_SOCKET_32,     //32
    LE910_IP_SOCKET_33,     //33
    LE910_IP_SOCKET_34,     //34
};


/// table_GPS  /////////////////////////////////////////////////////////////////

const char LE910_GPS_00[]    = "AT$GPSP=%u\r";                      //0
const char LE910_GPS_01[]    = "AT$GPSSLSR=%u,%u,,,,,%u\r";         //1
const char LE910_GPS_02[]    = "AT$GPSR=%u\r";                      //2
const char LE910_GPS_03[]    = "$GPSGLO";                           //3
const char LE910_GPS_04[]    = "AT$GPSACP\r";                       //4
const char LE910_GPS_05[]    = "$GPSNMUN=3,";                       //5
const char LE910_GPS_06[]    = "CONNECT\r\n";                       //6
const char LE910_GPS_07[]    = "$GPGLL";                            //7
const char LE910_GPS_08[]    = "$GPVTG";                            //8
const char LE910_GPS_09[]    = "$GPGSA";                            //9
const char LE910_GPS_10[]    = "$GPGGA";                            //10
const char LE910_GPS_11[]    = "$GPRMC";                            //11
const char LE910_GPS_12[]    = "$GPGSV";                            //12
const char LE910_GPS_13[]    = "AT$GPSP?\r";                        //13
const char LE910_GPS_14[]    = "GPSP: 1";                           //14
const char LE910_GPS_15[]    = "$GPSACP: ,";                        //15
const char LE910_GPS_16[]    = "\r\n$GPSACP: .,";                   //16
const char LE910_GPS_17[]    = "AT$GPSNMUN=3,%u,%u,%u,%u,%u,%u\r";  //17
const char LE910_GPS_18[]    = "+++";                               //18
const char LE910_GPS_19[]    = "AT$GPSQOS=%lu,%u,%u,%lu,%u,%u,%u\r";//19
const char LE910_GPS_20[]    = "AT$SUPLV=%u\r";                     //20

const char* const table_GPS[]  = 
{
    LE910_GPS_00,   //0
    LE910_GPS_01,   //1
    LE910_GPS_02,   //2
    LE910_GPS_03,   //3
    LE910_GPS_04,   //4
    LE910_GPS_05,   //5
    LE910_GPS_06,   //6
    LE910_GPS_07,   //7
    LE910_GPS_08,   //8
    LE910_GPS_09,   //9
    LE910_GPS_10,   //10
    LE910_GPS_11,   //11
    LE910_GPS_12,   //12
    LE910_GPS_13,   //13
    LE910_GPS_14,   //14
    LE910_GPS_15,   //15
    LE910_GPS_16,   //16
    LE910_GPS_17,   //17
    LE910_GPS_18,   //18
    LE910_GPS_19,   //19
    LE910_GPS_20,   //20
};


/// table_EMAIL_LE910 ////////////////////////////////////////////////////////////

const char LE910_EMAIL_00[] = "AT#ESMTP=\"%s\"\r";              //0
const char LE910_EMAIL_01[] = "AT#EADDR=\"%s\"\r";              //1
const char LE910_EMAIL_02[] = "AT#EUSER=\"%s\"\r";              //2
const char LE910_EMAIL_03[] = "AT#EPASSW=\"%s\"\r";             //3
const char LE910_EMAIL_04[] = "AT#EMAILD=\"%s\"\r";             //4
const char LE910_EMAIL_05[] = "AT#SGACT=1,1,\"%s\",\"%s\"\r";   //5
const char LE910_EMAIL_06[] = "#SGACT: ";                       //6
const char LE910_EMAIL_07[] = "AT#SMTPCFG=%u,%i\r";             //7
const char LE910_EMAIL_08[] = "AT#EMAILD=\"%s\",\"%s\"\r";      //8
const char LE910_EMAIL_09[] = "AT#ERST\r";                      //9
const char LE910_EMAIL_10[] = "AT#ESAV\r";                      //10

const char* const table_EMAIL_LE910[] = 
{
    LE910_EMAIL_00, //0
    LE910_EMAIL_01, //1
    LE910_EMAIL_02, //2
    LE910_EMAIL_03, //3
    LE910_EMAIL_04, //4
    LE910_EMAIL_05, //5
    LE910_EMAIL_06, //6
    LE910_EMAIL_07, //7
    LE910_EMAIL_08, //8
    LE910_EMAIL_09, //9
    LE910_EMAIL_10  //10
};

#endif
