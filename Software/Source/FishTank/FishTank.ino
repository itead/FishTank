/**
 * @file FishTank.ino
 *
 * @par To control fishtank's device and RTC display and temperature display and set time.
 *
 * This program is used to control intelligent fish tank. Its structure 
 * is very simple. It is consisted of six sections, a main display section, and
 * 5 functions sections, including time setting(Time), pump (Pump), feed (Feed), 
 * light (Light) and ultraviolet (Uv). Because this project needs to realize these
 * functions and set a timer, a lot of nextion components has been used inside.
 * But do not worry, these components are set separately according to the function. 
 * I believe as long as you understand one function implementation way, you will naturally 
 * understand the others.
 *
 * @author  huang xianming (email:<xianming.huang@itead.cc>)
 * @date    2015/7/23
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 * 
 */

#include "NexButton.h"
#include "NexText.h"
#include <string.h>
#include "Arduino.h"
#include "Wire.h"
#include "RTCLib.h"
#include "NexPage.h"
#include "NexPicture.h"
#include "NexGauge.h"
#include <OneWire.h>

#define PIN_PUMP  42
#define PIN_FEED  43
#define PIN_LIGHT 44
#define PIN_UV    45


OneWire  ds(A0); 


#define at_serial Serial1

/*
 *******************************************************************
 * AT Instruction Set
 * 
 *******************************************************************
 */

#define AT_AT           "AT"
#define AT_TIME         "AT+TIME"
#define AT_PUMP         "AT+PUMP"
#define AT_FEED         "AT+FEED"
#define AT_LIGHT        "AT+LIGHT"
#define AT_UV           "AT+UV"
#define AT_OK           "OK"
#define AT_FAILED       "FAILED"
#define AT_UNKNOWN_CMD  "Unknown command"
#define AT_UNSUPPORTED  "Not supported"
#define AT_ERR_FORMAT   "Bad format"
#define AT_ERR_PARAM   "Bad param"

/*
 *******************************************************************
 * Nextion component for page:home 
 * 
 *******************************************************************
 */
NexText hour = NexText(0, 1, "hour");
NexText minute = NexText(0, 13, "minute");
NexText second = NexText(0, 14, "sec");
NexText month = NexText(0, 3, "mon");
NexText year = NexText(0, 4, "year2");
NexText week = NexText(0, 5, "week");
NexText day = NexText(0, 6, "day");
NexText temperature = NexText(0, 7, "tem");
NexPicture home_pumppic = NexPicture(0, 8, "p0");
NexPicture home_feedpic = NexPicture(0, 10, "p1");
NexPicture home_lightpic = NexPicture(0, 11, "p2");
NexPicture home_uvpic = NexPicture(0, 12, "p3");
NexGauge pointer  = NexGauge(0, 16, "z0");
NexButton goset = NexButton(0, 2, "goset");
/*
 *******************************************************************
 * Nextion component for page:set
 * 
 *******************************************************************
 */
NexButton backhome = NexButton(1, 1, "backhome");
NexButton settime = NexButton(1, 2, "settime");
NexButton setpump = NexButton(1, 3, "setpump");
NexButton setfeed = NexButton(1, 4, "setfeed");
NexButton setlight = NexButton(1, 5, "setlight");
NexButton setuv = NexButton(1, 6, "setuv");
/*
 *******************************************************************
 * Nextion component for page:time
 * 
 *******************************************************************
 */
NexText sethour = NexText(2, 2, "sethour1");
NexText setmin = NexText(2, 3, "setmin1");
NexText setweek = NexText(2, 16, "setweek1");
NexText setday = NexText(2, 8, "setday1");
NexText setmon = NexText(2, 12, "setmon1");
NexText setyear = NexText(2, 13, "setyear2");
NexButton settime_add = NexButton(2, 5, "up0");
NexButton settime_dec = NexButton(2, 6, "down0");
NexButton settime_ok = NexButton(2, 7, "settimeok");
NexButton time_back_to_set = NexButton(2, 1, "backset1");
/*
 *******************************************************************
 * Nextion component for page:pump
 * 
 *******************************************************************
 */
NexPicture setpump_manual = NexPicture(3, 9, "on1");
NexPicture setpump_timer = NexPicture(3, 10, "timer1");
NexButton setpump_add = NexButton(3, 5, "up1");
NexButton setpump_dec = NexButton(3, 6, "down1");
NexText setpump_hour = NexText(3, 7, "hour1");
NexText setpump_min = NexText(3, 8, "min1");
NexButton pump_back_to_set = NexButton(3, 1, "backset2");
/*
 *******************************************************************
 * Nextion component for page:feed
 * 
 *******************************************************************
 */
NexPicture setfeed_manual = NexPicture(4, 9, "on2");
NexPicture setfeed_timer = NexPicture(4, 10, "timer2");
NexButton setfeed_add = NexButton(4, 5, "up2");
NexButton setfeed_dec = NexButton(4, 6, "down2");
NexText setfeed_hour = NexText(4, 7, "hour2");
NexText setfeed_min = NexText(4, 8, "min2");
NexButton feed_back_to_set = NexButton(4, 1, "backset3");
/*
 *******************************************************************
 * Nextion component for page:light
 * 
 *******************************************************************
 */
NexPicture setlight_manual = NexPicture(5, 11, "on3");
NexPicture setlight_timer = NexPicture(5, 12, "timer3");
NexButton setlight_add = NexButton(5, 5, "up3");
NexButton setlight_dec = NexButton(5, 6, "down3");
NexText setlight_hour_on = NexText(5, 7, "hour31");
NexText setlight_hour_off = NexText(5, 8, "hour32");
NexText setlight_min_on = NexText(5, 9, "min31");
NexText setlight_min_off = NexText(5, 10, "min32");
NexButton light_back_to_set = NexButton(5, 1, "backset4");
/*
 *******************************************************************
 * Nextion component for page:uv
 * 
 *******************************************************************
 */
NexPicture setuv_manual = NexPicture(6, 9, "on4");
NexPicture setuv_timer = NexPicture(6, 10, "timer4");
NexButton setuv_add = NexButton(6, 5, "up4");
NexButton setuv_dec = NexButton(6, 6, "down4");
NexText setuv_hour = NexText(6, 7, "hour4");
NexText setuv_min = NexText(6, 8, "min4");
NexButton uv_back_to_set = NexButton(6, 1, "backset5");
/*
 *******************************************************************
 * Nextion component all page
 * 
 *******************************************************************
 */
NexPage home = NexPage(0, 0, "home");
NexPage set = NexPage(1, 0, "set");
NexPage time = NexPage(2, 0, "time");
NexPage pump = NexPage(3, 0, "pump");
NexPage feed = NexPage(4, 0, "feed");
NexPage light = NexPage(5, 0, "light");
NexPage uv = NexPage(6, 0, "uv");

NexTouch *nex_Listen_List[] = 
{
    &settime, &setpump, &setfeed, &setlight, &setuv, &backhome, 
    &goset,

    &sethour, &setmin, &setweek, &setday, &setmon, &setyear, 
    &settime_add, &settime_dec, &settime_ok, &time_back_to_set,
    
    &setpump_manual, &setpump_timer, &setpump_add, &setpump_dec,
    &setpump_hour, &setpump_min, &pump_back_to_set,

    &setfeed_manual, &setfeed_timer, &setfeed_add, &setfeed_dec,
    &setfeed_hour, &setfeed_min, &feed_back_to_set,

    &setlight_manual, &setlight_timer, &setlight_add, &setlight_dec,
    &setlight_hour_on, &setlight_min_off, &setlight_hour_off, &setlight_min_on,
    &light_back_to_set,
    
    &setuv_manual, &setuv_timer, &setuv_add, &setuv_dec,
    &setuv_hour, &setuv_min, &uv_back_to_set,

    NULL
};

RTCLib rtc;

/*
 *******************************************************************
 * Temperature partial variable
 * 
 *******************************************************************
 */
