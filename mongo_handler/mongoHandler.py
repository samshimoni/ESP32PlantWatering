import pymongo
from datetime import datetime
import re
import pika


myclient = pymongo.MongoClient("mongodb://10.0.0.10:27017/")
mydb = myclient["plant_watering"]
HumidityCollection = mydb["Humidity"]


connection = pika.BlockingConnection(pika.ConnectionParameters(host='10.0.0.10', port=5672))
channel = connection.channel()

channel.exchange_declare(exchange='amq.topic', exchange_type='topic', durable=True)
result = channel.queue_declare('', exclusive=True)
queue_name = result.method.queue

channel.queue_bind(exchange='amq.topic', queue=queue_name, routing_key='#')

def callback(ch, method, properties, body):   
    if body.decode().startswith('plant'):   
        data = {
            "plant" : int(body.decode()[5]),
            "Humidity" : int(body.decode().split(':')[-1]),
            "time":  datetime.now()
            }
        HumidityCollection.insert_one(data)
        for i in HumidityCollection.find():
            print(i)
channel.basic_consume(queue=queue_name, on_message_callback=callback, auto_ack=True)
channel.start_consuming()
