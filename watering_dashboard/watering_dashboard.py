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
from datetime import datetime



MAXLEN = 30

X_AXIS = deque(maxlen=MAXLEN)
time = datetime.now()
X_AXIS.append(time.strftime('%H:%M'))

Plant1_Y = deque(maxlen=MAXLEN)
Plant2_Y = deque(maxlen=MAXLEN)
Plant3_Y = deque(maxlen=MAXLEN)

Plant1_Y.append(0)
Plant2_Y.append(0)
Plant3_Y.append(0)

def callback(ch, method, properties, body):
    if body.decode().startswith('plant'):
        time = datetime.now().strftime('%H:%M')
        if time not in X_AXIS:
            X_AXIS.append(time)
        if body.decode().startswith('plant1'):
            Plant1_Y.append(int(body.decode().split(':')[-1]))

        if body.decode().startswith('plant2'):
            Plant2_Y.append(int(body.decode().split(':')[-1]))

        if body.decode().startswith('plant3'):
            Plant3_Y.append(int(body.decode().split(':')[-1]))

connection = pika.BlockingConnection(pika.ConnectionParameters(host='localhost', port=5672))
channel = connection.channel()

channel.exchange_declare(exchange='amq.topic', exchange_type='topic', durable=True)
result = channel.queue_declare('', exclusive=True)
queue_name = result.method.queue

channel.queue_bind(exchange='amq.topic', queue=queue_name, routing_key='#')
channel.basic_consume(queue=queue_name, on_message_callback=callback, auto_ack=True)

thread = threading.Thread(target = channel.start_consuming)

app = dash.Dash(__name__) 
app.layout = html.Div(
    [
        dcc.Graph(id='live-graph', animate=True),
        dcc.Interval(
            id='graph-update',
            interval=1000 * 60
        ),
    ]
)
@app.callback(Output('live-graph', 'figure'),[Input('graph-update', 'n_intervals')])
def update_graph_scatter(input_data):
    
    global Plant1_Y
    global Plant2_Y
    global Plant3_Y
    global X_AXIS
    
    X = list(X_AXIS)
    Y1 = list(Plant1_Y)
    Y2= list(Plant2_Y)
    Y3 = list(Plant3_Y)
 
    data_plant_1 = plotly.graph_objs.Scatter(x=X, y=Y1, name='Plant1', mode= 'lines+markers')
    data_plant_2 = plotly.graph_objs.Scatter(x=X, y=Y2, name='Plant2', mode= 'lines+markers')
    data_plant_3 = plotly.graph_objs.Scatter(x=X, y=Y3, name='Plant3', mode= 'lines+markers')

    return {'data': [data_plant_1, data_plant_2, data_plant_3],
            'layout' : go.Layout(xaxis=dict(range=[min(X),max(X)]),yaxis=dict(range=[0,100]),)}

if __name__ == '__main__':
    thread.start()
    app.run_server(host='0.0.0.0', port=8080)