uint32_t number = 0;
uint16_t sensor_pin = A0;    /* select the input pin for the Temperature */
uint16_t sensor_value = 0;   /* variable to store the value coming from the Temperature*/
unsigned long update = 1;
int32_t timer = -1;          /* timer for second */

/*
 *******************************************************************
 * Buffer zone of various variables
 * 
 *******************************************************************
 */
char buffer[10] = {0};
char buffer_temp[10] = {0};
char buffer_year[10] = {0};
char buffer_month[10] = {0};
char buffer_day[10] = {0};
char buffer_hour[10] = {0};
char buffer_minute[10] = {0};
char buffer_week[10] = {0};
uint8_t buffer_second =0;
char buffer_temperature[10] = {0};

/*
 *******************************************************************
 * TIME partial variable
 * 
 *******************************************************************
 */
int8_t settime_type = -1;
int8_t settime_up = -1;
int8_t number_settime_year = -1;
int8_t number_settime_month = -1;
int8_t number_settime_day = -1;
int8_t number_settime_hour = -1;
int8_t number_settime_minute = -1;
int32_t number_settime_week = -1;

/*
 *******************************************************************
 * PUMP partial variable
 * 
 *******************************************************************
 */
int8_t setpump_type = -1;
int8_t setpump_up = -1;
uint8_t number_setpump_hour = 4;
uint8_t number_setpump_min = 30;
uint32_t pump_timer = 15;
uint32_t pump_manual = 16;
uint32_t pump_respond = 0;
uint32_t pump_stop = 0;

/*
 *******************************************************************
 * FEED partial variable
 * 
 *******************************************************************
 */
int8_t setfeed_type = -1;
int8_t setfeed_up = -1;
uint8_t number_setfeed_hour = 4;
uint8_t number_setfeed_sec = 30;
uint32_t feed_timer = 15;
uint32_t feed_manual = 16;
uint32_t feed_respond = 0;
uint32_t feed_stop = 0;

/*
 *******************************************************************
 * UV partial variable
 * 
 *******************************************************************
 */
int8_t setuv_type = -1;
int8_t setuv_up = -1;
uint8_t number_setuv_hour = 4;
uint8_t number_setuv_min = 30;
uint32_t uv_timer = 15;
uint32_t uv_manual = 16;
uint32_t uv_respond = 0;
uint32_t uv_stop = 0;

/*
 *******************************************************************
 * LIGHT partial variable
 * 
 *******************************************************************
 */
int8_t setlight_type = -1;
int8_t setlight_up = -1;
uint8_t number_setlight_hour_on = 18;
uint8_t number_setlight_min_on = 10;
uint8_t number_setlight_hour_off = 22;
uint8_t number_setlight_min_off = 10;
uint32_t light_timer = 40;
uint32_t light_manual = 38;

/*
 *******************************************************************
 * AT partial start
 * 
 *******************************************************************
 */
void atInit(void)
{
    at_serial.begin(9600);
    at_serial.println("Device Ready");
}

void atRespond(void)
{
    char temp[40];
    int16_t num;
    int16_t index_at = -1;
    int16_t index_crlf = -1;
    int16_t index_question_mark = -1;
    int16_t index_equal_mark = -1;
    String cmd;
    String param;
    while (at_serial.available() > 0)
    {
        String str = "";
        at_serial.setTimeout(500);
        num = at_serial.readBytesUntil('\n', temp, sizeof(temp) - 2);
        temp[num] = '\n';
        temp[num + 1] = '\0';
        str += temp;
        index_at = str.indexOf(AT_AT);
        index_equal_mark = str.indexOf("=");
        index_question_mark = str.indexOf("?");
        index_crlf = str.indexOf("\r\n");
        
        if (index_crlf > index_at 
            && index_at >= 0
            && index_equal_mark < 0
            && index_question_mark < 0
            )
        {
            /* Execute */
            cmd = str.substring(index_at, index_crlf);
            if (cmd == AT_AT)
            {
                at_serial.println(AT_OK);
            }
            else 
            {
                at_serial.println(AT_UNKNOWN_CMD);
            }
        }
        else if (index_crlf > index_equal_mark 
            && (index_equal_mark > index_at) 
            && index_at >= 0
            && index_question_mark < 0
            )
        {
            /* Set */
            cmd = str.substring(index_at, index_equal_mark);
            param = str.substring(index_equal_mark + 1, index_crlf);
            if (cmd == AT_TIME)
            {
                if (param.length() == 14)
                {
                    timeATRespond(param);
                }
                else 
                {
                    at_serial.println(AT_ERR_PARAM);
                }
            }
            else if (cmd == AT_PUMP)
            {
                if (atSet(param, &pump_manual, &pump_timer, &number_setpump_hour, &number_setpump_min))
                {
                    pumpATRespond();
                }
             
            }
            else if (cmd == AT_FEED)
            {
                if (atSet(param, &feed_manual, &feed_timer, &number_setfeed_hour, &number_setfeed_sec))
                {
                    feedATRespond();
                }
            }
            else if (cmd == AT_UV)
            {
                if(atSet(param, &uv_manual, &uv_timer, &number_setuv_hour, &number_setuv_min))
                {
                    uvATRespond();
                }
            }
            else if (cmd == AT_LIGHT)
            {

                lightATRespond(param);
            }
            else
            {
                at_serial.println(AT_UNKNOWN_CMD);
            }
            
        }
        else if (index_crlf > index_question_mark
            && index_question_mark > index_at
            && index_at >= 0
            && index_equal_mark < 0
            )
        {
            /* Query */
            cmd = str.substring(index_at, index_question_mark);
            if(cmd == AT_PUMP)
            {
                atQuery(pump_manual, pump_timer, number_setpump_hour, number_setpump_min, NULL, NULL);
            }
            else if (cmd == AT_FEED)
            {
                atQuery(feed_manual, feed_timer, number_setfeed_hour, number_setfeed_sec, NULL, NULL);
            }
            else if (cmd == AT_UV)
            {
                atQuery(uv_manual, uv_timer, number_setuv_hour, number_setuv_min, NULL, NULL);    
            }
            else if (cmd == AT_LIGHT)
            {
                atQuery(light_manual, light_timer, number_setlight_hour_on, number_setlight_min_on, 
                        number_setlight_hour_off, number_setlight_min_off);    
            }
            else
            {
                at_serial.println(AT_UNKNOWN_CMD);
            }
        }       
        else if (index_crlf > index_question_mark
            && index_question_mark > index_equal_mark
            && index_equal_mark > index_at
            && index_at >= 0
            )
        {
            /* Test */
           at_serial.println(AT_UNSUPPORTED); 
            
        }
        else
        { 
            at_serial.println(AT_ERR_FORMAT);
        } 
        if (index_at >= 0 && index_crlf >= 0)
        {
            refreshHome();
        }
 
    }
       
}

void atQuery(uint8_t query_manaul, uint8_t query_timer, uint8_t query_hour, uint8_t query_min,
                uint8_t query_hour2, uint8_t query_min2)
{
    String query;
    if (query_manaul == 16 || query_manaul == 38)
    {
        query += "OFF";
    }
    else if (query_manaul == 14 || query_manaul == 39)
    {
        query += "ON";
    }
    query += ",";
    if (query_timer == 15 || query_timer == 40)
    {
        query += "OFF";
    }
    else if (query_timer == 17 || query_timer == 41)
    {
        query += "ON";
    }
    query += ",";
    if (query_hour <10)
    {
        query += "0";
    }
    itoa(query_hour,buffer,10);
    query += buffer;
    query += ",";
    if (query_min <10)
    {
        query += "0";
    }
    itoa(query_min, buffer, 10);
    query += buffer;
    if (query_manaul == 16 || query_manaul == 14)
    {
        at_serial.println(query);    
    }
    else
    {
        query += ",";
        if (query_hour2 <10)
        {
            query += "0";
        }
        itoa(query_hour2, buffer, 10);
        query += buffer;
        query += ",";
        if (query_min2 <10)
        {
            query += "0";
        }
        itoa(query_min2, buffer, 10);
        query += buffer;
        at_serial.println(query);    
    }
    at_serial.println(AT_OK);    
}

