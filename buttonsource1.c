/*
 * buttonsource.c
 *
 *  Created on: Apr 3, 2025
 *      Author: M.O
 */
//#include<stdio.h>
#include<driver/gpio.h>
//#include "driver/i2c.h"
#include "esp_sleep.h"
#include "soc/gpio_num.h"
#include "ssd1306.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include<esp_log.h>
//#include "i2c_master.h" // old i2c driver
#include"driver/i2c_master.h"



#define CONFIG_OFFSETX 0  // or 2, or whatever the offset needs to be
void screen_on() 
{
    
    // Initialize the OLED display
    SSD1306_t dev;  // Define the SSD1306 display instance
    
    i2c_master_init(&dev, /*sda_gpio*/18 ,/*scl_gpio*/19,/*reset_gpio*/ -1);
    ssd1306_init(&dev, 126, 64); // Correct initialization

    // Clear the screen and display text
    ssd1306_clear_screen(&dev, false);
    ssd1306_display_text(&dev,2, "Hello!",6,false);  // Pass device handle and coordinates
    vTaskDelay(pdMS_TO_TICKS(9000));  // Wait 9 second
    ssd1306_clear_screen(&dev, false);
}

// Define row GPIO pins
#define ROW1_GPIO GPIO_NUM_1
#define ROW2_GPIO GPIO_NUM_2
#define ROW3_GPIO GPIO_NUM_3
#define ROW4_GPIO GPIO_NUM_4


// Define column GPIO pins
#define COL1_GPIO GPIO_NUM_34
#define COL2_GPIO GPIO_NUM_33
#define COL3_GPIO GPIO_NUM_26
#define COL4_GPIO GPIO_NUM_21


//Long press for Clear button
#define LONG_PRESS_TIME_MS 1000  // Define long press time in ms

//Long press for RESET button
#define LONG_PRESS_RESET_MS 3000   // Define long press time in ms for reset


void configure_buttons() {
	//Row Configuration
    gpio_config_t row1_conf = {
    .pin_bit_mask = (1ULL << ROW1_GPIO),
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .intr_type = GPIO_INTR_DISABLE
	};
	
	gpio_config_t row2_conf = {
	    .pin_bit_mask = (1ULL << ROW2_GPIO),
	    .mode = GPIO_MODE_INPUT,
	    .pull_up_en = GPIO_PULLUP_ENABLE,
	    .intr_type = GPIO_INTR_DISABLE
	};
	
	gpio_config_t row3_conf = {
	    .pin_bit_mask = (1ULL << ROW3_GPIO),
	    .mode = GPIO_MODE_INPUT,
	    .pull_up_en = GPIO_PULLUP_ENABLE,
	    .intr_type = GPIO_INTR_DISABLE
	};
	
	gpio_config_t row4_conf = {
	    .pin_bit_mask = (1ULL << ROW4_GPIO),
	    .mode = GPIO_MODE_INPUT,
	    .pull_up_en = GPIO_PULLUP_ENABLE,
	    .intr_type = GPIO_INTR_DISABLE
	};
	
	// Column configuration
	gpio_config_t col1_conf = {
	    .pin_bit_mask = (1ULL << COL1_GPIO),
	    .mode = GPIO_MODE_OUTPUT,
	    .pull_up_en = GPIO_PULLUP_DISABLE,
	    .intr_type = GPIO_INTR_DISABLE
	};
	
	gpio_config_t col2_conf = {
	    .pin_bit_mask = (1ULL << COL2_GPIO),
	    .mode = GPIO_MODE_OUTPUT,
	    .pull_up_en = GPIO_PULLUP_DISABLE,
	    .intr_type = GPIO_INTR_DISABLE
	};
	
	gpio_config_t col3_conf = {
	    .pin_bit_mask = (1ULL << COL3_GPIO),
	    .mode = GPIO_MODE_OUTPUT,
	    .pull_up_en = GPIO_PULLUP_DISABLE,
	    .intr_type = GPIO_INTR_DISABLE
	};
	
	gpio_config_t col4_conf = {
	    .pin_bit_mask = (1ULL << COL4_GPIO),
	    .mode = GPIO_MODE_OUTPUT,
	    .pull_up_en = GPIO_PULLUP_DISABLE,
	    .intr_type = GPIO_INTR_DISABLE
	};
	
	
    
    //Row pin GPIO CONFIG
    gpio_config(&row1_conf);
    gpio_config(&row2_conf);
    gpio_config(&row3_conf);
    gpio_config(&row4_conf);
    
    
    //Column pin GPIO CONFIG
    gpio_config(&col1_conf);
	gpio_config(&col2_conf);
	gpio_config(&col3_conf);
	gpio_config(&col4_conf);
    
    
   
    
    
}


