#!/bin/bash

docker-compose up -d 

sleep 5

docker-compose exec rabbitmq rabbitmq-plugins enable rabbitmq_mqtt

sleep 20

docker-compose exec rabbitmq rabbitmqctl add_user samuel Samuel3471
docker-compose exec rabbitmq rabbitmqctl set_permissions -p / samuel ".*" ".*" ".*"
docker-compose exec rabbitmq rabbitmqctl set_user_tags samuel management