bool atSet(String param, uint32_t *set_manual, uint32_t *set_timer, uint8_t *set_hour, uint8_t *set_min)
{
    String newparam;
    String manual;
    String timer;
    String next_time;
    String respond_timer;
    uint8_t comma;
    uint8_t temp_hour;
    uint8_t temp_min;
    comma = param.indexOf(",");
    manual = param.substring(0 , comma);
    param = param.substring(comma + 1);
    comma = param.indexOf(",");
    timer = param.substring(0 , comma);
    param = param.substring(comma + 1);

    comma = param.indexOf(",");
    next_time = param.substring(0, comma);
    newparam = param.substring(comma + 1);
    respond_timer = newparam.substring(0);
    if(param.length() == 5)
    {
        temp_hour = atoi(next_time.c_str());
        temp_min = atoi(respond_timer.c_str());
        if (temp_hour >= 1 && temp_hour <= 12
            && temp_min >= 1 && temp_min <= 59
            && manual.length() > 0 
            && timer.length() > 0
            )
        {
            *set_hour = temp_hour;
            *set_min = temp_min;
            if (!strcmp(manual.c_str(), "OFF"))
            {
                *set_manual = 16;
            }
            else if (!strcmp(manual.c_str(), "ON")) 
            {
                *set_manual = 14; 
            }
            else
            {
                at_serial.println(AT_ERR_PARAM);
                return false;      
            }
            if (!strcmp(timer.c_str(), "OFF"))
            {
                *set_timer = 15;
            }
            else if (!strcmp(timer.c_str(), "ON"))
            {
                *set_timer = 17; 
            }
            else
            {
                at_serial.println(AT_ERR_PARAM);
                return false;      
            }
            at_serial.println(AT_OK);
            return true;
        }
        else
        {
            at_serial.println(AT_ERR_PARAM);
            return false;  
        }    
    }
    else 
    {
        at_serial.println(AT_ERR_PARAM);
        return false;    
    }
}
void pumpATRespond()
{
    if (pump_manual == 14)
    {
        pump_timer = 15;
        digitalWrite(PIN_PUMP, HIGH);
    }
    else
    {
        digitalWrite(PIN_PUMP, LOW);
    }
    if (pump_timer == 17)
    {
        pump_respond = number_setpump_hour*60*60 + timer;
        pump_stop = number_setpump_hour*1*60*60 + timer + number_setpump_min*1*60;
    }
    setpump_manual.setPic(pump_manual);
    setpump_timer.setPic(pump_timer);
}
void feedATRespond()
{
    if (feed_manual == 14)
    {
        feed_timer = 15;
        digitalWrite(PIN_FEED, HIGH);
    }
    else
    {
        digitalWrite(PIN_FEED, LOW);
    }
    if (feed_timer == 17)
    {
        feed_respond = number_setfeed_hour*1*60*60 + timer;
        feed_stop = number_setfeed_hour*1*60*60 + timer + number_setfeed_sec*1;
    }
    setfeed_manual.setPic(feed_manual);
    setfeed_timer.setPic(feed_timer);
}
void uvATRespond()
{
    if (uv_manual == 14)
    {
        uv_timer = 15;
        digitalWrite(PIN_UV, HIGH);
    }
    else
    {
        digitalWrite(PIN_UV, LOW);
    }
    if (uv_timer == 17)
    {
        uv_respond = number_setuv_hour*1*60*60 + timer;
        uv_stop = number_setuv_hour*1*60*60 + timer + number_setuv_min*1*60;
    }
    setuv_manual.setPic(uv_manual);
    setuv_timer.setPic(uv_timer);
}

void lightATRespond(String param)
{
    String newparam;
    String manual;
    String timer;
    uint8_t comma;
    comma = param.indexOf(",");
    manual = param.substring(0 , comma);
    param = param.substring(comma + 1);
    comma = param.indexOf(",");
    timer = param.substring(0 , comma);
    
    param = param.substring(comma + 1);
    comma = param.indexOf(",");
    String light_hour_on = param.substring(0, comma);

    newparam = param.substring(comma + 1);
    comma = newparam.indexOf(",");
    String light_min_on = newparam.substring(0, comma);

    newparam = newparam.substring(comma + 1);
    comma = newparam.indexOf(",");
    String light_hour_off = newparam.substring(0, comma);

    newparam = newparam.substring(comma + 1);
    String light_min_off = newparam.substring(0);
    if(param.length() == 11)
    {
        number_setlight_hour_on = atoi(light_hour_on.c_str());
        number_setlight_min_on = atoi(light_min_on.c_str());
        number_setlight_hour_off = atoi(light_hour_off.c_str());
        number_setlight_min_off = atoi(light_min_off.c_str());
        if(number_setlight_hour_on >= 0 && number_setlight_hour_on <= 23
            &&number_setlight_hour_off >= 0 && number_setlight_hour_off <= 23
            &&number_setlight_min_on >= 0 && number_setlight_min_on <=59
            &&number_setlight_min_off >= 0 && number_setlight_min_off <=59
         )
         {
            if (!strcmp(timer.c_str(),"OFF"))
            {
                light_timer = 40;
            }
            else if (!strcmp(timer.c_str(),"ON")) 
            {
                light_timer = 41;
            } 
            else
            {
                at_serial.println(AT_ERR_PARAM);
            }
            if (!strcmp(manual.c_str(),"OFF"))
            {
                light_manual = 38;
                digitalWrite(PIN_LIGHT, LOW);
                setlight_manual.setPic(light_manual);
                setlight_timer.setPic(light_timer);
                at_serial.println(AT_OK); 
            }
            else if (!strcmp(manual.c_str(),"ON"))
            {
                
                light_manual = 39;
                light_timer = 40; 
                digitalWrite(PIN_LIGHT, HIGH);
                setlight_manual.setPic(light_manual);
                setlight_timer.setPic(light_timer);
                at_serial.println(AT_OK);
            }
            else 
            {
                at_serial.println(AT_ERR_PARAM);    
            }
         }
         else
         {
            at_serial.println(AT_ERR_PARAM);
         }
     }
     else
     {
            at_serial.println(AT_ERR_PARAM);
     }
     
}
void timeATRespond(String param)
{
    
    uint8_t comma;
    
    comma = param.indexOf(",");
    String year = param.substring(0 , comma);
    param = param.substring(comma+1);
    number_settime_year = atoi(year.c_str());

    comma = param.indexOf(",");
    String mon = param.substring(0 , comma);
    param = param.substring(comma+1);
    number_settime_month = atoi(mon.c_str());

    comma = param.indexOf(",");
    String day = param.substring(0 , comma);
    param = param.substring(comma+1);
    number_settime_day = atoi(day.c_str());

    comma = param.indexOf(",");
    String hour = param.substring(0 , comma);
    param = param.substring(comma+1);
    number_settime_hour = atoi(hour.c_str());

    comma = param.indexOf(",");
    String min = param.substring(0 , comma);
    param = param.substring(comma+1);
    number_settime_minute = atoi(min.c_str()); 

    setWeek();
    if (number_settime_minute >= 0 && number_settime_minute <= 59
        && number_settime_hour >= 0 && number_settime_hour <= 23
        && number_settime_month >= 1 && number_settime_month <= 12
        && number_settime_year >= 10 && number_settime_year <= 99
        )
    {
        if (number_settime_month == 1 || number_settime_month == 3
            || number_settime_month == 5 || number_settime_month == 7
            || number_settime_month == 8 || number_settime_month == 10
            || number_settime_month == 12
        )
        {
            if (number_settime_day >=  1 && number_settime_day <= 31)
            {
                rtc.set(0, number_settime_minute, number_settime_hour, 
                    number_settime_week, number_settime_day, 
                    number_settime_month, number_settime_year);
                at_serial.println(AT_OK); 
            }
            else
            {
                at_serial.println(AT_ERR_PARAM);
            }
      
        }
        else if (number_settime_month == 4 || number_settime_month == 6
            || number_settime_month == 9 || number_settime_month == 10
            || number_settime_month == 11
        )
            if (number_settime_day >=  1 && number_settime_day <= 30)
            {
                rtc.set(0, number_settime_minute, number_settime_hour, 
                    number_settime_week, number_settime_day, 
                    number_settime_month, number_settime_year);
                at_serial.println(AT_OK); 
            }
            else
            {
                at_serial.println(AT_ERR_PARAM);
            }
        else if (number_settime_month == 2)
        {
            if ((number_settime_year%4 == 0&&number_settime_year%100 != 0)||number_settime_year%400 == 0)
            {
                if (number_settime_day >= 1 && number_settime_day <=29)
                {
                    rtc.set(0, number_settime_minute, number_settime_hour, 
                        number_settime_week, number_settime_day, 
                        number_settime_month, number_settime_year);
                    at_serial.println(AT_OK); 
                }
                else
                {
                    at_serial.println(AT_ERR_PARAM);
                }
            }
            else
            {
                if (number_settime_day >= 1 && number_settime_day <=28)
                {
                    rtc.set(0, number_settime_minute, number_settime_hour, 
                        number_settime_week, number_settime_day, 
                        number_settime_month, number_settime_year);
                    at_serial.println(AT_OK); 
                }    
                else
                {
                    at_serial.println(AT_ERR_PARAM);
                }
            }
        }
          
    }
    else
    {
        at_serial.println(AT_ERR_PARAM);
    }
     
}
/*Local Synchronization Funcation */
void localSync(String cmd, uint32_t *manual, uint32_t *timer, uint8_t *time_hour, 
                      uint8_t *time_min, uint8_t *time_hour2, uint8_t *time_min2)
{
    cmd += "=";
    if (*manual == 14 || *manual == 39)
    {
        cmd += "ON";
    }
    else if (*manual == 16 || *manual == 38)
    {
        cmd += "OFF";
    }
    cmd += ",";
    if (*timer == 17 || *timer == 41)
    {
        cmd += "ON";
    }
    else if (*timer == 15 || *timer == 40)
    {
        cmd += "OFF";
    }
    cmd += ",";
    if (*time_hour < 10)
    {
        cmd += "0";    
    }
    itoa(*time_hour, buffer, 10);
    cmd += buffer;
    cmd += ",";
    if (*time_min < 10)
    {
        cmd += "0";    
    }
    itoa(*time_min, buffer, 10);
    cmd += buffer;
    String cmd_new = "+IPD,";
    uint8_t len;
    if (*manual == 14 || *manual == 16)
    {
        len = cmd.length();
        itoa(len, buffer, 10);
        cmd_new += buffer;
        cmd_new += ":";
        cmd_new += cmd;
        at_serial.println(cmd_new);
    }
    else
    {
        cmd += ",";
        if (*time_hour2 < 10)
        {
            cmd += "0";    
        }
        itoa(*time_hour2, buffer, 10);
        cmd += buffer;
        cmd += ",";
        if (*time_min2 < 10)
        {
            cmd += "0";    
        }
        itoa(*time_min2, buffer, 10);
        cmd += buffer;
        len = cmd.length();
        itoa(len, buffer, 10);
        cmd_new += buffer;
        cmd_new += ":";
        cmd_new += cmd;
        at_serial.println(cmd_new);
    }
    
}
/*
 *******************************************************************
 * AT partial end
 * 
 *******************************************************************
 */

