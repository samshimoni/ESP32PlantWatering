# PlantWateringESP32
Plant Watering now with the ESP32 dev module

docker exec -it /bin/bash rabbit  

rabbitmq-plugins enable rabbitmq_mqtt

rabbitmqctl add_user user pass

rabbitmqctl set_permissions -p / user-test ".*" ".*" ".*"

rabbitmqctl set_user_tags user management