void scan_keypad() {
    // Loop through all columns
    for (int col = 0; col < 4; col++) {
        // Set all columns to high (disable them)
        gpio_set_level(COL1_GPIO, 1);
        gpio_set_level(COL2_GPIO, 1);
        gpio_set_level(COL3_GPIO, 1);
        gpio_set_level(COL4_GPIO, 1);

        // Set one column to low (enable it)
        switch (col) {
            case 1:
                gpio_set_level(COL1_GPIO, 0);
                break;
            case 2:
                gpio_set_level(COL2_GPIO, 0);
                break;
            case 3:
                gpio_set_level(COL3_GPIO, 0);
                break;
            case 4:
                gpio_set_level(COL4_GPIO, 0);
                break;
        }

        // Check if any row is pressed (low level)
        if (gpio_get_level(ROW1_GPIO) == 0) {
            printf("Button in column %d, row 1 pressed\n", col);
        }
        if (gpio_get_level(ROW2_GPIO) == 0) {
            printf("Button in column %d, row 2 pressed\n", col);
        }
        if (gpio_get_level(ROW3_GPIO) == 0) {
            printf("Button in column %d, row 3 pressed\n", col);
        }
        if (gpio_get_level(ROW4_GPIO) == 0) {
            printf("Button in column %d, row 4 pressed\n", col);
        }
    }
}


void handle_key_action(int key) {
    // Define key mapping based on row and column
    char keypad_map[4][4] = {
        {'1', '2', '3', '4'},
        {'5', '6', '7', '8'},
        {'9', '0', 'E', 'C'},
        {'R', 'e', ' ', ' '}//small e for empty button with no writing 
    };

    if (key != -1) {
        char pressed_button = keypad_map[key / 4][key % 4];  // Map key to character
		SSD1306_t dev;// to Define the SSD1306 display instance dev/&dev
        // Perform actions based on the button pressed
        
        char input_number[10];  // Assuming max 10 digits
		int input_index = 0;    // To track the index for the next digit
		static uint32_t press_start_time = 0;  // Declare globally
		//static bool button_pressed = false;    // To track if the button is being pressed
        switch (pressed_button) {
			
            case '1':
                // Action for 1
                printf("Button 1 pressed\n");
                ssd1306_display_text(&dev, 0, "1", 1, false);
                input_number[input_index++] = '1';  // Append '1' to input_number
    			input_number[input_index] = '\0';  // Null-terminate the string
                break;
                
            case '2':
                // Action for 2
                printf("Button 2 pressed\n");
                ssd1306_display_text(&dev,0, "2", 1, false);
                input_number[input_index++] = '2';  // Append '1' to input_number
    			input_number[input_index] = '\0';  // Null-terminate the string
                break;
                
            case '3':
                // Action for 3
                printf("Button 3 pressed\n");
                ssd1306_display_text(&dev, 0, "3", 1, false);
                input_number[input_index++] = '3';  // Append '1' to input_number
    			input_number[input_index] = '\0';  // Null-terminate the string
                break;
                
            case '4':
                // Action for 4
                printf("Button 4 pressed\n");
                ssd1306_display_text(&dev, 0, "4", 1, false);
                input_number[input_index++] = '4';  // Append '1' to input_number
    			input_number[input_index] = '\0';  // Null-terminate the string
                break;
                
            case '5':
                // Action for 5
                printf("Button 5 pressed\n");
                ssd1306_display_text(&dev, 0, "5", 1, false);
                input_number[input_index++] = '5';  // Append '1' to input_number
    			input_number[input_index] = '\0';  // Null-terminate the string
                break;
                
            case '6':
                // Action for 6
                printf("Button 6 pressed\n");
                ssd1306_display_text(&dev, 0, "6", 1, false);
                input_number[input_index++] = '6';  // Append '1' to input_number
    			input_number[input_index] = '\0';  // Null-terminate the string
                break;   
       	
            case '7':
                // Action for 7
                printf("Button 7 pressed\n");
                ssd1306_display_text(&dev, 0, "7", 1, false);
                input_number[input_index++] = '7';  // Append '1' to input_number
    			input_number[input_index] = '\0';  // Null-terminate the string
                break;
                
            case '8':
                // Action for 8
                printf("Button 8 pressed\n");
                ssd1306_display_text(&dev, 0, "8", 1, false);
                input_number[input_index++] = '8';  // Append '1' to input_number
    			input_number[input_index] = '\0';  // Null-terminate the string
                break;
                
            case '9':
                // Action for 9
                printf("Button 9 pressed\n");
                ssd1306_display_text(&dev, 0, "9", 1, false);
                input_number[input_index++] = '1';  // Append '1' to input_number
    			input_number[input_index] = '\0';  // Null-terminate the string
                break;
                
            case '0':
                // Action for 0
                printf("Button 0 pressed\n");
                ssd1306_display_text(&dev, 0, "0", 1, false);
                input_number[input_index++] = '0';  // Append '1' to input_number
    			input_number[input_index] = '\0';  // Null-terminate the string
                break;
             
                
            case 'E':
                // Action for E
                printf("Button Enter pressed\n");
                
				input_number[input_index] = '\0';  // Null-terminate the string
			
				if (strcmp(input_number, "ADD condition") == 0) {  // Replace "1234" with your correct code
				       //Add action taken
				    } else {
				        //Add action taken
				 }
				break;
                
            case 'C':
                // Action for C
                printf("Button Clear pressed\n");
                //ssd1306_display_text(&dev, 0, "CLEAR", 1, false);
                
               static bool clear_pressed = false;
               if (key == 'C' && !clear_pressed) {
		    	// Button just pressed
		       press_start_time = xTaskGetTickCount();
		       clear_pressed = true;
			   }
			   
			   if (key != 'C' && clear_pressed) {
			    // Button just released
			    uint32_t press_duration = xTaskGetTickCount() - press_start_time;
			    clear_pressed = false;
			
				    if (press_duration < pdMS_TO_TICKS(LONG_PRESS_TIME_MS)) {
				      // Short press: Delete last digit
				    	if (input_index > 0) {
				    		input_number[--input_index] = '\0';  // Remove last character
							printf("Current input: %s\n", input_number);  // For debugging
				        }
				    }
				    if (press_duration >= pdMS_TO_TICKS(LONG_PRESS_TIME_MS)) {
				       // Long press: Clear the screen
				        ssd1306_clear_screen(&dev, true);
				    } 
			   }   
                break;
                
            case 'R':
                // Action for R
                printf("Button Reset pressed\n");
                ssd1306_display_text(&dev, 0, "RESET", 1, false);
                if (key == 'R') {  // Assuming 'R' is the "RESET" button
				   
					press_start_time = xTaskGetTickCount();  // Capture start time when button is pressed
					uint32_t press_duration = xTaskGetTickCount() - press_start_time;  // Calculate how long the button has been pressed
					
					if (press_duration >= LONG_PRESS_RESET_MS) {
					  // Long press: Restart system
					    esp_restart();
					}
				}  
                break;
                
            case 'e':
                // Action for e
                printf("Button empty pressed\n");
                //empty button use for whatever
                break;
                
             // Continue for other buttons...
            default:
                printf("Unknown key pressed\n");
                break;
        }
    }
}


