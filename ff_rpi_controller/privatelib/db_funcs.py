'''
Created on 2 Jul. 2018

@author: brendan
'''

from configparser import ConfigParser
from collections import deque
#import mysql.connector
from mysql.connector import MySQLConnection, Error
from datetime import datetime, timedelta
from privatelib.ff_config import ParsedMessage


def iter_row(cursor, size=10):
    while True:
        rows = cursor.fetchmany(size)
        if not rows:
            break
        for row in rows:
            yield row

def db_get_last_energy_message_by_string(msg_string, message: ParsedMessage, look_back_mins, db='mysql'):
    try:
        dbconfig = read_db_config(db=db)
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor(buffered=True)
        endDT = datetime.now()
        periodTD = timedelta(minutes=look_back_mins)
        startDT = endDT - periodTD
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        query = "SELECT datetime, source, msg_type, message, int_val, float_val FROM message_log " \
                "WHERE datetime BETWEEN %s and %s " \
                "AND source = 'system' " \
                "AND msg_type = 'DATA' " \
                "AND message = %s " \
                "ORDER BY datetime DESC " \
                "LIMIT 1"
        args = (startDTstr, endDTstr, msg_string)
        cursor.execute(query, args)
        for row in iter_row(cursor, 10):
            message.dt = row[0]
            message.source_block_string = row[1]
            message.msg_type_string = row[2]
            message.msg_string = row[3]
            message.int_val = row[4]
            message.float_val = row[5]
            message.valid = True
            message.time_rx = row[0]
    except Error as e:
        print(e)
    try:
        cursor.close()
    except Error as error:
        print(error)
    try:
        conn.close()
    except Error as error:
        print(error)
    return message


def db_get_last_message_by_source(source, message: ParsedMessage, look_back_mins, db='mysql'):
    try:
        dbconfig = read_db_config(db=db)
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor(buffered=True)
        endDT = datetime.now()
        periodTD = timedelta(minutes=look_back_mins)
        startDT = endDT - periodTD
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        query = "SELECT datetime, source, msg_type, message, int_val, float_val FROM message_log " \
                "WHERE datetime BETWEEN %s and %s " \
                "AND source = %s " \
                "ORDER BY datetime DESC " \
                "LIMIT 1"
        args = (startDTstr, endDTstr, source)
        cursor.execute(query, args)
        for row in iter_row(cursor, 10):
            message.dt = row[0]
            message.source_block_string = row[1]
            message.msg_type_string = row[2]
            message.msg_string = row[3]
            message.int_val = row[4]
            message.float_val = row[5]
            message.valid = True
            message.time_rx = row[0]
    except Error as e:
        print(e)
    try:
        cursor.close()
    except Error as error:
        print(error)
    try:
        conn.close()
    except Error as error:
        print(error)
    return message
    
def db_log_history(endDT=datetime.now(), limit=100, \
                   periodTD=timedelta(days=1), db_result_list = [], db='mysql'):    
    try:
        dbconfig = read_db_config(db=db)
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor(buffered=True)
        startDT = endDT - periodTD
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        query = "SELECT datetime, source, msg_type, message FROM message_log " \
                "WHERE datetime BETWEEN %s and %s " \
                "ORDER BY datetime DESC " \
                "LIMIT %s "
        args = (startDTstr, endDTstr, limit)
        cursor.execute(query, args)
        #text_result = ""
        #db_result_list = []
        for row in iter_row(cursor, 10):
            #text_result = row[0].strftime("%H:%M:%S") + " " + row[1] + " " + row[2] + " " + row[3]
            #db_result_list.append(text_result, )
            db_result_list.append(row,)
        db_result_list.reverse()
    except Error as e:
        print(e)
    try:
        cursor.close()
    except Error as error:
        print(error)
    try:
        conn.close()
    except Error as error:
        print(error)
    return db_result_list

