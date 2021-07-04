import dash
from dash.dependencies import Output, Input
import dash_core_components as dcc
import dash_html_components as html
import plotly
import random
import plotly.graph_objs as go
from collections import deque
import pika
import sys
import threading
from time import gmtime, strftime

MAXLEN = 30

X_AXIS = deque(maxlen=MAXLEN)
X_AXIS.append(strftime("%H:%M", gmtime()))

Plant1_Y = deque(maxlen=MAXLEN)
Plant2_Y = deque(maxlen=MAXLEN)
Plant3_Y = deque(maxlen=MAXLEN)

Plant1_Y.append(0)
Plant2_Y.append(0)
Plant3_Y.append(0)

def callback(ch, method, properties, body):
    if body.decode().startswith('plant'):
        time = strftime("%H:%M", gmtime())
        if time not in X_AXIS:
            X_AXIS.append(time) 

        if body.decode().startswith('plant1'):
            Plant1_Y.append(int(body.decode().split(':')[-1]))

        if body.decode().startswith('plant2'):
            Plant2_Y.append(int(body.decode().split(':')[-1]))

        if body.decode().startswith('plant3'):
            Plant3_Y.append(int(body.decode().split(':')[-1]))
            
connection = pika.BlockingConnection(pika.ConnectionParameters(host='localhost', port=30000))
channel = connection.channel()

channel.exchange_declare(exchange='amq.topic', exchange_type='topic', durable=True)
result = channel.queue_declare('', exclusive=True)
queue_name = result.method.queue

channel.queue_bind(exchange='amq.topic', queue=queue_name, routing_key='#')
channel.basic_consume(queue=queue_name, on_message_callback=callback, auto_ack=True)

thread = threading.Thread(target = channel.start_consuming)

app = dash.Dash(_name_) 
app.layout = html.Div(
    [
        dcc.Graph(id='live-graph', animate=True),
        dcc.Interval(
            id='graph-update',
            interval=1000 * 1 * 30
        ),
    ]
)
@app.callback(Output('live-graph', 'figure'),[Input('graph-update', 'n_intervals')])
def update_graph_scatter(input_data):
    data_plant_1 = plotly.graph_objs.Scatter(x=X_AXIS, y=Plant1_Y, name='Plant1', mode= 'lines+markers')
    data_plant_2 = plotly.graph_objs.Scatter(x=X_AXIS, y=Plant2_Y, name='Plant2', mode= 'lines+markers')
    data_plant_3 = plotly.graph_objs.Scatter(x=X_AXIS, y=Plant3_Y, name='Plant3', mode= 'lines+markers')

    return {'data': [data_plant_1, data_plant_2, data_plant_3],
            'layout' : go.Layout(xaxis=dict(range=[min(X_AXIS),max(X_AXIS)]),yaxis=dict(range=[0,110]),)}

if _name_ == '_main_':
    thread.start()
    app.run_server(host='0.0.0.0', port=8080)