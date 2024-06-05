idf_component_register(SRCS "motor.cpp" "EC11.cpp" "hello_world_main.c" "oled.cpp" "ui.c" "motor.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES driver ,esp_timer)
