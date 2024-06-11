#include "wiced.h"

#define LEFT_MOTOR_IN1 WICED_GPIO_1 // J10:1
#define LEFT_MOTOR_IN2 WICED_GPIO_2 // J10:2
#define RIGHT_MOTOR_IN3 WICED_GPIO_9 // J10:3
#define RIGHT_MOTOR_IN4 WICED_GPIO_10 // J10:5
#define LEFT_MOTOR_PWM WICED_PWM_1 // J12:4
#define RIGHT_MOTOR_PWM WICED_PWM_2 // J12:3

#define TCP_CLIENT_STACK_SIZE (6200)
#define SERVER_PORT (7000)

void go_forwards() {
    wiced_gpio_output_high(LEFT_MOTOR_IN1);
    wiced_gpio_output_low(LEFT_MOTOR_IN2);
    wiced_gpio_output_high(RIGHT_MOTOR_IN3);
    wiced_gpio_output_low(RIGHT_MOTOR_IN4);

    float left_motor_duty_cycle = 50.0;
    float right_motor_duty_cycle = 60.0;

    wiced_pwm_init(LEFT_MOTOR_PWM, 2000, left_motor_duty_cycle);
    wiced_pwm_init(RIGHT_MOTOR_PWM, 2000, right_motor_duty_cycle);

    wiced_pwm_start(LEFT_MOTOR_PWM);
    wiced_pwm_start(RIGHT_MOTOR_PWM);
}

void go_backwards() {
    wiced_gpio_output_low(LEFT_MOTOR_IN1);
    wiced_gpio_output_high(LEFT_MOTOR_IN2);
    wiced_gpio_output_low(RIGHT_MOTOR_IN3);
    wiced_gpio_output_high(RIGHT_MOTOR_IN4);

    float left_motor_duty_cycle = 50.0;
    float right_motor_duty_cycle = 60.0;

    wiced_pwm_init(LEFT_MOTOR_PWM, 2000, left_motor_duty_cycle);
    wiced_pwm_init(RIGHT_MOTOR_PWM, 2000, right_motor_duty_cycle);

    wiced_pwm_start(LEFT_MOTOR_PWM);
    wiced_pwm_start(RIGHT_MOTOR_PWM);
}

void turn_left() {
    wiced_gpio_output_high(LEFT_MOTOR_IN1);
    wiced_gpio_output_low(LEFT_MOTOR_IN2);
    wiced_gpio_output_low(RIGHT_MOTOR_IN3);
    wiced_gpio_output_high(RIGHT_MOTOR_IN4);

    float left_motor_duty_cycle = 40.0;
    float right_motor_duty_cycle = 50.0;

    wiced_pwm_init(LEFT_MOTOR_PWM, 2000, left_motor_duty_cycle);
    wiced_pwm_init(RIGHT_MOTOR_PWM, 2000, right_motor_duty_cycle);

    wiced_pwm_start(LEFT_MOTOR_PWM);
    wiced_pwm_start(RIGHT_MOTOR_PWM);
}

void turn_right() {
    wiced_gpio_output_low(LEFT_MOTOR_IN1);
    wiced_gpio_output_high(LEFT_MOTOR_IN2);
    wiced_gpio_output_high(RIGHT_MOTOR_IN3);
    wiced_gpio_output_low(RIGHT_MOTOR_IN4);

    float left_motor_duty_cycle = 40.0;
    float right_motor_duty_cycle = 50.0;

    wiced_pwm_init(LEFT_MOTOR_PWM, 2000, left_motor_duty_cycle);
    wiced_pwm_init(RIGHT_MOTOR_PWM, 2000, right_motor_duty_cycle);

    wiced_pwm_start(LEFT_MOTOR_PWM);
    wiced_pwm_start(RIGHT_MOTOR_PWM);
}

