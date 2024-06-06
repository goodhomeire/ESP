idf_component_register(SRCS  "main.cpp"
                    INCLUDE_DIRS "." "../components/oled" "../components/EC11"
                    REQUIRES driver esp_timer)