/*
 *******************************************************************
 * UV partial start
 * 
 *******************************************************************
 */
void uvPopCallback(void *ptr)
{
    uv.show();
    memset(buffer, 0, sizeof(buffer));  
    itoa(number_setuv_hour, buffer, 10);
    setuv_hour.setText(buffer);

    memset(buffer, 0, sizeof(buffer));  
    itoa(number_setuv_min, buffer, 10);
    setuv_min.setText(buffer);

    setuv_timer.setPic(uv_timer);
    setuv_manual.setPic(uv_manual);
}

void uvHourPopCallback(void *ptr)
{
    setuv_type = 1;
}

void uvMinPopCallback(void *ptr)
{
    setuv_type = 2;
}

void uvAddPopCallback(void *ptr)
{
    setuv_up = 1;
    setUv();
}

void uvDECPopCallback(void *ptr)
{
    setuv_up = 0;
    setUv();
}
void setUv(void)
{
    switch (setuv_type)
    {
        case 1: 
            if (setuv_up == 1)
            {
               number_setuv_hour++;
               if (number_setuv_hour > 12)
               {
                    number_setuv_hour = 1;
               }
               memset(buffer, 0, sizeof(buffer));  
               itoa(number_setuv_hour, buffer, 10);
               setuv_hour.setText(buffer);
            }   
            else if (setuv_up == 0)
            {
                number_setuv_hour--;
                if (number_setuv_hour < 1)
                {
                    number_setuv_hour = 12;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setuv_hour, buffer, 10);
                setuv_hour.setText(buffer);
            }
            break;
        case 2:
            if (setuv_up == 1)
            {
                number_setuv_min++;
                if (number_setuv_min > 60)
                {
                    number_setuv_min = 1;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setuv_min, buffer, 10);
                setuv_min.setText(buffer);
            }
            else if (setuv_up == 0)
            {
                number_setuv_min--;
                if (number_setuv_min < 1)
                {
                    number_setuv_min= 60;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setuv_min, buffer, 10);
                setuv_min.setText(buffer);
            }
            break;
    }

}

void uvManualPopCallback(void *ptr)
{
    setuv_manual.getPic(&uv_manual);
    if (uv_manual == 14)
    {
        uv_manual = 16;
        digitalWrite(PIN_UV, LOW);
        
    }
    else
    {
        uv_manual = 14;
        digitalWrite(PIN_UV, HIGH);
    }
    setuv_manual.setPic(uv_manual);

    setuv_timer.getPic(&uv_timer);
    if (uv_timer == 17)
    {
        uv_timer = 15;
        setuv_timer.setPic(uv_timer);
    }
    localSync("UV", &uv_manual, &uv_timer, &number_setuv_hour, &number_setuv_min, NULL, NULL);
}

void uvTimerPopCallback(void *ptr)
{
    
    setuv_timer.getPic(&uv_timer);
   if (uv_manual == 16)
    {
         if (uv_timer == 15)
        {
            uv_timer = 17;
            uv_respond = number_setuv_hour*1*60*60 + timer;
            uv_stop = number_setuv_hour*1*60*60 + timer +number_setuv_min*1*60;
        }
        else
        {
            uv_timer = 15;
            digitalWrite(PIN_UV, LOW);
        }           
    localSync("UV", &uv_manual, &uv_timer, &number_setuv_hour, &number_setuv_min, NULL, NULL);
    }
    setuv_timer.setPic(uv_timer);
}


void uvTimerRespond()
{
    if (uv_timer == 17)
    {
        if (timer >= uv_respond)
        {
            if (timer != uv_stop)
            {
                digitalWrite(PIN_UV, HIGH);
            }
            else
            {
                digitalWrite(PIN_UV, LOW);
                uv_respond = number_setuv_hour*1*60*60 + timer;
                uv_stop = number_setuv_hour*1*60*60 + timer +number_setuv_min*1*60;
            }
        }
    }
}

void uvToSetPopCallback(void *ptr)
{
    set.show();        
}

/*
 *******************************************************************
 * UV partial end
 * 
 *******************************************************************
 */

/*
 *******************************************************************
 * LIGHT partial start
 * 
 *******************************************************************
 */