def db_energy_data_new(energy_message, endDT=datetime.now(), \
                   periodTD=timedelta(days=3), db_result_list = [], db='mysql'):    
    try:
        dbconfig = read_db_config(db=db)
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
        startDT = endDT - periodTD
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        sample_mod = periodTD.days
        if sample_mod == 3: sample_mod = 6
        elif sample_mod == 4: sample_mod = 14
        elif sample_mod >= 5:
            sample_mod = sample_mod * 5   
            
        drop_tmp1 = "DROP TEMPORARY TABLE IF EXISTS tmp1;"
        drop_tmp2 = "DROP TEMPORARY TABLE IF EXISTS tmp2;"
        drop_tmp3 = "DROP TEMPORARY TABLE IF EXISTS tmp3;"

        create_tmp1 =   "CREATE TEMPORARY TABLE tmp1 (id INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY) " \
                        "SELECT datetime, int_val " \
                        "FROM message_log " \
                        "WHERE source = 'system' " \
                        "AND msg_type = 'DATA' " \
                        "AND message = %s " \
                        "AND datetime BETWEEN %s and %s " \
                        "ORDER BY datetime; " 
        create_tmp1_args = (energy_message, startDTstr, endDTstr)

        create_tmp2 =   "CREATE TEMPORARY TABLE tmp2 " \
                        "SELECT * FROM tmp1; "

        create_tmp3 =   "CREATE TEMPORARY TABLE tmp3 " \
                        "SELECT * FROM tmp1; "

        main_query =    "SELECT A.datetime, A.int_val " \
                        "FROM tmp1 A, tmp2 P, tmp3 N " \
                        "WHERE A.id = P.id + 1 " \
                        "AND A.id = N.id - 1 " \
                        "AND ( " \
                        "        ( " \
                        "        A.int_val > P.int_val " \
                        "        AND A.int_val > N.int_val " \
                        "        ) " \
                        "    OR  ( " \
                        "        A.int_val < P.int_val " \
                        "        AND A.int_val < N.int_val " \
                        "        ) " \
                        "    OR A.id mod %s = 0 " \
                        ") " \
                        "ORDER BY A.datetime; "
        main_query_args = (sample_mod, )
                 
        cursor.execute(drop_tmp1)
        cursor.execute(drop_tmp2)
        cursor.execute(drop_tmp3)
        cursor.execute(create_tmp1, create_tmp1_args)
        cursor.execute(create_tmp2)
        cursor.execute(create_tmp3)
        cursor.execute(main_query, main_query_args)
        
        db_result_list = []
        record_count = 0
        for row in iter_row(cursor, 10):
            record_count += 1
            td = row[0] - startDT
            tds = td.total_seconds()
            tdsi = int(tds)
            if energy_message == "VE_DATA_VOLTAGE":
                db_result_list.append( (tdsi, row[1]/1000, row[0], ) )
            elif energy_message == "VE_DATA_CURRENT":
                db_result_list.append( (tdsi, row[1]/1000, row[0], ) )
            elif energy_message == "VE_DATA_SOC":
                db_result_list.append( (tdsi, row[1]/10, row[0], ) )
            elif energy_message == "VE_DATA_POWER":
                db_result_list.append( (tdsi, row[1], row[0], ) )
        print("Records retrieved = " + str(record_count))
    except Error as e:
        print(e)
    try:
        cursor.close()
    except Error as error:
        print(error)
    try:
        conn.close()
    except Error as error:
        print(error)
    return db_result_list

def db_energy_data(energy_message, endDT=datetime.now(), \
                   periodTD=timedelta(days=3), db_result_list = [], db='mysql'):    
    try:
        dbconfig = read_db_config(db=db)
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
        startDT = endDT - periodTD
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        sample_mod = periodTD.days
        if sample_mod == 3: sample_mod = 6
        elif sample_mod == 4: sample_mod = 14
        elif sample_mod >= 5:
            sample_mod = sample_mod * 5        
        query = "SELECT datetime, int_val from message_log " \
                "WHERE source = 'system' " \
                "AND msg_type = 'DATA' " \
                "AND message = %s " \
                "AND datetime BETWEEN %s and %s " \
                "AND id mod %s = 0 " \
                "ORDER BY datetime"
        args = (energy_message, startDTstr, endDTstr, sample_mod)
        cursor.execute(query, args)
        db_result_list = []
        record_count = 0
        for row in iter_row(cursor, 10):
            record_count += 1
            td = row[0] - startDT
            tds = td.total_seconds()
            tdsi = int(tds)
            if energy_message == "VE_DATA_VOLTAGE":
                db_result_list.append( (tdsi, row[1]/1000, row[0], ) )
            elif energy_message == "VE_DATA_CURRENT":
                db_result_list.append( (tdsi, row[1]/1000, row[0], ) )
            elif energy_message == "VE_DATA_SOC":
                db_result_list.append( (tdsi, row[1]/10, row[0], ) )
            elif energy_message == "VE_DATA_POWER":
                db_result_list.append( (tdsi, row[1], row[0], ) )
        print("Records retrieved = " + str(record_count))
    except Error as e:
        print(e)
    try:
        cursor.close()
    except Error as error:
        print(error)
    try:
        conn.close()
    except Error as error:
        print(error)
    return db_result_list

