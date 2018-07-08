

from privatelib.db_funcs import db_device_data, \
                                DBConnectTest


############################################################################
# Main
############################################################################
if __name__ == '__main__':
       
    if DBConnectTest():
        q_res = []
        db_device_data(block_label="OUT_CIRCULATION_FAN", db_result_list = q_res)
        print (q_res)