void lightPopCallback(void *ptr)
{
    light.show();
    memset(buffer, 0, sizeof(buffer));  
    itoa(number_setlight_hour_on, buffer, 10);
    setlight_hour_on.setText(buffer);

    memset(buffer, 0, sizeof(buffer));  
    itoa(number_setlight_min_on, buffer, 10);
    setlight_min_on.setText(buffer);

    memset(buffer, 0, sizeof(buffer));  
    itoa(number_setlight_hour_off, buffer, 10);
    setlight_hour_off.setText(buffer);

    memset(buffer, 0, sizeof(buffer));  
    itoa(number_setlight_min_off, buffer, 10);
    setlight_min_off.setText(buffer);

    setlight_timer.setPic(light_timer);
    setlight_manual.setPic(light_manual);
}

void lightHourOnPopCallback(void *ptr)
{
    setlight_type = 1;
}

void lightHourOffPopCallback(void *ptr)
{
    setlight_type = 2;
}


void lightMinOnPopCallback(void *ptr)
{
    setlight_type = 3;
}

void lightMinOffPopCallback(void *ptr)
{
    setlight_type = 4;
}


void lightAddPopCallback(void *ptr)
{
    setlight_up = 1;
    setLight();
}

void lightDECPopCallback(void *ptr)
{
    setlight_up = 0;
    setLight();
}

void setLight(void)
{
    switch (setlight_type)
    {
        case 1: 
            if (setlight_up == 1)
            {
               number_setlight_hour_on++;
               if (number_setlight_hour_on > 23)
               {
                    number_setlight_hour_on = 0;
               }
               memset(buffer, 0, sizeof(buffer));  
               itoa(number_setlight_hour_on, buffer, 10);
               setlight_hour_on.setText(buffer);
            }   
            else if (setlight_up == 0)
            {
                number_setlight_hour_on--;
                if (number_setlight_hour_on < 0)
                {
                    number_setlight_hour_on = 23;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setlight_hour_on, buffer, 10);
                setlight_hour_on.setText(buffer);
            }
            break;
        case 2: 
            if (setlight_up == 1)
            {
               number_setlight_hour_off++;
               if (number_setlight_hour_off > 23)
               {
                    number_setlight_hour_off = 0;
               }
               memset(buffer, 0, sizeof(buffer));  
               itoa(number_setlight_hour_off, buffer, 10);
               setlight_hour_off.setText(buffer);
            }   
            else if (setlight_up == 0)
            {
                number_setlight_hour_off--;
                if (number_setlight_hour_off < 0)
                {
                    number_setlight_hour_off = 23;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setlight_hour_off, buffer, 10);
                setlight_hour_off.setText(buffer);
            }
            break;    
        case 3:
            if (setlight_up == 1)
            {
                number_setlight_min_on++;
                if (number_setlight_min_on > 60)
                {
                    number_setlight_min_on = 1;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setlight_min_on, buffer, 10);
                setlight_min_on.setText(buffer);
            }
            else if (setlight_up == 0)
            {
                number_setlight_min_on--;
                if (number_setlight_min_on < 1)
                {
                    number_setlight_min_on = 60;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setlight_min_on, buffer, 10);
                setlight_min_on.setText(buffer);
            }
            break;
        case 4:
            if (setlight_up == 1)
            {
                number_setlight_min_off++;
                if (number_setlight_min_off > 60)
                {
                    number_setlight_min_off = 1;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setlight_min_off, buffer, 10);
                setlight_min_off.setText(buffer);
            }
            else if (setlight_up == 0)
            {
                number_setlight_min_off--;
                if (number_setlight_min_off < 1)
                {
                    number_setlight_min_off = 60;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setlight_min_off, buffer, 10);
                setlight_min_off.setText(buffer);
            }
            break;            
    }

}

void lightManualPopCallback(void *ptr)
{   
    setlight_manual.getPic(&light_manual);
    if (light_manual == 39)
    {
        light_manual = 38;
        digitalWrite(PIN_LIGHT, LOW);
    }
    else
    {
        light_manual = 39;
        digitalWrite(PIN_LIGHT, HIGH);
    }
    setlight_manual.setPic(light_manual);
    
    setlight_timer.getPic(&light_timer);
    if (light_timer == 41)
    {
        light_timer = 40;
        setlight_timer.setPic(light_timer);
    }
    localSync("LIGHT", &light_manual, &light_timer, &number_setlight_hour_on, 
        &number_setlight_min_on, &number_setlight_hour_off, &number_setlight_min_off);
}

void lightTimerPopCallback(void *ptr)
{
    setlight_timer.getPic(&light_timer);
    if(light_manual == 38)
    { 
        if (light_timer == 40)
        {
            light_timer = 41;
        }
        else
        {
            light_timer = 40;
            digitalWrite(PIN_LIGHT, LOW);
        }
    localSync("LIGHT", &light_manual, &light_timer, &number_setlight_hour_on, 
        &number_setlight_min_on, &number_setlight_hour_off, &number_setlight_min_off);
    }
    setlight_timer.setPic(light_timer);
}


void lightTimerRespond()
{
    if (light_timer == 41)
    {
        if (rtc.hour() > number_setlight_hour_on && rtc.hour() < number_setlight_hour_off)
        {
            digitalWrite(PIN_LIGHT, HIGH);
        }
        if (rtc.hour() < number_setlight_hour_on && rtc.hour() > number_setlight_hour_off)
        {
            digitalWrite(PIN_LIGHT, LOW);
            
        }
        if (number_setlight_hour_on != number_setlight_hour_off)
        {
            if (rtc.hour() == number_setlight_hour_on)
            {
                if (rtc.minute() >= number_setlight_min_on)
                {
                    digitalWrite(PIN_LIGHT, HIGH);
                }
                else
                {
                    digitalWrite(PIN_LIGHT, LOW);
                }    
            }
            if (rtc.hour() == number_setlight_hour_off)
            {
                if (rtc.minute() < number_setlight_min_off)
                {
                    digitalWrite(PIN_LIGHT, HIGH);
                }
                else
                {
                    digitalWrite(PIN_LIGHT, LOW);
                }
            }
        }
        else
        {
            if (rtc.minute() >= number_setlight_min_on && rtc.minute() < number_setlight_min_off)
            {
                digitalWrite(PIN_LIGHT, HIGH);
            }
            else
            {
                digitalWrite(PIN_LIGHT, LOW);
            }
        }
    }
    
}

void lightToSetPopCallback(void *ptr)
{
    set.show();        
}
/*
 *******************************************************************
 * LIGHT partial end
 * 
 *******************************************************************
 */

/*
 *******************************************************************
 * FEED partial start
 * 
 *******************************************************************
 */
void feedPopCallback(void *ptr)
{
    feed.show();
    memset(buffer, 0, sizeof(buffer));  
    itoa(number_setfeed_hour, buffer, 10);
    setfeed_hour.setText(buffer);

    memset(buffer, 0, sizeof(buffer));  
    itoa(number_setfeed_sec, buffer, 10);
    setfeed_min.setText(buffer);

    setfeed_timer.setPic(feed_timer);
    setfeed_manual.setPic(feed_manual);
}

void feedHourPopCallback(void *ptr)
{
    setfeed_type = 1;
}

void feedMinPopCallback(void *ptr)
{
    setfeed_type = 2;
}

void feedAddPopCallback(void *ptr)
{
    setfeed_up = 1;
    setFeed();
}

void feedDECPopCallback(void *ptr)
{
    setfeed_up = 0;
    setFeed();
}

void setFeed(void)
{
    switch (setfeed_type)
    {
        case 1: 
            if (setfeed_up == 1)
            {
               number_setfeed_hour++;
               if (number_setfeed_hour > 12)
               {
                    number_setfeed_hour = 1;
               }
               memset(buffer, 0, sizeof(buffer));  
               itoa(number_setfeed_hour, buffer, 10);
               setfeed_hour.setText(buffer);
            }   
            else if (setfeed_up == 0)
            {
                number_setfeed_hour--;
                if (number_setfeed_hour < 1)
                {
                    number_setfeed_hour = 12;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setfeed_hour, buffer, 10);
                setfeed_hour.setText(buffer);
            }
            break;
        case 2:
            if (setfeed_up == 1)
            {
                number_setfeed_sec++;
                if (number_setfeed_sec > 60)
                {
                    number_setfeed_sec = 1;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setfeed_sec, buffer, 10);
                setfeed_min.setText(buffer);
            }
            else if (setfeed_up == 0)
            {
                number_setfeed_sec--;
                if (number_setfeed_sec < 1)
                {
                    number_setfeed_sec= 60;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setfeed_sec, buffer, 10);
                setfeed_min.setText(buffer);
            }
            break;
    }

}

