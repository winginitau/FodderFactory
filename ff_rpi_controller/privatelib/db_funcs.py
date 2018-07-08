'''
Created on 2 Jul. 2018

@author: brendan
'''

from configparser import ConfigParser
 
#import mysql.connector
from mysql.connector import MySQLConnection, Error
from datetime import datetime, timedelta


#db_result_list = []

def iter_row(cursor, size=10):
    while True:
        rows = cursor.fetchmany(size)
        if not rows:
            break
        for row in rows:
            yield row

def db_energy_data(energy_message, endDT=datetime.now(), \
                   periodTD=timedelta(days=3), db_result_list = []):    
    #global db_result_list
    
    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
 
        startDT = endDT - periodTD
        
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        
        query = "SELECT datetime, int_val from message_log " \
                "WHERE source = 'system' " \
                "AND msg_type = 'DATA' " \
                "AND message = %s " \
                "AND datetime BETWEEN %s and %s " \
                "ORDER BY datetime"
                #"and time <= '00:07:00'"
            
        #args = (block_label,)
        args = (energy_message, startDTstr, endDTstr)
        
        cursor.execute(query, args)

        db_result_list = []
        for row in iter_row(cursor, 10):
            #print (row)
            td = row[0] - startDT
            tds = td.total_seconds()
            tdsi = int(tds)
            if energy_message == "VE_DATA_VOLTAGE":
                db_result_list.append( (tdsi, row[1]/1000, ) )
            elif energy_message == "VE_DATA_CURRENT":
                db_result_list.append( (tdsi, row[1]/1000, ) )
            elif energy_message == "VE_DATA_SOC":
                db_result_list.append( (tdsi, row[1]/10, ) )
            elif energy_message == "VE_DATA_POWER":
                db_result_list.append( (tdsi, row[1], ) )
                                       
        #print(db_result_list)
 
    except Error as e:
        print(e)
 
    finally:
        cursor.close()
        conn.close()

    return db_result_list


def db_device_data(block_label, endDT=datetime.now(), \
                   periodTD=timedelta(days=1), db_result_list = []):
    #global db_result_list
    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
 
        startDT = endDT - periodTD
        
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        
        query = "SELECT datetime, msg_type from message_log " \
                "WHERE source = %s " \
                "AND datetime BETWEEN %s and %s " \
                "ORDER BY datetime"
                #"and time <= '00:07:00'"
            
        #args = (block_label,)
        args = (block_label, startDTstr, endDTstr)
        
        cursor.execute(query, args)

        db_result_list = []
        for row in iter_row(cursor, 10):
            #print (row)
            td = row[0] - startDT
            tds = td.total_seconds()
            tdsi = int(tds)
            if row[1] == "ACTIVATED":
                db_result_list.append( (tdsi, 0, ) )
                db_result_list.append( (tdsi, 1, ) )
            if row[1] == "DEACTIVATED":
                db_result_list.append( (tdsi, 1, ) )
                db_result_list.append( (tdsi, 0, ) )
                                       
        #print(db_result_list)
 
    except Error as e:
        print(e)
 
    finally:
        cursor.close()
        conn.close()

    return db_result_list


def db_temperature_data(block_label, endDT=datetime.now(), \
                   periodTD=timedelta(days=1), db_result_list = []):
    
    #global db_result_list
    try:
        dbconfig = read_db_config()
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
 
        startDT = endDT - periodTD
        
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        
        query = "SELECT datetime, float_val from message_log " \
                "WHERE source = %s " \
                "AND datetime BETWEEN %s and %s " \
                "ORDER BY datetime"
                #"and time <= '00:07:00'"
            
        #args = (block_label,)
        args = (block_label, startDTstr, endDTstr)
        
        cursor.execute(query, args)

        db_result_list = []
        for row in iter_row(cursor, 10):
            #print (row)
            td = row[0] - startDT
            tds = td.total_seconds()
            tdsi = int(tds)
            db_result_list.append( (tdsi, row[1]), )
        #print(db_result_list)
 
    except Error as e:
        print(e)
 
    finally:
        cursor.close()
        conn.close()
    
    return db_result_list
        
def db_add_log_entry(parsed_message):
    
    log_data = {
        'datetime': parsed_message.dt.strftime("%Y-%m-%d %H:%M:%S"),
        'date': parsed_message.dt.strftime("%Y-%m-%d"),
        'time': parsed_message.dt.strftime("%H:%M:%S"),
        'source': parsed_message.source_block_string,
        'destination': parsed_message.dest_block_string,
        'msg_type': parsed_message.msg_type_string,
        'message': parsed_message.msg_string,
        'int_val': parsed_message.int_val,
        'float_val': parsed_message.float_val,
        }

    query = "INSERT INTO message_log " \
            "(datetime, date, time, source, destination, msg_type, message, int_val, float_val) " \
            "VALUES" \
            "(%(datetime)s, %(date)s, %(time)s, %(source)s,%(destination)s, " \
            "%(msg_type)s, %(message)s, %(int_val)s, %(float_val)s)"

    try:
        db_config = read_db_config()
        conn = MySQLConnection(**db_config) 
        cursor = conn.cursor()
        cursor.execute(query, log_data)
        conn.commit()
    except Error as error:
        print(error)
 
    finally:
        cursor.close()
        conn.close()

def read_db_config(filename='controller_config.ini', section='mysql'):
    """ Read database configuration file and return a dictionary object
    :param filename: name of the configuration file
    :param section: section of database configuration
    :return: a dictionary of database parameters
    """
    # create parser and read ini configuration file
    parser = ConfigParser()
    parser.read(filename)
 
    # get section, default to mysql
    db = {}
    if parser.has_section(section):
        items = parser.items(section)
        for item in items:
            db[item[0]] = item[1]
    else:
        raise Exception('{0} not found in the {1} file'.format(section, filename))
 
    return db

def DBConnectTest():
    """ Connect to MySQL database """
 
    db_config = read_db_config()
 
    try:
        print('Connecting to MySQL database...')
        conn = MySQLConnection(**db_config)
 
        if conn.is_connected():
            print('Database Connection Tested OK')
        else:
            print('Database Connection Failed.')
            exit()
 
    except Error as error:
        print(error)
 
    finally:
        conn.close()
        #print('Connection closed.')

