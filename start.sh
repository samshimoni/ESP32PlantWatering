#!/bin/bash

docker-compose up -d 


sleep 5

docker-compose exec rabbitmq rabbitmq-plugins enable rabbitmq_mqtt
docker-compose exec rabbitmq rabbitmqctl set_user_tags mqtt-test management
