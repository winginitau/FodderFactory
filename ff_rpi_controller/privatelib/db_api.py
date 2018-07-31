'''
Created on 29 Jul. 2018

@author: brendan
'''

from privatelib.ff_config import INPUTS_LIST, OUTPUTS_LIST, ENERGY_LIST

from privatelib.db_funcs import db_temperature_data ,db_device_data, \
                                db_energy_data, db_log_history, \
                                db_energy_data_new, db_temperature_data_new

from flask import Flask, request, jsonify
from flask_restful import Resource, Api
#from sqlalchemy import create_engine
from json import dumps
from _datetime import timedelta

app = Flask(__name__)
api = Api(app)

class TemperatureInputs(Resource):
    def get(self):
        result = {'temperature_inputs': []}
        for _i, block_label, _display, _graph_period in INPUTS_LIST:
            result['temperature_inputs'].append(block_label)
        jresult = jsonify(result)
        return jresult

class TemperatureData(Resource):
    def get(self, label, days=1):
        db_result = []
        db_result = db_temperature_data_new(label, periodTD=timedelta(days=int(days)), db_result_list=db_result)
        result = {'data' : []}
        for secs, temp in db_result:
            result['data'].append((secs, temp,))
        jresult = jsonify(result)
        return jresult
        
api.add_resource(TemperatureInputs, '/temperature_inputs')
api.add_resource(TemperatureData, '/temperature_data/<label>,<days>')
    
if __name__ == '__main__':
    app.run(host='0.0.0.0', port='5002', debug=True, use_reloader=False)
