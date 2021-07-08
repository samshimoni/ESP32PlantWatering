#!/bin/bash

docker-compose up -d 


sleep 5

docker-compose exec rabbitmq rabbitmq-plugins enable rabbitmq_mqtt

sleep 20

docker-compose exec rabbitmq rabbitmqctl add_user mqtt-test mqtt-test
docker-compose exec rabbitmq rabbitmqctl set_permissions -p / mqtt-test ".*" ".*" ".*"
docker-compose exec rabbitmq rabbitmqctl set_user_tags mqtt-test management