void feedManualPopCallback(void *ptr)
{
    setfeed_manual.getPic(&feed_manual);
    if (feed_manual == 14)
    {
        feed_manual = 16;
        digitalWrite(PIN_FEED, LOW);
        delay(10);
    }
    else
    {
        feed_manual = 14;
        digitalWrite(PIN_FEED, HIGH);
        delay(10);
    }
    setfeed_manual.setPic(feed_manual);

    setfeed_timer.getPic(&feed_timer);
    if (feed_timer == 17)
    {
        feed_timer = 15;
        setfeed_timer.setPic(feed_timer);
    }
    localSync("FEED", &feed_manual, &feed_timer, &number_setfeed_hour, &number_setfeed_sec, NULL, NULL);
}

void feedTimerPopCallback(void *ptr)
{
    setfeed_timer.getPic(&feed_timer);
    if (feed_manual == 16)
    {
        if (feed_timer == 15)
        {
            feed_timer = 17;
            feed_respond = number_setfeed_hour*1*60*60 + timer;
            feed_stop = number_setfeed_hour*1*60*60 + timer +number_setfeed_sec*1;
        }
        else
        {
            feed_timer = 15;
            digitalWrite(PIN_FEED, LOW);
        }
    localSync("FEED", &feed_manual, &feed_timer, &number_setfeed_hour, &number_setfeed_sec, NULL, NULL);
    }
    setfeed_timer.setPic(feed_timer);
}


void feedTimerRespond()
{
    if (feed_timer == 17)
    {
        if (timer >= feed_respond)
        {
            if (timer != feed_stop)
            {
                digitalWrite(PIN_FEED, HIGH);
            }
            else
            {
                digitalWrite(PIN_FEED, LOW);
                feed_respond = number_setfeed_hour*1*60*60 + timer;
                feed_stop = number_setfeed_hour*1*60*60 + timer +number_setfeed_sec*1;
            }
        }
    }
}

void feedToSetPopCallback(void *ptr)
{
    set.show();        
}
/*
 *******************************************************************
 * FEED partial end
 * 
 *******************************************************************
 */

 /*
 *******************************************************************
 * PUMP partial start
 * 
 *******************************************************************
 */
void pumpPopCallback(void *ptr)
{
    pump.show();
    memset(buffer, 0, sizeof(buffer));  
    itoa(number_setpump_hour, buffer, 10);
    setpump_hour.setText(buffer);

    memset(buffer, 0, sizeof(buffer));  
    itoa(number_setpump_min, buffer, 10);
    setpump_min.setText(buffer);

    setpump_timer.setPic(pump_timer);
    setpump_manual.setPic(pump_manual);
}

void pumpHourPopCallback(void *ptr)
{
    setpump_type = 1;
}

void pumpMinPopCallback(void *ptr)
{
    setpump_type = 2;
}

void pumpAddPopCallback(void *ptr)
{
    setpump_up = 1;
    setPump();
}

void pumpDECPopCallback(void *ptr)
{
    setpump_up = 0;
    setPump();
}

void setPump(void)
{
    switch (setpump_type)
    {
        case 1: 
            if (setpump_up == 1)
            {
               number_setpump_hour++;
               if (number_setpump_hour > 12)
               {
                    number_setpump_hour = 1;
               }
               memset(buffer, 0, sizeof(buffer));  
               itoa(number_setpump_hour, buffer, 10);
               setpump_hour.setText(buffer);
            }   
            else if (setpump_up == 0)
            {
                number_setpump_hour--;
                if (number_setpump_hour < 1)
                {
                    number_setpump_hour = 12;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setpump_hour, buffer, 10);
                setpump_hour.setText(buffer);
            }
            break;
        case 2:
            if (setpump_up == 1)
            {
                number_setpump_min++;
                if (number_setpump_min > 60)
                {
                    number_setpump_min = 1;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setpump_min, buffer, 10);
                setpump_min.setText(buffer);
            }
            else if (setpump_up == 0)
            {
                number_setpump_min--;
                if (number_setpump_min < 1)
                {
                    number_setpump_min= 60;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_setpump_min, buffer, 10);
                setpump_min.setText(buffer);
            }
            break;
    }

}
void pumpManualPopCallback(void *ptr)
{
    setpump_manual.getPic(&pump_manual);
    if (pump_manual == 14)
    {
        pump_manual = 16;
        digitalWrite(PIN_PUMP, LOW);
        delay(10);
    }
    else
    {
        pump_manual = 14;
        digitalWrite(PIN_PUMP, HIGH);
        delay(10);
    }
    setpump_manual.setPic(pump_manual);

    if (pump_timer == 17)
    {
        pump_timer = 15;
        setpump_timer.setPic(pump_timer);
    }
    localSync("PUMP", &pump_manual, &pump_timer, &number_setpump_hour, &number_setpump_min, NULL, NULL);
}

void pumpTimerPopCallback(void *ptr)
{
    setpump_timer.getPic(&pump_timer);
    if(pump_manual == 16)
    {
        if (pump_timer == 15)
        {
            pump_timer = 17;
            pump_respond = number_setpump_hour*1*60*60 + timer;
            pump_stop = number_setpump_hour*1*60*60 + timer + number_setpump_min*1*60;
        }
        else if (pump_timer == 17)
        {
            pump_timer = 15;
            digitalWrite(PIN_PUMP, LOW);
        }
    localSync("PUMP", &pump_manual, &pump_timer, &number_setpump_hour, &number_setpump_min, NULL, NULL);
    }
    setpump_timer.setPic(pump_timer);
    
}



void pumpTimerRespond()
{
    if (pump_timer == 17)
    {
        if (timer >= pump_respond)
        {
            if (timer != pump_stop)
            {
                digitalWrite(PIN_PUMP, HIGH);
            }
            else
            {
                digitalWrite(PIN_PUMP, LOW);
                pump_respond = number_setpump_hour*1*60*60 + timer;
                pump_stop = number_setpump_hour*1*60*60 + timer + number_setpump_min*1*60;
            }
        }
    }
}
void pumpToSetPopCallback(void *ptr)
{
    set.show();        
}
/*
*******************************************************************
* PUMP partial end
* 
*******************************************************************
*/

/*
*******************************************************************
* TIME partial start
* 
*******************************************************************
*/
void timePopCallback(void *ptr)
{
    time.show();
    setyear.setText(buffer_year);

    memset(buffer, 0, sizeof(buffer)); 
    number_settime_month = rtc.month();
    itoa(number_settime_month, buffer, 10);
    setmon.setText(buffer);
    
    setday.setText(buffer_day);
    sethour.setText(buffer_hour);
    setmin.setText(buffer_minute);
    setweek.setText(buffer_week);
    number_settime_year = atoi(buffer_year);
    number_settime_day = atoi(buffer_day);
    number_settime_hour = atoi(buffer_hour);
    number_settime_minute = atoi(buffer_minute);
    number_settime_week = rtc.dayOfWeek();
    
    
}

void timeOkPopCallback(void *ptr)
{
    rtc.set(0, number_settime_minute, number_settime_hour, 
            number_settime_week, number_settime_day, 
            number_settime_month, number_settime_year);
}

void timeAddPopCallback(void *ptr)
{
    settime_up = 1;
    setTime();
    setWeek();
}

void timeDECPopCallback(void *ptr)
{
    settime_up = 0;
    setTime();
    setWeek();
}

void setYearPopCallback(void *ptr)
{
    settime_type = 1;   
}

void setMonPopCallback(void *ptr)
{
    settime_type = 2;
}