#define WAKEUP_MASK  ((1ULL << ROW1_GPIO) | (1ULL << ROW2_GPIO) | (1ULL << ROW3_GPIO) | (1ULL << ROW4_GPIO) | \
                      (1ULL << COL1_GPIO) | (1ULL << COL2_GPIO) | (1ULL << COL3_GPIO) | (1ULL << COL4_GPIO)|(1ULL << TOUCHOUT_GPIO))
                      //This is a bitmask that includes all the GPIOs of the keypad matrix (rows and columns

void power_manager(){
	
	SSD1306_t dev;
	const int sleep_delay_sec = 180;  // 3 minutes
    bool cancel_sleep = false;
    
    for (int i = sleep_delay_sec; i > 0; i--) {
		
        // Check if any key GPIO is LOW (pressed)
        if ((gpio_get_level(ROW1_GPIO) == 0) || (gpio_get_level(ROW2_GPIO) == 0) || 
            (gpio_get_level(ROW3_GPIO) == 0) || (gpio_get_level(ROW4_GPIO) == 0) ||
            (gpio_get_level(COL1_GPIO) == 0) || (gpio_get_level(COL2_GPIO) == 0) ||
            (gpio_get_level(COL3_GPIO) == 0) || (gpio_get_level(COL4_GPIO) == 0)||(gpio_get_level(TOUCHOUT_GPIO) == 0)) {
            cancel_sleep = true;
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));  // Wait 1 second
    }
	if (cancel_sleep == true) {
        //ssd1306_display_text(&dev, 2, "Sleep cancelled", 15, false);
        vTaskDelay(pdMS_TO_TICKS(2000));
        //ssd1306_clear_screen(&dev, false);
        return;
    }
	
	
	
	esp_sleep_enable_ext1_wakeup(WAKEUP_MASK, ESP_EXT1_WAKEUP_ANY_LOW);//using ESP_EXT1_WAKEUP_ALL_LOW instead of ESP_EXT1_WAKEUP_ALL_HIGH since i have internal pull up enabled on button config, buttons are detected when low. 
	
	printf("Going to sleep now.");
	ssd1306_display_text(&dev,2, "Going to sleep now.",21,false);
	
	
 	vTaskDelay(pdMS_TO_TICKS(2000));  // Delay for 2 seconds before clearing screen
	
	
	
	esp_deep_sleep_start(); //puts the ESP32 into deep sleep, awaiting a wake-up event 
	
}