void stop() {
    wiced_gpio_output_low(LEFT_MOTOR_IN1);
    wiced_gpio_output_low(LEFT_MOTOR_IN2);
    wiced_gpio_output_low(RIGHT_MOTOR_IN3);
    wiced_gpio_output_low(RIGHT_MOTOR_IN4);
}

void application_start()
{
    wiced_init(); /* Initialize the WICED device */

    wiced_gpio_init(LEFT_MOTOR_IN1, OUTPUT_PUSH_PULL);
    wiced_gpio_init(LEFT_MOTOR_IN2, OUTPUT_PUSH_PULL);
    wiced_gpio_init(RIGHT_MOTOR_IN3, OUTPUT_PUSH_PULL);
    wiced_gpio_init(RIGHT_MOTOR_IN4, OUTPUT_PUSH_PULL);

    wiced_network_up(WICED_STA_INTERFACE,
    WICED_USE_EXTERNAL_DHCP_SERVER, NULL);

    wiced_result_t result;
    wiced_ip_address_t serverAddress; //Server IP address

    result = wiced_hostname_lookup("server.py",
    &serverAddress, 5000, WICED_STA_INTERFACE );

    if (result == WICED_ERROR || serverAddress.ip.v4 == 0)
    {
        WPRINT_APP_INFO(("Error in resolving DNS using hard coded address\n"));

        // Replace this with the IP address of the server
        // running on your machine
        SET_IPV4_ADDRESS(serverAddress, MAKE_IPV4_ADDRESS(10, 16, 230, 251)); // alextro IP
//         SET_IPV4_ADDRESS(serverAddress, MAKE_IPV4_ADDRESS( 10, 16, 230, 124)); // Leon Laptop IP
    }

    wiced_tcp_socket_t socket; // The TCP socket
    wiced_tcp_stream_t stream; // The TCP stream
    wiced_result_t conStatus;

    // Open the connection to the remote server via a socket
    wiced_tcp_create_socket(&socket, WICED_STA_INTERFACE);
    wiced_tcp_bind(&socket, WICED_ANY_PORT);
    conStatus = wiced_tcp_connect(&socket, &serverAddress, SERVER_PORT, 2000);
    // 2 second timeout

    if (conStatus == WICED_SUCCESS)
        WPRINT_APP_INFO(("Successful connection!\n"));
    else {
        WPRINT_APP_INFO(("Failed connection!\n"));
        wiced_tcp_delete_socket(&socket);
        return;
    }

    wiced_tcp_stream_init(&stream, &socket);
    char rbuffer[2] = {0};

    while (1) {
        // Get the response back from the server
        uint32_t read_count;
        result = wiced_tcp_stream_read_with_count(&stream, rbuffer, 1, WICED_WAIT_FOREVER,
        &read_count);

        if (rbuffer[0] == 'F') {
            WPRINT_APP_INFO(("Forwards\n"));

            go_forwards();

            wiced_gpio_output_low(WICED_LED1);
            wiced_gpio_output_low(WICED_LED2);
        }
        else if (rbuffer[0] == 'B') {
            WPRINT_APP_INFO(("Backwards\n"));

            go_backwards();

            wiced_gpio_output_high(WICED_LED1);
            wiced_gpio_output_high(WICED_LED2);
        }
        else if (rbuffer[0] == 'L') {
            WPRINT_APP_INFO(("Left\n"));

            turn_left();

            wiced_gpio_output_low(WICED_LED1);
            wiced_gpio_output_high(WICED_LED2);
        }
        else if (rbuffer[0] == 'R') {
            WPRINT_APP_INFO(("Right\n"));

            turn_right();

            wiced_gpio_output_high(WICED_LED1);
            wiced_gpio_output_low(WICED_LED2);
        }
        else if (rbuffer[0] == 'S') {
            WPRINT_APP_INFO(("Stop\n"));

            stop();

            wiced_gpio_output_low(WICED_LED1);
            wiced_gpio_output_low(WICED_LED2);
        }
    }
}