void setDayPopCallback(void *ptr)
{
    settime_type = 3;
}

void setHourPopCallback(void *ptr)
{
    settime_type = 4;
}

void setMinPopCallback(void *ptr)
{
    settime_type = 5;
}

void setWeekPopCallback(void *ptr)
{
    settime_type = 6;

}

void setTime(void)
{
    switch (settime_type)
    {
        case 1: 
            if (settime_up == 1)
            {
               number_settime_year++;
               if (number_settime_year > 99)
               {
                    number_settime_year = 10;
               }
               memset(buffer, 0, sizeof(buffer));  
               itoa(number_settime_year, buffer, 10);
               setyear.setText(buffer);
            }   
            else if (settime_up == 0)
            {
                number_settime_year--;
                if (number_settime_year < 10)
                {
                    number_settime_year = 99;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_settime_year, buffer, 10);
                setyear.setText(buffer);
            }
            break;
        case 2:
            if (settime_up == 1)
            {
                number_settime_month++;
                if (number_settime_month > 12)
                {
                    number_settime_month = 1;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_settime_month, buffer, 10);
                setmon.setText(buffer);
            }
            else if (settime_up == 0)
            {
                number_settime_month--;
                if (number_settime_month < 1)
                {
                    number_settime_month= 12;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_settime_month, buffer, 10);
                setmon.setText(buffer);
            }
            break;
        case 3:
        {
            
            int8_t dayofmonth = number_settime_month;
            if (settime_up == 1)
            {
                number_settime_day++;
                if (dayofmonth == 1 || dayofmonth == 3 || dayofmonth == 5 
                    || dayofmonth == 7 || dayofmonth == 8 || dayofmonth == 10
                    || dayofmonth == 12 )
                {
                    if (number_settime_day > 31)
                    {
                        number_settime_day = 1;
                    }
                }
                if (dayofmonth == 4 || dayofmonth == 6 || dayofmonth == 9 
                    || dayofmonth == 11)
                {
                    if (number_settime_day > 30)
                    {
                        number_settime_day = 1;
                    }
                }
                if (dayofmonth == 2)
                {
                   if ((number_settime_year%4 == 0&&number_settime_year%100 != 0)||number_settime_year%400 == 0)
                    {
                        if (number_settime_day > 29)
                        {
                            number_settime_day = 1;
                        }
                    }
                    else
                    {
                        if (number_settime_day > 28)
                        {
                            number_settime_day = 1;
                        }
                    }
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_settime_day, buffer, 10);
                setday.setText(buffer);
            }
            else if (settime_up == 0)
            {
                number_settime_day--;
                if (dayofmonth == 1 || dayofmonth == 3 || dayofmonth == 5 
                    || dayofmonth == 7 || dayofmonth == 8 || dayofmonth == 10
                    || dayofmonth == 12 )
                {
                    if (number_settime_day < 1)
                    {
                        number_settime_day = 31;
                    }
                }
                if (dayofmonth == 4 || dayofmonth == 6 || dayofmonth == 9 
                    || dayofmonth == 11)
                {
                    if (number_settime_day < 1)
                    {
                        number_settime_day = 30;
                    }
                }
                if (dayofmonth == 2)
                {
                    if ((number_settime_year%4 == 0&&number_settime_year%100 != 0)||number_settime_year%400 == 0)
                    {
                        if (number_settime_day < 1)
                        {
                            number_settime_day = 29;
                        }
                    }
                    else
                    {
                        if (number_settime_day < 1)
                        {
                            number_settime_day = 28;
                        }
                    }
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_settime_day, buffer, 10);
                setday.setText(buffer);
            }
            break;
        }
        case 4:
            if (settime_up == 1)
            {
                number_settime_hour++;
                if (number_settime_hour > 23)
                {
                    number_settime_hour = 0;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_settime_hour, buffer, 10);
                sethour.setText(buffer);
            }
            else if (settime_up == 0)
            {
                number_settime_hour--;
                if (number_settime_hour < 0)
                {
                    number_settime_hour = 23;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_settime_hour, buffer, 10);
                sethour.setText(buffer);
            }
            break;
        case 5:
            if (settime_up == 1)
            {
                number_settime_minute++;
                if (number_settime_minute > 59)
                {
                    number_settime_minute = 0;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_settime_minute, buffer, 10);
                setmin.setText(buffer);
            }
            else if (settime_up == 0)
            {
                number_settime_minute--;
                if (number_settime_minute < 0)
                {
                    number_settime_minute = 59;
                }
                memset(buffer, 0, sizeof(buffer));  
                itoa(number_settime_minute, buffer, 10);
                setmin.setText(buffer);
            }
            break;                
    }
}

void setWeek(void)
{
    int32_t year;
    int32_t month;
    if(number_settime_month < 3)
    {
        month = number_settime_month + 12;
        year = number_settime_year-1;
    }
    else
    {
        month = number_settime_month;
        year = number_settime_year;
    }
    
    number_settime_week =year + year/4 + 20/4 - 2*20 + ((26*(month+1))/10 + number_settime_day - 1);
    number_settime_week = (number_settime_week %7 + 7)%7 + 1; 
    switch (number_settime_week)
    {
        case 1:
                strcpy(buffer, "Sunday");
                break;
        case 2: 
                strcpy(buffer, "Monday");
                break;
        case 3:
                strcpy(buffer, "Tuesday");
                break;
        case 4:
                strcpy(buffer, "Wednesday");
                break;
        case 5:
                strcpy(buffer, "Thursday");
                break;
        case 6:
                strcpy(buffer, "Friday");
                break;
        case 7:
                strcpy(buffer, "Saturday");
                break;
        default:
                strcpy(buffer, "fail");
                break;
    }
    setweek.setText(buffer);
}

void timeToSetPopCallback(void *ptr)
{
    set.show();        
}


/*
*******************************************************************
* TIME partial end
* 
*******************************************************************
*/

void backHomePopCallback(void *ptr)
{
    home.show();
    refreshHome();
    
}

void  refreshHome(void)
{
    year.setText(buffer_year);
    month.setText(buffer_month);
    day.setText(buffer_day);
    hour.setText(buffer_hour);
    minute.setText(buffer_minute);
    week.setText(buffer_week);
    temperature.setText(buffer_temperature);
    if (pump_timer == 17 )
    {
        home_pumppic.setPic(20);
    }
    else
    {
        home_pumppic.setPic(24);    
    } 
     if (feed_timer == 17 )
    {
        home_feedpic.setPic(21);
    }
    else
    {
        home_feedpic.setPic(25);    
    }
     if (light_timer == 41 )
    {
        home_lightpic.setPic(22);
    }
    else
    {
        home_lightpic.setPic(26);    
    }
    if (uv_timer == 17 )
    {
        home_uvpic.setPic(23);
    }
    else
    {
        home_uvpic.setPic(27);    
    }  
}

void goSetPopCallback(void *ptr)
{
    set.show();     
}

