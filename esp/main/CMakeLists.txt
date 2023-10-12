idf_component_register(SRCS "main.c" "src/ble.c" "src/wifi.c" "src/http_client.c" "src/door.c"
                       INCLUDE_DIRS "."
                       EMBED_TXTFILES "certs/certificate.pem")
