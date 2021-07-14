FROM python:3
RUN apt-get update

RUN  python3 -m pip install --upgrade pip

RUN echo $(whoami)
RUN pip3 install pipenv --user
ENV PATH $PATH:/root/.local/bin

WORKDIR /usr/src/app

COPY watering_dashboard.py /usr/src/app

RUN pip3 install --user app \
dash===1.20.0 \     
dash-core-components===1.16.0 \     
dash-html-components===1.1.3  \    
dash-renderer===1.9.1 \
dash-table===4.11.3 \
plotly===5.0.0 \
pika===1.1.0       
      
EXPOSE 8080

CMD python3 watering_dashboard.py