void timeDisplay(void)
{
    memset(buffer, 0, sizeof(buffer)); 
    number = rtc.hour();
    itoa(number, buffer, 10);
    if (strcmp(buffer_hour, buffer))
    {
        hour.setText(buffer);
        strcpy(buffer_hour, buffer); 
    }

    memset(buffer, 0, sizeof(buffer));
    memset(buffer_temp, 0, sizeof(buffer_temp));
    number = rtc.minute();
    itoa(number, buffer_temp, 10);
    if (rtc.minute() < 10)
    {
        strcat(buffer, "0");
    }
    strcat(buffer, buffer_temp);
    if (strcmp(buffer_minute, buffer))
    {
        minute.setText(buffer);
        strcpy(buffer_minute, buffer); 
    }
    
    memset(buffer, 0, sizeof(buffer)); 
    memset(buffer_temp, 0, sizeof(buffer_temp)); 
    number = rtc.second();
    if(number != buffer_second)
    {
        timer++;
        if (number%2)
        {
            strcpy(buffer,":");
            second.setText(buffer);
            buffer_second = number;
        }
        else
        {
            strcpy(buffer," ");
            second.setText(buffer);
            buffer_second = number;
        }
    }
    switch (rtc.dayOfWeek())
    {
        case 1:
                strcpy(buffer, "Sunday");
                break;
        case 2: 
                strcpy(buffer, "Monday");
                break;
        case 3:
                strcpy(buffer, "Tuesday");
                break;
        case 4:
                strcpy(buffer, "Wednesday");
                break;
        case 5:
                strcpy(buffer, "Thursday");
                break;
        case 6:
                strcpy(buffer, "Friday");
                break;
        case 7:
                strcpy(buffer, "Saturday");
                break;
        default:
                strcpy(buffer, "fail");
                break;
    }

    if (strcmp(buffer_week, buffer))
    {
        week.setText(buffer);
        strcpy(buffer_week, buffer);
    }

    memset(buffer, 0, sizeof(buffer));
    memset(buffer_temp, 0, sizeof(buffer_temp));
    number = rtc.year();
    itoa(number, buffer, 10);
    if (strcmp(buffer_year, buffer))
    {
        year.setText(buffer);
        strcpy(buffer_year, buffer);
    }
    
    switch (rtc.month())
    {
        case 1:
                strcpy(buffer, "Jan.");
                break;
        case 2: 
                strcpy(buffer, "Feb.");
                break;
        case 3:
                strcpy(buffer, "Mar.");
                break;
        case 4:
                strcpy(buffer, "Apr.");
                break;
        case 5:
                strcpy(buffer, "May.");
                break;
        case 6:
                strcpy(buffer, "Jun.");
                break;
        case 7:
                strcpy(buffer, "Jul.");
                break;
        case 8: 
                strcpy(buffer, "Aug.");
                break;
        case 9:
                strcpy(buffer, "Sep.");
                break;
        case 10:
                strcpy(buffer, "Oct.");
                break;
        case 11:
                strcpy(buffer, "Nov.");
                break;
        case 12:
                strcpy(buffer, "Dec.");
                break;
        default:
                strcpy(buffer, "fail");
                break;
    }
    if (strcmp(buffer_month, buffer))
    {
        month.setText(buffer);
        strcpy(buffer_month, buffer);
    }
    
    memset(buffer, 0, sizeof(buffer));
    number = rtc.day();
    itoa(number, buffer, 10);
    if (strcmp(buffer_day, buffer))
    {
        day.setText(buffer);
        strcpy(buffer_day, buffer);
    }
}

void temperatureDisplay(void)
{
    byte i;
    byte present = 0;
    byte type_s = 0;
    byte data[12];
    byte addr[8];
    float celsius;

    ds.search(addr);

    ds.reset();
    ds.select(addr);
    ds.write(0x44,1);                       /* start conversion, with parasite power on at the end */

    delay(1000);                            /* maybe 750ms is enough, maybe not */
                                            /* we might do a ds.depower() here, but the reset will take care of it. */
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE);                         /* Read Scratchpad */

    for ( i = 0; i < 9; i++) 
    {   /* we need 9 bytes */
        data[i] = ds.read();
    }
    /* convert the data to actual temperature*/
    unsigned int raw = (data[1] << 8) | data[0];
    if (type_s) 
    {
        raw = raw << 3;                         /* 9 bit resolution default */
        if (data[7] == 0x10) 
        {
            /* count remain gives full 12 bit resolution */
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    } 
    else 
    {
        byte cfg = (data[4] & 0x60);
        if (cfg == 0x00) raw = raw << 3; 
        else if (cfg == 0x20) raw = raw << 2; 
        else if (cfg == 0x40) raw = raw << 1; 
        /* default is 12 bit resolution, 750 ms conversion time */
    }
    celsius = (float)raw / 16.0;
    number = ((float)celsius/60)*180+30;
    pointer.setValue(number);

    memset(buffer, 0, sizeof(buffer));
    itoa(celsius, buffer, 10);
    temperature.setText(buffer);
     
    
}


void setup(void)
{
    nexInit();
    atInit();
    goset.attachPop(goSetPopCallback);
    backhome.attachPop(backHomePopCallback);
    settime.attachPop(timePopCallback);
    settime_ok.attachPop(timeOkPopCallback);
    settime_add.attachPop(timeAddPopCallback);
    settime_dec.attachPop(timeDECPopCallback);
    setyear.attachPop(setYearPopCallback);
    setmon.attachPop(setMonPopCallback);
    setday.attachPop(setDayPopCallback);
    sethour.attachPop(setHourPopCallback);
    setmin.attachPop(setMinPopCallback);
    setweek.attachPop(setWeekPopCallback);
    time_back_to_set.attachPop(timeToSetPopCallback);

    setpump.attachPop(pumpPopCallback);
    setpump_manual.attachPop(pumpManualPopCallback);
    setpump_timer.attachPop(pumpTimerPopCallback);
    setpump_add.attachPop(pumpAddPopCallback);
    setpump_dec.attachPop(pumpDECPopCallback);
    setpump_hour.attachPop(pumpHourPopCallback);
    setpump_min.attachPop(pumpMinPopCallback);
    pump_back_to_set.attachPop(pumpToSetPopCallback);

    setfeed.attachPop(feedPopCallback);
    setfeed_manual.attachPop(feedManualPopCallback);
    setfeed_timer.attachPop(feedTimerPopCallback);
    setfeed_add.attachPop(feedAddPopCallback);
    setfeed_dec.attachPop(feedDECPopCallback);
    setfeed_hour.attachPop(feedHourPopCallback);
    setfeed_min.attachPop(feedMinPopCallback);
    feed_back_to_set.attachPop(feedToSetPopCallback);

    setuv.attachPop(uvPopCallback);
    setuv_manual.attachPop(uvManualPopCallback);
    setuv_timer.attachPop(uvTimerPopCallback);
    setuv_add.attachPop(uvAddPopCallback);
    setuv_dec.attachPop(uvDECPopCallback);
    setuv_hour.attachPop(uvHourPopCallback);
    setuv_min.attachPop(uvMinPopCallback);
    uv_back_to_set.attachPop(uvToSetPopCallback);

    setlight.attachPop(lightPopCallback);
    setlight_manual.attachPop(lightManualPopCallback);
    setlight_timer.attachPop(lightTimerPopCallback);
    setlight_add.attachPop(lightAddPopCallback);
    setlight_dec.attachPop(lightDECPopCallback);
    setlight_hour_on.attachPop(lightHourOnPopCallback);
    setlight_hour_off.attachPop(lightHourOffPopCallback);
    setlight_min_on.attachPop(lightMinOnPopCallback);
    setlight_min_off.attachPop(lightMinOffPopCallback);
    light_back_to_set.attachPop(lightToSetPopCallback);
    
    pinMode(PIN_LIGHT, OUTPUT);
    pinMode(PIN_UV, OUTPUT);
    pinMode(PIN_FEED, OUTPUT);
    pinMode(PIN_PUMP, OUTPUT); 
    digitalWrite(PIN_LIGHT, LOW);
    digitalWrite(PIN_UV, LOW);
    digitalWrite(PIN_FEED, LOW);
    digitalWrite(PIN_PUMP, LOW);
   /* RTCLib::set(byte second, byte minute, byte hour, 
    * byte dayOfWeek, byte dayOfMonth, byte month, byte year);
    */
   /*rtc.set(0, 5, 16, 3, 15, 2, 16);*/ 
}


void loop(void)
{
    
    if (timer > update)
    {
        temperatureDisplay();
        update += 30;
    }
    nexLoop(nex_Listen_List);
    rtc.refresh();
    timeDisplay();
    pumpTimerRespond();
    feedTimerRespond();
    uvTimerRespond();
    lightTimerRespond();
    if (timer > 3)
    {
        atRespond();
    }
    else
    {
        while (at_serial.available() > 0)
        {
            at_serial.read();
            Serial.print((uint8_t)at_serial.read(),HEX);
        }
    }
    
}