def db_device_data(block_label, endDT=datetime.now(), \
                   periodTD=timedelta(days=1), db_result_list = [], db='mysql'):
    try:
        dbconfig = read_db_config(db=db)
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
        startDT = endDT - periodTD
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        sample_mod = 1
        query = "SELECT datetime, msg_type from message_log " \
                "WHERE source = %s " \
                "AND datetime BETWEEN %s and %s " \
                "AND id mod %s = 0 " \
                "ORDER BY datetime"
        args = (block_label, startDTstr, endDTstr, sample_mod)
        cursor.execute(query, args)
        record_count = 0
        for row in iter_row(cursor, 10):
            record_count += 1
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
        print("Records retrieved = " + str(record_count))
    except Error as e:
        print(e)
    try:
        cursor.close()
    except Error as error:
        print(error)
    try:
        conn.close()
    except Error as error:
        print(error)
    return db_result_list


def db_temperature_data_new(block_label, endDT=datetime.now(), \
                   periodTD=timedelta(days=1), db_result_list = [], db='mysql.local'):
    try:
        dbconfig = read_db_config(db=db)
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
        startDT = endDT - periodTD
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        sample_mod = periodTD.days
        if sample_mod == 2: sample_mod = 1
        elif sample_mod == 3: sample_mod = 5
        elif sample_mod == 4: sample_mod = sample_mod * 2
        elif sample_mod >= 5:
            sample_mod = sample_mod * 3

        drop_tmp1 = "DROP TEMPORARY TABLE IF EXISTS tmp1;"
        drop_tmp2 = "DROP TEMPORARY TABLE IF EXISTS tmp2;"
        drop_tmp3 = "DROP TEMPORARY TABLE IF EXISTS tmp3;"

        create_tmp1 =   "CREATE TEMPORARY TABLE tmp1 (id INTEGER NOT NULL AUTO_INCREMENT PRIMARY KEY) " \
                        "SELECT datetime, float_val " \
                        "FROM message_log " \
                        "WHERE source = %s " \
                        "AND datetime BETWEEN %s and %s " \
                        "ORDER BY datetime; " 
        create_tmp1_args = (block_label, startDTstr, endDTstr)

        create_tmp2 =   "CREATE TEMPORARY TABLE tmp2 " \
                        "SELECT * FROM tmp1; "

        create_tmp3 =   "CREATE TEMPORARY TABLE tmp3 " \
                        "SELECT * FROM tmp1; "

        main_query =    "SELECT A.datetime, A.float_val " \
                        "FROM tmp1 A, tmp2 P, tmp3 N " \
                        "WHERE A.id = P.id + 1 " \
                        "AND A.id = N.id - 1 " \
                        "AND ( " \
                        "        ( " \
                        "        A.float_val > P.float_val " \
                        "        AND A.float_val > N.float_val " \
                        "        ) " \
                        "    OR  ( " \
                        "        A.float_val < P.float_val " \
                        "        AND A.float_val < N.float_val " \
                        "        ) " \
                        "    OR A.id mod %s = 0 " \
                        ") " \
                        "ORDER BY A.datetime; "
        main_query_args = (sample_mod, )
                 
        cursor.execute(drop_tmp1)
        cursor.execute(drop_tmp2)
        cursor.execute(drop_tmp3)
        cursor.execute(create_tmp1, create_tmp1_args)
        cursor.execute(create_tmp2)
        cursor.execute(create_tmp3)
        cursor.execute(main_query, main_query_args)
               
        db_result_list = []
        record_count = 0
        for row in iter_row(cursor, 10):
            record_count += 1
            #print (row)
            td = row[0] - startDT
            tds = td.total_seconds()
            tdsi = int(tds)
            db_result_list.append( (tdsi, row[1]), )
        #print(db_result_list)
        print("Records retrieved = " + str(record_count))
    except Error as e:
        print(e) 
    try:
        cursor.close()
    except Error as error:
        print(error)
    try:
        conn.close()
    except Error as error:
        print(error)
    return db_result_list


