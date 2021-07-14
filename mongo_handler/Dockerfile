FROM python:3
RUN apt-get update

WORKDIR /usr/src/app

COPY mongoHandler.py /usr/src/app

RUN pip3 install --user app \
pymongo==3.11.4 \
pika===1.1.0 

EXPOSE 30000
EXPOSE 27017

CMD python3 mongoHandler.py