def db_temperature_data(block_label, endDT=datetime.now(), \
                   periodTD=timedelta(days=1), db_result_list = [], db='mysql'):
    try:
        dbconfig = read_db_config(db=db)
        conn = MySQLConnection(**dbconfig)
        cursor = conn.cursor()
        startDT = endDT - periodTD
        startDTstr = startDT.strftime("%Y-%m-%d %H:%M:%S")
        endDTstr = endDT.strftime("%Y-%m-%d %H:%M:%S")
        sample_mod = periodTD.days
        if sample_mod == 2: sample_mod = 1
        elif sample_mod == 3: sample_mod = 5
        elif sample_mod == 4: sample_mod = sample_mod * 2
        elif sample_mod >= 5:
            sample_mod = sample_mod * 3
                
        query = "SELECT datetime, float_val from message_log " \
                "WHERE source = %s " \
                "AND datetime BETWEEN %s and %s " \
                "AND id mod %s = 0 " \
                "ORDER BY datetime"
        args = (block_label, startDTstr, endDTstr, sample_mod)
        cursor.execute(query, args)
        db_result_list = []
        record_count = 0
        for row in iter_row(cursor, 10):
            record_count += 1
            #print (row)
            td = row[0] - startDT
            tds = td.total_seconds()
            tdsi = int(tds)
            db_result_list.append( (tdsi, row[1]), )
        #print(db_result_list)
        print("Records retrieved = " + str(record_count))
    except Error as e:
        print(e) 
    try:
        cursor.close()
    except Error as error:
        print(error)
    try:
        conn.close()
    except Error as error:
        print(error)
    return db_result_list
        
def db_add_log_entry(message_buffer: deque, db='mysql'):  
    
    
    query = "INSERT INTO message_log " \
            "(datetime, date, time, source, destination, msg_type, message, int_val, float_val) " \
            "VALUES" \
            "(%(datetime)s, %(date)s, %(time)s, %(source)s,%(destination)s, " \
            "%(msg_type)s, %(message)s, %(int_val)s, %(float_val)s)"
    good_conn = False
    try:
        db_config = read_db_config(db=db)
        conn = MySQLConnection(**db_config) 
        if conn.is_connected(): 
            good_conn = True
            cursor = conn.cursor()
            records = 0
            while len(message_buffer) > 0:
                parsed_message = message_buffer.popleft()
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
                cursor.execute(query, log_data)
                records += 1
            conn.commit()
    except Error as error:
        print(error) 
    if good_conn:
        try:
            cursor.close()
        except Error as error:
            print(error)
        try:
            conn.close()
            if records > 1:
                print("(db_add_log_entry) sucessfully wrote " + str(records) + " records")
        except Error as error:
            print(error)
    pass

def read_db_config(filename='controller_config.ini', db='mysql'):
    """ Read database configuration file and return a dictionary object
    :param filename: name of the configuration file
    :param db: db of database configuration
    :return: a dictionary of database parameters
    """
    # create parser and read ini configuration file
    parser = ConfigParser()
    parser.read(filename)
 
    # get db, default to mysql
    db_config = {}
    if parser.has_section(db):
        items = parser.items(db)
        for item in items:
            db_config[item[0]] = item[1]
    else:
        raise Exception('{0} not found in the {1} file'.format(db, filename))
 
    return db_config

def DBConnectTest(db='mysql'):
    """ Connect to MySQL database """
    result = False
    db_config = read_db_config(db=db)
    #print (db_config)
    try:
        print('Testing connection to MySQL database: ' + db)
        conn = MySQLConnection(**db_config)
        if conn.is_connected():
            print('Database Connection Tested OK')
            result = True
        else:
            print('Database Connection Failed.')
            exit(1)
    except Error as error:
        print(error)
    finally:
        conn.close()
    return result
